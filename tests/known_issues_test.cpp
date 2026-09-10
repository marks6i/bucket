// known_issues_test.cpp
//
// This file documents (and reproduces) latent bugs/quirks discovered in the
// library headers while writing the dev10 coverage test suite. None of
// these are fixed here -- library source is intentionally left untouched
// in this pass so the behavior/fix can be reviewed and decided on
// separately (dev10 is expected to become release 1.1).
//
// Every test below is prefixed with DISABLED_ so the normal `ctest`/
// `gtest_discover_tests` run stays green. To see them fail (and confirm the
// bug still reproduces) run the test binary directly with:
//
//     ./known_issues_test --gtest_also_run_disabled_tests
//
// or select just one, e.g.:
//
//     ./known_issues_test --gtest_also_run_disabled_tests \
//         --gtest_filter=*CaseInsensitiveLess*
//
// Each TEST describes the *expected/desired* behavior; as written, they
// currently FAIL against the unmodified library, which is the point --
// they exist so a fix can be validated against them (just remove the
// DISABLED_ prefix once the underlying bug is fixed).

#include <gtest/gtest.h>

#include "../include/buckets.h"
#include "../include/buckets_supp.h"
#include "../include/compare_traits.h"

#include <sstream>
#include <string>
#include <vector>

using namespace masutils;

// ---------------------------------------------------------------------
// Bug 1: compare_traits_descending<E>'s inherited gt/le/ge do not use the
// overridden (descending) lt().
//
// gt/le/ge are ordinary (non-virtual, non-CRTP) static methods defined in
// the base compare_traits<E> template; their bodies call the unqualified
// `lt(...)`, which the compiler resolves at compile time to
// compare_traits<E>::lt -- *not* compare_traits_descending<E>::lt -- even
// though the call is made through compare_traits_descending<E>::gt/le/ge.
// So a caller relying on gt/le/ge (rather than lt directly) to respect
// "descending" order will get ascending-order results instead.
//
// A conforming fix would need CRTP (e.g. compare_traits<E, Derived>) or to
// redeclare gt/le/ge in compare_traits_descending in terms of its own lt.
// ---------------------------------------------------------------------
TEST(KnownIssuesTest, DISABLED_DescendingTraitsGtLeGeShouldUseOverriddenLt) {
    // If gt/le/ge correctly dispatched through the derived (descending) lt,
    // these would hold (derived lt(x,y) means x > y):
    EXPECT_TRUE(compare_traits_descending<int>::gt(3, 5));  // desired: 3 "descending-gt" 5 == lt_desc(5,3) == true
    EXPECT_TRUE(compare_traits_descending<int>::le(5, 3));  // desired: !lt_desc(3,5) == true
    EXPECT_TRUE(compare_traits_descending<int>::ge(3, 5));  // desired: !lt_desc(3,5) == true

    // As currently implemented, all three instead silently fall back to
    // the base (ascending) lt(), producing:
    //   gt(3, 5) == false   (uses base lt(5,3) == false)
    //   le(5, 3) == false   (uses base lt(3,5) == true -> !true == false)
    //   ge(3, 5) == false   (uses base lt(3,5) == true -> !true == false)
}

// ---------------------------------------------------------------------
// Bug 2: endRange()/rendRange() build their search sentinel from
// end_range alone (ignoring start_range), which can wrongly exclude a
// bucket that genuinely overlaps [start_range, end_range) if end_range
// happens to fall strictly inside that bucket.
// ---------------------------------------------------------------------
TEST(KnownIssuesTest, DISABLED_EndRangeShouldIncludeBucketContainingEndRangePoint) {
    buckets<int, int> b;
    b.spread(0, 10, 1);
    b.spread(10, 20, 2);
    b.spread(20, 30, 3);
    b.spread(40, 50, 4);

    // Query range [15, 25): this genuinely overlaps both [10,20) and
    // [20,30) (the latter over [20,25)).
    std::vector<int> lows;
    for (auto it = b.beginRange<false>(15, 25); it != b.endRange<false>(15, 25); ++it) {
        lows.push_back(it->first);
    }

    // Desired/correct result: both overlapping buckets are visited.
    EXPECT_EQ(lows, (std::vector<int>{10, 20}));

    // As currently implemented, endRange(15, 25) computes its sentinel by
    // searching for the first bucket overlapping the single point 25,
    // which is [20,30) itself -- so that bucket becomes the (excluded)
    // sentinel and iteration stops too early, yielding just {10}.
}

