#include <gtest/gtest.h>
#include <bucket/bucket_compare_traits.h>
#include <chrono>

using namespace masutils;

// Test numeric types
TEST(BucketCompareTraitsTest, NumericTypes) {
    // Test int
    EXPECT_TRUE(bucket_compare_traits<int>::less(1, 2));
    EXPECT_FALSE(bucket_compare_traits<int>::less(2, 1));
    EXPECT_TRUE(bucket_compare_traits<int>::equal(1, 1));
    EXPECT_FALSE(bucket_compare_traits<int>::equal(1, 2));
    EXPECT_TRUE(bucket_compare_traits<int>::greater(2, 1));
    EXPECT_FALSE(bucket_compare_traits<int>::greater(1, 2));
    EXPECT_EQ(bucket_compare_traits<int>::min(1, 2), 1);
    EXPECT_EQ(bucket_compare_traits<int>::max(1, 2), 2);

    // Test double
    EXPECT_TRUE(bucket_compare_traits<double>::less(1.0, 2.0));
    EXPECT_FALSE(bucket_compare_traits<double>::less(2.0, 1.0));
    EXPECT_TRUE(bucket_compare_traits<double>::equal(1.0, 1.0));
    EXPECT_FALSE(bucket_compare_traits<double>::equal(1.0, 2.0));
    EXPECT_TRUE(bucket_compare_traits<double>::greater(2.0, 1.0));
    EXPECT_FALSE(bucket_compare_traits<double>::greater(1.0, 2.0));
    EXPECT_DOUBLE_EQ(bucket_compare_traits<double>::min(1.0, 2.0), 1.0);
    EXPECT_DOUBLE_EQ(bucket_compare_traits<double>::max(1.0, 2.0), 2.0);
}

// Test time_point
TEST(BucketCompareTraitsTest, TimePoint) {
    using time_point = std::chrono::system_clock::time_point;
    auto now = std::chrono::system_clock::now();
    auto later = now + std::chrono::seconds(1);

    EXPECT_TRUE(bucket_compare_traits<time_point>::less(now, later));
    EXPECT_FALSE(bucket_compare_traits<time_point>::less(later, now));
    EXPECT_TRUE(bucket_compare_traits<time_point>::equal(now, now));
    EXPECT_FALSE(bucket_compare_traits<time_point>::equal(now, later));
    EXPECT_TRUE(bucket_compare_traits<time_point>::greater(later, now));
    EXPECT_FALSE(bucket_compare_traits<time_point>::greater(now, later));
    EXPECT_EQ(bucket_compare_traits<time_point>::min(now, later), now);
    EXPECT_EQ(bucket_compare_traits<time_point>::max(now, later), later);
}

// Test custom type
struct custom_index {
    int value;
    bool operator<(const custom_index& other) const { return value < other.value; }
    bool operator==(const custom_index& other) const { return value == other.value; }
    bool operator>(const custom_index& other) const { return value > other.value; }
};

template<>
struct bucket_compare_traits<custom_index> {
    static bool less(const custom_index& a, const custom_index& b) { return a.value < b.value; }
    static bool equal(const custom_index& a, const custom_index& b) { return a.value == b.value; }
    static bool greater(const custom_index& a, const custom_index& b) { return a.value > b.value; }
    static custom_index min(const custom_index& a, const custom_index& b) { return {std::min(a.value, b.value)}; }
    static custom_index max(const custom_index& a, const custom_index& b) { return {std::max(a.value, b.value)}; }
};

TEST(BucketCompareTraitsTest, CustomType) {
    custom_index a{1}, b{2};

    EXPECT_TRUE(bucket_compare_traits<custom_index>::less(a, b));
    EXPECT_FALSE(bucket_compare_traits<custom_index>::less(b, a));
    EXPECT_TRUE(bucket_compare_traits<custom_index>::equal(a, a));
    EXPECT_FALSE(bucket_compare_traits<custom_index>::equal(a, b));
    EXPECT_TRUE(bucket_compare_traits<custom_index>::greater(b, a));
    EXPECT_FALSE(bucket_compare_traits<custom_index>::greater(a, b));
    EXPECT_EQ(bucket_compare_traits<custom_index>::min(a, b).value, 1);
    EXPECT_EQ(bucket_compare_traits<custom_index>::max(a, b).value, 2);
} 