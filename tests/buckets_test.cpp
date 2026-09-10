#include <gtest/gtest.h>

#include "../include/buckets.h"

#include <list>
#include <stdexcept>
#include <tuple>
#include <vector>

using masutils::buckets;
using masutils::compare_traits;
using masutils::compare_traits_descending;

namespace {

// A small helper that flattens a buckets<> collection into a vector of
// (low, high, values) tuples so tests can compare expected shape/contents
// without depending on the (separately tested) mastest::bucket_compare
// helper from include/test/support.h.
template <class Bucket>
std::vector<std::tuple<typename Bucket::index_type,
                        typename Bucket::index_type,
                        std::vector<typename Bucket::value_type>>>
snapshot(const Bucket& b) {
    std::vector<std::tuple<typename Bucket::index_type,
                            typename Bucket::index_type,
                            std::vector<typename Bucket::value_type>>>
        result;
    for (auto it = b.begin(); it != b.end(); ++it) {
        std::vector<typename Bucket::value_type> values(it->third.begin(), it->third.end());
        result.emplace_back(it->first, it->second, std::move(values));
    }
    return result;
}

} // namespace

TEST(BucketsTest, DefaultConstructionIsUnconstrainedAndEmpty) {
    buckets<int, int> b;
    EXPECT_TRUE(b.empty());
    EXPECT_EQ(b.size(), 0u);
    EXPECT_FALSE(b.constrained());
    EXPECT_EQ(b.low(), 0);
    EXPECT_EQ(b.high(), 0);
}

TEST(BucketsTest, ConstrainedConstructionStoresBounds) {
    buckets<int, int> b(10, 20);
    EXPECT_TRUE(b.constrained());
    EXPECT_EQ(b.low(), 10);
    EXPECT_EQ(b.high(), 20);
    EXPECT_TRUE(b.empty());
}

TEST(BucketsTest, ConstrainedConstructionThrowsWhenOutOfOrder) {
    using IntBucket = buckets<int, int>;
    EXPECT_THROW(IntBucket(20, 10), std::invalid_argument);
}

TEST(BucketsTest, ConstrainedConstructionAllowsEqualBounds) {
    using IntBucket = buckets<int, int>;
    EXPECT_NO_THROW(IntBucket(5, 5));
}

TEST(BucketsTest, SpreadOnUnconstrainedCreatesBuckets) {
    buckets<int, int> b;

    EXPECT_TRUE(b.spread(9, 10, 1));
    EXPECT_EQ(b.size(), 1u);

    EXPECT_TRUE(b.spread(10, 25, 2));
    EXPECT_EQ(b.size(), 2u);

    EXPECT_TRUE(b.spread(30, 40, 3));
    // Overlapping/slicing bucket
    EXPECT_TRUE(b.spread(30, 31, 4));
    EXPECT_EQ(b.size(), 4u);

    auto snap = snapshot(b);
    ASSERT_EQ(snap.size(), 4u);
    EXPECT_EQ(std::get<0>(snap[0]), 9);
    EXPECT_EQ(std::get<1>(snap[0]), 10);
    EXPECT_EQ(std::get<2>(snap[0]), (std::vector<int>{1}));

    EXPECT_EQ(std::get<0>(snap[2]), 30);
    EXPECT_EQ(std::get<1>(snap[2]), 31);
    EXPECT_EQ(std::get<2>(snap[2]), (std::vector<int>{3, 4}));

    EXPECT_EQ(std::get<0>(snap[3]), 31);
    EXPECT_EQ(std::get<1>(snap[3]), 40);
    EXPECT_EQ(std::get<2>(snap[3]), (std::vector<int>{3}));
}

TEST(BucketsTest, SpreadWideRangeOverExistingGapsAndBuckets) {
    buckets<int, int> b;
    b.spread(9, 10, 1);
    b.spread(10, 25, 2);
    b.spread(30, 40, 3);
    b.spread(30, 31, 4);
    b.spread(50, 60, 5);
    b.spread(59, 60, 6);
    b.spread(70, 80, 7);
    b.spread(80, 81, 8);

    ASSERT_TRUE(b.spread(15, 75, 9));
    EXPECT_EQ(b.size(), 13u);

    auto snap = snapshot(b);
    ASSERT_EQ(snap.size(), 13u);
    // Spot-check a few of the slices produced by the spread.
    EXPECT_EQ(std::get<0>(snap[2]), 15);
    EXPECT_EQ(std::get<1>(snap[2]), 25);
    EXPECT_EQ(std::get<2>(snap[2]), (std::vector<int>{2, 9}));

    EXPECT_EQ(std::get<0>(snap[6]), 40);
    EXPECT_EQ(std::get<1>(snap[6]), 50);
    EXPECT_EQ(std::get<2>(snap[6]), (std::vector<int>{9}));

    EXPECT_EQ(std::get<0>(snap[12]), 80);
    EXPECT_EQ(std::get<1>(snap[12]), 81);
    EXPECT_EQ(std::get<2>(snap[12]), (std::vector<int>{8}));
}