// ---------------------------------------------------------------------
// Bug 3: caseInsensitiveLess<T> never falls back to comparing lengths, so
// strings that share a common prefix but differ in length compare as
// mutually "not less than" (i.e. equivalent), rather than the shorter one
// sorting first as normal lexicographic order would dictate.
// ---------------------------------------------------------------------
TEST(KnownIssuesTest, DISABLED_CaseInsensitiveLessShouldUseLengthAsTiebreaker) {
    caseInsensitiveLess<std::string> less;

    // Desired/correct (normal lexicographic) behavior: a strict prefix
    // sorts before the longer string it is a prefix of.
    EXPECT_TRUE(less("app", "apple"));
    EXPECT_FALSE(less("apple", "app"));

    // As currently implemented, operator() only compares the first
    // min(lhs.size(), rhs.size()) characters case-insensitively and
    // returns false if they are all equal -- so less("app", "apple") is
    // false, not true, treating "app" and "apple" as equivalent keys.
}

// ---------------------------------------------------------------------
// Bug 4: bucket_value_wrapper<T> (and its std::basic_string/char*
// specializations) store a `const T&` bound directly to the constructor's
// `const T&` parameter, rather than a copy. Constructing one directly from
// a temporary/prvalue leaves a dangling reference once the temporary is
// destroyed at the end of the full expression -- undefined behavior.
//
// NOTE: this test intentionally reproduces real UB. In practice it tends
// to read stale-but-still-present stack/heap bytes for a plain int, but
// for std::string it can throw std::bad_alloc, corrupt the heap, or crash
// the entire process outright (not just fail this one test case) --
// because it is UB, the exact symptom is unpredictable and may vary by
// compiler, optimization level, or standard library implementation. It is
// disabled by default for that reason; only run it deliberately, in
// isolation, if you want to observe/confirm the hazard for yourself:
//
//     ./known_issues_test --gtest_also_run_disabled_tests \
//         --gtest_filter=*DanglingReference*
// ---------------------------------------------------------------------
TEST(KnownIssuesTest, DISABLED_BucketValueWrapperDanglesWhenBoundToTemporary) {
    // Constructed directly from a temporary int -- the temporary is
    // destroyed once the constructor returns, so `wrapper` holds a
    // dangling reference even though this looks like innocuous code.
    bucket_value_wrapper<int> wrapper(42);

    std::ostringstream oss;
    oss << wrapper; // UB: reads through the dangling reference
    EXPECT_EQ(oss.str(), "42");
    EXPECT_EQ(wrapper.get(), 42);
}

// ---------------------------------------------------------------------
// Bug 5: range_iterator<IsConst>::operator*() and operator->() are
// declared to always return non-const triplet_type&/triplet_type*,
// regardless of IsConst. This means the const (IsConst = true)
// instantiation of beginRange/endRange/rbeginRange/rendRange -- which the
// public API otherwise exposes as a usable template argument -- fails to
// compile as soon as it is actually instantiated, because operator->()
// tries to implicitly convert a `const triplet_type*` (from a
// const_iterator) to `triplet_type*`.
//
// This can't be demonstrated as a DISABLED_ runtime test (it's a compile
// error, not a runtime failure), so it is left here as commented-out code
// for you to uncomment and confirm/investigate directly. As of this
// writing, uncommenting the block below fails to build with:
//
//   error: invalid conversion from
//   'const masutils::triplet<int, int, ...>*' to
//   'masutils::buckets<int, int>::triplet_type*' [-fpermissive]
//
// TEST(KnownIssuesTest, DISABLED_ConstRangeIteratorInstantiationCompiles) {
//     buckets<int, int> b;
//     b.spread(0, 10, 1);
//     b.spread(10, 20, 2);
//
//     std::vector<int> lows;
//     for (auto it = b.beginRange<true>(0, 20); it != b.endRange<true>(0, 20); ++it) {
//         lows.push_back(it->first);
//     }
//     EXPECT_EQ(lows, (std::vector<int>{0, 10}));
// }
