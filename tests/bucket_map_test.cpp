#include <gtest/gtest.h>
#include "bucket/bucket_map.h"
#include "bucket/bucket_range.h"
#include "bucket/bucket_compare_traits.h"
#include "bucket/bucket_value_traits.h"
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <limits>

namespace masutils {

// Test fixture for bucket_map tests
class BucketMapTest : public ::testing::Test {
protected:
    using TestBucketMap = bucket_map<int, int>;
    using TestBucket = TestBucketMap::bucket_type;
    using TestValueContainer = typename TestBucketMap::value_container;

    std::unique_ptr<TestBucketMap> map;
    TestBucketMap bucket_map_;

    BucketMapTest() : bucket_map_() {} // Initialize with default constructor

    void SetUp() override {
        map = std::make_unique<TestBucketMap>();
        [[maybe_unused]] auto spread1 = map->spread(0, 10, 1);
        [[maybe_unused]] auto spread2 = map->spread(20, 30, 2);
        [[maybe_unused]] auto spread3 = map->spread(40, 50, 3);
    }

    // Helper function to verify container contents
    void verifyContainerContents(const TestValueContainer& container, const std::vector<int>& expected_values) {
        ASSERT_EQ(container.size(), expected_values.size());
        auto it = container.begin();
        for (const auto& expected : expected_values) {
            ASSERT_EQ(*it, expected);
            ++it;
        }
    }
};

// Construction tests
TEST_F(BucketMapTest, DefaultConstruction) {
    TestBucketMap test_map;
    EXPECT_FALSE(test_map.is_constrained());
    EXPECT_TRUE(test_map.begin() == test_map.end());
}

TEST_F(BucketMapTest, ConstrainedConstruction) {
    TestBucketMap test_map(0, 100);
    EXPECT_TRUE(test_map.is_constrained());
    EXPECT_TRUE(test_map.begin() == test_map.end());
    EXPECT_EQ(test_map.lower_bound(), 0);
    EXPECT_EQ(test_map.upper_bound(), 100);
}

TEST_F(BucketMapTest, InvalidConstrainedConstruction) {
    EXPECT_THROW(TestBucketMap test_map(100, 0), std::invalid_argument);
}

// Accessor tests
TEST_F(BucketMapTest, AccessorFunctions) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread_result = test_map.spread(1, 3, 1);
    test_map.spread(1, 3, 2);
    test_map.spread(1, 3, 3);

    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 3);
    const auto& bucket = *it;
    const auto& values = TestBucketMap::accessor::values(bucket);
    verifyContainerContents(values, {1, 2, 3});
}

// Iterator tests
TEST_F(BucketMapTest, IteratorFunctionality) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread_result = test_map.spread(1, 3, 1);
    test_map.spread(1, 3, 2);
    test_map.spread(1, 3, 3);
    
    // Forward iteration
    auto it = test_map.begin();
    EXPECT_NE(it, test_map.end());
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 3);
    const auto& bucket = *it;
    const auto& values = TestBucketMap::accessor::values(bucket);
    verifyContainerContents(values, {1, 2, 3});
    
    // Reverse iteration
    auto rit = test_map.rbegin();
    EXPECT_NE(rit, test_map.rend());
    EXPECT_EQ(TestBucketMap::accessor::low(*rit), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*rit), 3);
    const auto& rbucket = *rit;
    const auto& rvalues = TestBucketMap::accessor::values(rbucket);
    verifyContainerContents(rvalues, {1, 2, 3});
}

// Range iterator tests
TEST_F(BucketMapTest, RangeIteratorFunctionality) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread1 = test_map.spread(1, 3, 1);
    test_map.spread(1, 3, 2);
    test_map.spread(1, 3, 3);

    auto range = test_map.range(1, 3);
    auto it = range.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 3);
    const auto& bucket = *it;
    const auto& values = TestBucketMap::accessor::values(bucket);
    verifyContainerContents(values, {1, 2, 3});
}

// Basic operations tests
TEST_F(BucketMapTest, SpreadOperation) {
    TestBucketMap test_map;
    auto result = test_map.spread(1, 3, 1);
    EXPECT_EQ(result, 1);
    EXPECT_FALSE(test_map.begin() == test_map.end());
    
    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 3);
    const auto& bucket = *it;
    const auto& values = TestBucketMap::accessor::values(bucket);
    verifyContainerContents(values, {1});
}