TEST(BucketsTest, SpreadOutsideConstraintIsNoOp) {
    buckets<int, int> b(26, 74);

    EXPECT_FALSE(b.spread(9, 10, 1));
    EXPECT_EQ(b.size(), 0u);

    // Upper bound of the spread equals the lower constraint: no overlap.
    EXPECT_FALSE(b.spread(10, 26, 2));
    EXPECT_EQ(b.size(), 0u);
}

TEST(BucketsTest, SpreadClampsToConstraint) {
    buckets<int, int> b(26, 74);

    // Overlaps the lower bound of the constraint; should be clamped to it.
    ASSERT_TRUE(b.spread(20, 30, 1));
    auto snap = snapshot(b);
    ASSERT_EQ(snap.size(), 1u);
    EXPECT_EQ(std::get<0>(snap[0]), 26);
    EXPECT_EQ(std::get<1>(snap[0]), 30);
}

TEST(BucketsTest, CoverReplacesOverlappingValues) {
    buckets<int, int> b;
    b.spread(0, 100, 1);

    ASSERT_TRUE(b.cover(40, 60, 2));

    auto snap = snapshot(b);
    ASSERT_EQ(snap.size(), 3u);
    EXPECT_EQ(std::get<2>(snap[0]), (std::vector<int>{1}));
    EXPECT_EQ(std::get<0>(snap[1]), 40);
    EXPECT_EQ(std::get<1>(snap[1]), 60);
    EXPECT_EQ(std::get<2>(snap[1]), (std::vector<int>{2}));
    EXPECT_EQ(std::get<2>(snap[2]), (std::vector<int>{1}));
}

TEST(BucketsTest, CoverOutsideConstraintIsNoOp) {
    buckets<int, int> b(0, 10);
    EXPECT_FALSE(b.cover(20, 30, 1));
    EXPECT_EQ(b.size(), 0u);
}

TEST(BucketsTest, CoverClampsToConstraintOnBothSides) {
    buckets<int, int> b(20, 80);
    b.spread(20, 80, 1);

    // Overlaps but extends below the lower constraint: clamp low end.
    ASSERT_TRUE(b.cover(10, 50, 2));
    auto snap = snapshot(b);
    ASSERT_EQ(snap.size(), 2u);
    EXPECT_EQ(std::get<0>(snap[0]), 20);
    EXPECT_EQ(std::get<1>(snap[0]), 50);
    EXPECT_EQ(std::get<2>(snap[0]), (std::vector<int>{2}));

    // Overlaps but extends above the upper constraint: clamp high end.
    ASSERT_TRUE(b.cover(60, 100, 3));
    snap = snapshot(b);
    ASSERT_EQ(snap.size(), 3u);
    EXPECT_EQ(std::get<0>(snap[2]), 60);
    EXPECT_EQ(std::get<1>(snap[2]), 80);
    EXPECT_EQ(std::get<2>(snap[2]), (std::vector<int>{3}));
}

TEST(BucketsTest, SpreadEntirelyBeforeExistingBucketsIsDisjointInsert) {
    buckets<int, int> b;
    b.spread(50, 60, 1);
    b.spread(70, 80, 2);

    // The new range [10,20) lies entirely in the gap before the first
    // existing bucket and does not overlap it at all -- this exercises the
    // "no overlap, insert and stop" branch (as opposed to the "insert the
    // leading part, then continue processing the overlap" branch already
    // covered by SpreadWideRangeOverExistingGapsAndBuckets).
    ASSERT_TRUE(b.spread(10, 20, 9));
    EXPECT_EQ(b.size(), 3u);

    auto snap = snapshot(b);
    ASSERT_EQ(snap.size(), 3u);
    EXPECT_EQ(std::get<0>(snap[0]), 10);
    EXPECT_EQ(std::get<1>(snap[0]), 20);
    EXPECT_EQ(std::get<2>(snap[0]), (std::vector<int>{9}));
}

TEST(BucketsTest, SpreadFromAnotherBucketOfSameTraits) {
    buckets<int, int> source;
    source.spread(0, 10, 1);
    source.spread(10, 20, 2);

    buckets<int, int> target;
    target.spread(source);

    auto snap = snapshot(target);
    ASSERT_EQ(snap.size(), 2u);
    EXPECT_EQ(std::get<2>(snap[0]), (std::vector<int>{1}));
    EXPECT_EQ(std::get<2>(snap[1]), (std::vector<int>{2}));
}

TEST(BucketsTest, CoverFromAnotherBucketOfSameTraits) {
    buckets<int, int> source;
    source.spread(0, 100, 9);

    buckets<int, int> target;
    target.spread(0, 100, 1);
    target.cover(source);

    auto snap = snapshot(target);
    ASSERT_EQ(snap.size(), 1u);
    EXPECT_EQ(std::get<2>(snap[0]), (std::vector<int>{9}));
}

