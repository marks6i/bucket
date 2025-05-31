#include <gtest/gtest.h>
#include <bucket/detail/bucket_compare_traits.h>
#include <chrono>
#include <concepts>

using namespace masutils;

TEST(BucketCompareTraitsTest, IntComparisons) {
    EXPECT_TRUE(bucket_compare_traits<int>::lt(1, 2));
    EXPECT_FALSE(bucket_compare_traits<int>::lt(2, 1));
    EXPECT_TRUE(bucket_compare_traits<int>::eq(1, 1));
    EXPECT_FALSE(bucket_compare_traits<int>::eq(1, 2));
}

TEST(BucketCompareTraitsTest, DoubleComparisons) {
    EXPECT_TRUE(bucket_compare_traits<double>::lt(1.0, 2.0));
    EXPECT_FALSE(bucket_compare_traits<double>::lt(2.0, 1.0));
    EXPECT_TRUE(bucket_compare_traits<double>::eq(1.0, 1.0));
    EXPECT_FALSE(bucket_compare_traits<double>::eq(1.0, 2.0));
}

TEST(BucketCompareTraitsTest, TimePointComparisons) {
    using time_point = std::chrono::system_clock::time_point;
    auto now = std::chrono::system_clock::now();
    auto later = now + std::chrono::seconds(1);

    EXPECT_TRUE(bucket_compare_traits<time_point>::lt(now, later));
    EXPECT_FALSE(bucket_compare_traits<time_point>::lt(later, now));
    EXPECT_TRUE(bucket_compare_traits<time_point>::eq(now, now));
    EXPECT_FALSE(bucket_compare_traits<time_point>::eq(now, later));
}

// Test custom type
struct custom_index {
    int value;
    bool operator<(const custom_index& other) const { return value < other.value; }
    bool operator==(const custom_index& other) const { return value == other.value; }
};

TEST(BucketCompareTraitsTest, CustomType) {
    custom_index a{1}, b{2};

    EXPECT_TRUE(bucket_compare_traits<custom_index>::lt(a, b));
    EXPECT_FALSE(bucket_compare_traits<custom_index>::lt(b, a));
    EXPECT_TRUE(bucket_compare_traits<custom_index>::eq(a, a));
    EXPECT_FALSE(bucket_compare_traits<custom_index>::eq(a, b));
}

TEST(BucketCompareTraitsTest, DescendingOrder) {
    EXPECT_FALSE(bucket_compare_traits_descending<int>::lt(1, 2));
    EXPECT_TRUE(bucket_compare_traits_descending<int>::lt(2, 1));
    EXPECT_TRUE(bucket_compare_traits_descending<int>::eq(1, 1));
    EXPECT_FALSE(bucket_compare_traits_descending<int>::eq(1, 2));

    EXPECT_FALSE(bucket_compare_traits_descending<double>::lt(1.0, 2.0));
    EXPECT_TRUE(bucket_compare_traits_descending<double>::lt(2.0, 1.0));
    EXPECT_TRUE(bucket_compare_traits_descending<double>::eq(1.0, 1.0));
    EXPECT_FALSE(bucket_compare_traits_descending<double>::eq(1.0, 2.0));

    custom_index a{1}, b{2};
    EXPECT_FALSE(bucket_compare_traits_descending<custom_index>::lt(a, b));
    EXPECT_TRUE(bucket_compare_traits_descending<custom_index>::lt(b, a));
    EXPECT_TRUE(bucket_compare_traits_descending<custom_index>::eq(a, a));
    EXPECT_FALSE(bucket_compare_traits_descending<custom_index>::eq(a, b));
}

#if __cplusplus >= 202002L
TEST(BucketCompareTraitsTest, Concepts) {
    // Test that the concepts are satisfied for built-in types
    static_assert(LessThanComparable<int>);
    static_assert(EqualityComparable<int>);
    static_assert(LessThanComparable<double>);
    static_assert(EqualityComparable<double>);
    static_assert(LessThanComparable<custom_index>);
    static_assert(EqualityComparable<custom_index>);
}
#endif // __cplusplus >= 202002L 