TEST_F(BucketMapTest, CoverOperation) {
    TestBucketMap test_map;
    auto result = test_map.cover(1, 3, 1);
    EXPECT_EQ(result, 1);
    EXPECT_FALSE(test_map.begin() == test_map.end());
    
    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 3);
    const auto& bucket = *it;
    const auto& values = TestBucketMap::accessor::values(bucket);
    verifyContainerContents(values, {1});
}

TEST_F(BucketMapTest, EraseOperation) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread_result = test_map.spread(1, 3, 1);
    EXPECT_TRUE(test_map.erase(1, 3));
    EXPECT_TRUE(test_map.begin() == test_map.end());
}

// Bound tests
TEST_F(BucketMapTest, UnconstrainedBoundOperations) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread1 = test_map.spread(1, 3, 1);
    [[maybe_unused]] auto spread2 = test_map.spread(5, 7, 2);
    [[maybe_unused]] auto spread3 = test_map.spread(9, 11, 3);

    // Should throw when called on unconstrained bucket_map
    EXPECT_THROW(test_map.lower_bound(), std::runtime_error);
    EXPECT_THROW(test_map.upper_bound(), std::runtime_error);
}

TEST_F(BucketMapTest, ConstrainedBoundOperations) {
    TestBucketMap test_map(0, 100);
    [[maybe_unused]] auto spread1 = test_map.spread(1, 3, 1);
    [[maybe_unused]] auto spread2 = test_map.spread(5, 7, 2);
    [[maybe_unused]] auto spread3 = test_map.spread(9, 11, 3);

    // Should return the constraints, not the bucket bounds
    EXPECT_EQ(test_map.lower_bound(), 0);
    EXPECT_EQ(test_map.upper_bound(), 100);
}

// Edge cases and error conditions
TEST_F(BucketMapTest, OverlappingRanges) {
    TestBucketMap test_map;
    int count = test_map.spread(1, 3, 1);
    EXPECT_EQ(count, 1);
    count = test_map.spread(3, 5, 2);
    EXPECT_EQ(count, 1);
    count = test_map.spread(1, 5, 3);
    EXPECT_EQ(count, 2);

    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 3);
    const auto& bucket1 = *it;
    const auto& values1 = TestBucketMap::accessor::values(bucket1);
    verifyContainerContents(values1, {1, 3});

    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 3);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 5);
    const auto& bucket2 = *it;
    const auto& values2 = TestBucketMap::accessor::values(bucket2);
    verifyContainerContents(values2, {2, 3});
}

TEST_F(BucketMapTest, ConstrainedRangeOperations) {
    TestBucketMap test_map(0, 100);
    [[maybe_unused]] auto spread1 = test_map.spread(1, 3, 1);
    [[maybe_unused]] auto spread2 = test_map.spread(5, 7, 2);
    [[maybe_unused]] auto spread3 = test_map.spread(9, 11, 3);

    // Test range operations within constraints
    auto range = test_map.range(1, 5);
    auto range_it = range.begin();
    EXPECT_NE(range_it, range.end());
    EXPECT_EQ(TestBucketMap::accessor::low(*range_it), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*range_it), 3);
    const auto& bucket = *range_it;
    const auto& values = TestBucketMap::accessor::values(bucket);
    verifyContainerContents(values, {1});
}

TEST_F(BucketMapTest, AutomaticOrdering) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread1 = test_map.spread(5, 7, 3);
    [[maybe_unused]] auto spread2 = test_map.spread(1, 3, 1);
    [[maybe_unused]] auto spread3 = test_map.spread(9, 11, 5);

    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 1);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 3);
    const auto& bucket1 = *it;
    const auto& values1 = TestBucketMap::accessor::values(bucket1);
    verifyContainerContents(values1, {1});

    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 5);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 7);
    const auto& bucket2 = *it;
    const auto& values2 = TestBucketMap::accessor::values(bucket2);
    verifyContainerContents(values2, {3});

    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(*it), 9);
    EXPECT_EQ(TestBucketMap::accessor::high(*it), 11);
    const auto& bucket3 = *it;
    const auto& values3 = TestBucketMap::accessor::values(bucket3);
    verifyContainerContents(values3, {5});
}

TEST_F(BucketMapTest, NodiscardAttributes) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread_result = test_map.spread(1, 3, 1);
    EXPECT_EQ(spread_result, 1);
}

TEST_F(BucketMapTest, Concepts) {
    EXPECT_TRUE(std::totally_ordered<TestBucketMap::index_type>);
    EXPECT_TRUE(std::equality_comparable<TestBucketMap::index_type>);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

} // namespace masutils 