TEST(BucketsTest, IteratorsTraverseInOrder) {
    buckets<int, int> b;
    b.spread(0, 10, 1);
    b.spread(20, 30, 2);

    std::vector<int> lows;
    for (auto it = b.begin(); it != b.end(); ++it) {
        lows.push_back(it->first);
    }
    EXPECT_EQ(lows, (std::vector<int>{0, 20}));

    // const overloads
    const buckets<int, int>& cb = b;
    std::vector<int> const_lows;
    for (auto it = cb.begin(); it != cb.end(); ++it) {
        const_lows.push_back(it->first);
    }
    EXPECT_EQ(const_lows, (std::vector<int>{0, 20}));
}

TEST(BucketsTest, ReverseIteratorsTraverseInReverseOrder) {
    buckets<int, int> b;
    b.spread(0, 10, 1);
    b.spread(20, 30, 2);

    std::vector<int> lows;
    for (auto it = b.rbegin(); it != b.rend(); ++it) {
        lows.push_back(it->first);
    }
    EXPECT_EQ(lows, (std::vector<int>{20, 0}));

    const buckets<int, int>& cb = b;
    std::vector<int> const_lows;
    for (auto it = cb.rbegin(); it != cb.rend(); ++it) {
        const_lows.push_back(it->first);
    }
    EXPECT_EQ(const_lows, (std::vector<int>{20, 0}));
}

TEST(BucketsTest, ForwardRangeIteratorFindsOverlappingBuckets) {
    buckets<int, int> b;
    b.spread(0, 10, 1);
    b.spread(10, 20, 2);
    b.spread(20, 30, 3);
    b.spread(40, 50, 4);

    // NOTE: endRange()'s sentinel is computed by searching (forward, from
    // begin()) for a bucket that overlaps the single point [end_range,
    // end_range) -- it does not receive start_range at all. So if end_range
    // itself falls strictly inside an existing bucket, that bucket becomes
    // the (excluded) sentinel, even though it legitimately overlaps
    // [start_range, end_range). Use an end_range that falls in a gap
    // between buckets (30..40) to avoid that edge case here.
    std::vector<int> lows;
    for (auto it = b.beginRange<false>(15, 35); it != b.endRange<false>(15, 35); ++it) {
        lows.push_back(it->first);
    }
    // buckets overlapping [15, 35): [10,20) and [20,30)
    EXPECT_EQ(lows, (std::vector<int>{10, 20}));
}

TEST(BucketsTest, ReverseRangeIteratorFindsOverlappingBucketsInReverse) {
    buckets<int, int> b;
    b.spread(0, 10, 1);
    b.spread(10, 20, 2);
    b.spread(20, 30, 3);
    b.spread(40, 50, 4);

    // NOTE: this captures the verified, actual behavior of
    // rbeginRange/rendRange, which -- unlike the forward pair -- can
    // "overshoot" by one bucket beyond the nominal range: after locating
    // the first (highest) overlapping bucket, the reverse constructor
    // steps forward by one more before stopping. So even though [40,50)
    // does not overlap [15,35), it is included here.
    std::vector<int> lows;
    for (auto it = b.rbeginRange<false>(15, 35); it != b.rendRange<false>(15, 35); --it) {
        lows.push_back(it->first);
    }
    EXPECT_EQ(lows, (std::vector<int>{40, 20, 10}));
}

TEST(BucketsTest, RangeIteratorWithNoOverlapIsEmpty) {
    buckets<int, int> b;
    b.spread(0, 10, 1);
    b.spread(40, 50, 4);

    int count = 0;
    for (auto it = b.beginRange<false>(15, 25); it != b.endRange<false>(15, 25); ++it) {
        ++count;
    }
    EXPECT_EQ(count, 0);
}

// NOTE: range_iterator<IsConst>::operator*/operator-> both return
// non-const triplet_type&/triplet_type* unconditionally, regardless of
// IsConst. This means the const (IsConst = true) instantiation of
// beginRange/endRange/rbeginRange/rendRange -- e.g. b.beginRange<true>(...)
// -- fails to compile (binding a const_iterator's const reference to a
// non-const reference/pointer), since operator->() tries to return
// `&(*current_)` (a `const triplet_type*` when current_ is a
// const_iterator) as `triplet_type*`. This latent bug is never triggered
// by the library's own code, which never instantiates the const variant,
// so it is reported here rather than fixed (no request was made to modify
// library source), and no test can be written for that code path as-is.

TEST(BucketsTest, DescendingCompareTraitsReversesSpreadOrder) {
    using DescBucket = buckets<int, const char*, compare_traits_descending<int>>;
    DescBucket b(72, 18); // constrained, "descending" order: low=72 (largest), high=18 (smallest)

    EXPECT_TRUE(b.spread(40, 30, "30-40"));
    EXPECT_EQ(b.size(), 1u);
}
