#include <gtest/gtest.h>

#include "../include/buckets.h"
#include "../include/buckets_supp.h"

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace masutils;

TEST(MostRecentBucketValueTraitsTest, AddReplacesFrontOrPushesBack) {
    std::vector<int> container;
    most_recent_bucket_value_traits<int>::add(container, 1);
    EXPECT_EQ(container, (std::vector<int>{1}));

    most_recent_bucket_value_traits<int>::add(container, 2);
    EXPECT_EQ(container, (std::vector<int>{2}));
}

TEST(MostRecentBucketValueTraitsTest, AppendUsesLastOfOther) {
    std::vector<int> container;
    std::vector<int> other{10, 20, 30};
    most_recent_bucket_value_traits<int>::append(container, other);
    EXPECT_EQ(container, (std::vector<int>{30}));

    most_recent_bucket_value_traits<int>::append(container, other);
    EXPECT_EQ(container, (std::vector<int>{30}));
}

TEST(BucketValueAddTraitsTest, AddAccumulatesIntoFront) {
    std::vector<int> container;
    bucket_value_add_traits<int>::add(container, 5);
    EXPECT_EQ(container, (std::vector<int>{5}));

    bucket_value_add_traits<int>::add(container, 3);
    EXPECT_EQ(container, (std::vector<int>{8}));
}

TEST(BucketValueAddTraitsTest, AppendAddsEachElementOfOther) {
    std::vector<int> container;
    std::vector<int> other{1, 2, 3};
    bucket_value_add_traits<int>::append(container, other);
    EXPECT_EQ(container, (std::vector<int>{6}));
}

TEST(UniqueBucketValueTraitsTest, AddInsertsIntoSet) {
    std::set<int> container;
    unique_bucket_value_traits<int>::add(container, 1);
    unique_bucket_value_traits<int>::add(container, 1);
    unique_bucket_value_traits<int>::add(container, 2);
    EXPECT_EQ(container, (std::set<int>{1, 2}));
}

TEST(UniqueBucketValueTraitsTest, AppendInsertsEachElementOfOther) {
    std::set<int> container;
    // Note: append()'s internal const_iterator loop type is tied to
    // value_container, so the "other" container passed in must be the same
    // container type (this matches how buckets<>::spread/cover actually use
    // it -- appending from another bucket built with the same traits).
    std::set<int> other{3, 4, 3};
    unique_bucket_value_traits<int>::append(container, other);
    EXPECT_EQ(container, (std::set<int>{3, 4}));
}

TEST(CaseInsensitiveLessTest, ComparesIgnoringCase) {
    caseInsensitiveLess<std::string> less;
    EXPECT_FALSE(less("apple", "APPLE"));
    EXPECT_FALSE(less("APPLE", "apple"));
    EXPECT_TRUE(less("apple", "banana"));
    EXPECT_FALSE(less("banana", "apple"));
}

TEST(CaseInsensitiveLessTest, HandlesDifferentLengths) {
    caseInsensitiveLess<std::string> less;
    // NOTE: this comparator only compares characters up to the shorter
    // string's length and never falls back to comparing lengths, so it
    // (perhaps unintuitively) treats "app" and "apple" as equivalent
    // ("not less than" in either direction) rather than "app" < "apple".
    // This documents the actual, verified behavior.
    EXPECT_FALSE(less("app", "apple"));
    EXPECT_FALSE(less("apple", "app"));
    EXPECT_FALSE(less("apple", "apple"));
}

TEST(BucketValueWrapperTest, GenericTypeStreamsDirectly) {
    // bucket_value_wrapper<T> stores a `const T&` reference rather than a
    // copy, so it must be given a named, still-alive object (as
    // bucket_wrapper's operator<< always does internally, wrapping
    // long-lived elements of a bucket's value container) -- binding it to
    // a temporary here would leave a dangling reference.
    int value = 42;
    bucket_value_wrapper<int> wrapper(value);
    std::ostringstream oss;
    oss << wrapper;
    EXPECT_EQ(oss.str(), "42");
    EXPECT_EQ(wrapper.get(), 42);
}

TEST(BucketValueWrapperTest, StringSpecializationQuotesOutput) {
    // See the note above: pass a named, still-alive std::string, not a
    // temporary, since the specialization also stores a reference.
    std::string value = "hello";
    bucket_value_wrapper<std::string> wrapper(value);
    std::ostringstream oss;
    oss << wrapper;
    EXPECT_EQ(oss.str(), "\"hello\"");
    EXPECT_EQ(wrapper.get(), "hello");
}

TEST(BucketValueWrapperTest, CharPointerSpecializationQuotesOutput) {
    bucket_value_wrapper<char*> wrapper("world");
    std::ostringstream oss;
    oss << wrapper;
    EXPECT_EQ(oss.str(), "\"world\"");
    EXPECT_EQ(wrapper.get(), "world");
}

TEST(BucketWrapperTest, FormatsEmptyBucket) {
    buckets<int, int> b;
    std::ostringstream oss;
    oss << bucket_wrapper<buckets<int, int>>(b);
    EXPECT_EQ(oss.str(), "");
}

TEST(BucketWrapperTest, FormatsSingleBucketSingleValue) {
    buckets<int, int> b;
    b.spread(1, 2, 42);

    std::ostringstream oss;
    oss << bucket_wrapper<buckets<int, int>>(b);
    EXPECT_EQ(oss.str(), "{ 1, 2, { 42 } }");
}

TEST(BucketWrapperTest, FormatsMultipleBucketsAndValues) {
    buckets<int, int> b;
    b.spread(1, 2, 42);
    b.spread(1, 2, 43);
    b.spread(5, 6, 99);

    std::ostringstream oss;
    oss << bucket_wrapper<buckets<int, int>>(b);
    EXPECT_EQ(oss.str(), "{ 1, 2, { 42, 43 } },\n{ 5, 6, { 99 } }");
}

TEST(BucketWrapperTest, FormatsConstCharPointerValuedBucketUnquoted) {
    // buckets<int, const char*>::value_type is `const char*`, which does not
    // match the bucket_value_wrapper<char*> specialization (that only
    // matches a mutable char*), so it falls back to the generic wrapper and
    // is streamed unquoted -- matching the library's existing demo output.
    buckets<int, const char*> b;
    b.spread(1, 2, "a");

    std::ostringstream oss;
    oss << bucket_wrapper<buckets<int, const char*>>(b);
    EXPECT_EQ(oss.str(), "{ 1, 2, { a } }");
}

TEST(BucketWrapperTest, FormatsStdStringValuedBucketWithQuotes) {
    // buckets<int, std::string>::value_type is std::string, which does
    // match the bucket_value_wrapper<std::basic_string<...>> specialization.
    buckets<int, std::string> b;
    b.spread(1, 2, std::string("a"));

    std::ostringstream oss;
    oss << bucket_wrapper<buckets<int, std::string>>(b);
    EXPECT_EQ(oss.str(), "{ 1, 2, { \"a\" } }");
}
