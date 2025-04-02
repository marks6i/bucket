#include "gtest/gtest.h"
#include "../include/bucket/bucket_map.h"
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <limits>

using namespace masutils;

// Test-specific derived class that exposes protected members
class TestBucketMapExposed : public bucket_map<int, std::string> {
public:
    using bucket_map<int, std::string>::splice;
    
    // Expose constructors
    TestBucketMapExposed() : bucket_map<int, std::string>() {}
    explicit TestBucketMapExposed(int low, int high) : bucket_map<int, std::string>(low, high) {}
};

// Test fixture for bucket_map tests
class BucketMapTest : public ::testing::Test {
protected:
    using TestBucketMap = TestBucketMapExposed;
    using TestBucket = TestBucketMap::bucket_type;
    using TestValueContainer = TestBucketMap::value_container;

    std::unique_ptr<TestBucketMap> map;
    TestBucketMap bucket_map_;

    BucketMapTest() : bucket_map_() {} // Initialize with default constructor

    void SetUp() override {
        map = std::make_unique<TestBucketMap>();
        [[maybe_unused]] auto spread1 = map->spread(0, 10, "test1");
        [[maybe_unused]] auto spread2 = map->spread(20, 30, "test2");
        [[maybe_unused]] auto spread3 = map->spread(40, 50, "test3");
    }

    void TearDown() override {
        // Clean up any resources if needed
    }
};

// Construction tests
TEST_F(BucketMapTest, DefaultConstruction) {
    TestBucketMap test_map;
    EXPECT_FALSE(test_map.constrained());
    EXPECT_TRUE(test_map.empty());
    EXPECT_EQ(test_map.size(), 0);
}

TEST_F(BucketMapTest, ConstrainedConstruction) {
    TestBucketMap test_map(0, 100);
    EXPECT_TRUE(test_map.constrained());
    EXPECT_TRUE(test_map.empty());
    EXPECT_EQ(test_map.size(), 0);
    EXPECT_EQ(test_map.low(), 0);
    EXPECT_EQ(test_map.high(), 100);
}

TEST_F(BucketMapTest, InvalidConstrainedConstruction) {
    EXPECT_THROW(TestBucketMap test_map(100, 0), std::invalid_argument);
}

// Accessor tests
TEST_F(BucketMapTest, AccessorFunctions) {
    TestValueContainer values = {"test"};
    TestBucketMap::bucket_type bucket = TestBucketMap::make_bucket(0, 10, values);
    
    EXPECT_EQ(TestBucketMap::accessor::low(bucket), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(bucket), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(bucket), values);
}

// Iterator tests
TEST_F(BucketMapTest, IteratorFunctionality) {
    TestBucketMap test_map;
    TestValueContainer values = {"test"};
    [[maybe_unused]] auto spread_result = test_map.spread(0, 10, "test");
    
    // Forward iteration
    auto it = test_map.begin();
    EXPECT_NE(it, test_map.end());
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), values);
    
    // Reverse iteration
    auto rit = test_map.rbegin();
    EXPECT_NE(rit, test_map.rend());
    EXPECT_EQ(TestBucketMap::accessor::low(rit->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(rit->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(rit->second), values);
}

// Range iterator tests
TEST_F(BucketMapTest, RangeIteratorFunctionality) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread1 = test_map.spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = test_map.spread(20, 30, "test2");
    [[maybe_unused]] auto spread3 = test_map.spread(40, 50, "test3");

    // Test forward range iteration over [15, 35]
    auto range_it = test_map.beginRange<false>(15, 35);
    auto range_end = test_map.endRange<false>(15, 35);
    EXPECT_NE(range_it, range_end);
    EXPECT_EQ(TestBucketMap::accessor::low(*range_it), 20);
    EXPECT_EQ(TestBucketMap::accessor::high(*range_it), 30);
    EXPECT_EQ(TestBucketMap::accessor::values(*range_it), TestValueContainer{"test2"});

    // Test reverse range iteration over [15, 35]
    // For reverse iteration, we need to use the template parameter true
    auto range_rit = test_map.rbeginRange<true>(15, 35);
    auto range_rend = test_map.rendRange<true>(15, 35);
    EXPECT_NE(range_rit, range_rend);
    
    // In reverse iteration, we should still get the same bucket [20, 30)
    // but we're iterating from the end of the range towards the beginning
    auto rit = range_rit;
    EXPECT_EQ(TestBucketMap::accessor::low(*rit), 20);
    EXPECT_EQ(TestBucketMap::accessor::high(*rit), 30);
    EXPECT_EQ(TestBucketMap::accessor::values(*rit), TestValueContainer{"test2"});
    
    // Moving to next bucket in reverse should reach the end
    ++rit;
    EXPECT_EQ(rit, range_rend);

    // Test empty range
    auto empty_it = test_map.beginRange<false>(15, 15);
    auto empty_end = test_map.endRange<false>(15, 15);
    EXPECT_EQ(empty_it, empty_end);

    // Test range with no buckets
    auto no_buckets_it = test_map.beginRange<false>(35, 40);
    auto no_buckets_end = test_map.endRange<false>(35, 40);
    EXPECT_EQ(no_buckets_it, no_buckets_end);

    // Also test reverse range with no buckets
    auto no_buckets_rit = test_map.rbeginRange<true>(35, 40);
    auto no_buckets_rend = test_map.rendRange<true>(35, 40);
    EXPECT_EQ(no_buckets_rit, no_buckets_rend);
}

// Basic operations tests
TEST_F(BucketMapTest, SpreadOperation) {
    TestBucketMap test_map;
    auto result = test_map.spread(0, 10, "test");
    EXPECT_EQ(result, 1);
    EXPECT_EQ(test_map.size(), 1);
    EXPECT_FALSE(test_map.empty());
    
    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), TestValueContainer{"test"});
}

TEST_F(BucketMapTest, CoverOperation) {
    TestBucketMap test_map;
    auto result = test_map.cover(0, 10, "test");
    EXPECT_EQ(result, 1);
    EXPECT_EQ(test_map.size(), 1);
    EXPECT_FALSE(test_map.empty());
    
    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), TestValueContainer{"test"});
}

TEST_F(BucketMapTest, EraseOperation) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread_result = test_map.spread(0, 10, "test");
    EXPECT_TRUE(test_map.erase(0, 10));
    EXPECT_TRUE(test_map.empty());
    EXPECT_EQ(test_map.size(), 0);
}

// Splice operation tests
TEST_F(BucketMapTest, SpliceOperation) {
    TestBucketMap test_map1;
    TestBucketMap test_map2;
    [[maybe_unused]] auto spread1 = test_map1.spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = test_map2.spread(5, 15, "test2");

    // Get iterators for splicing
    auto it2 = test_map2.begin();
    auto end2 = test_map2.end();

    // Test the internal splice operation
    EXPECT_TRUE(test_map1.splice(5, 15, it2, end2));
    EXPECT_EQ(test_map1.size(), 2);

    auto it = test_map1.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 5);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), TestValueContainer{"test1"});

    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 15);
    TestValueContainer empty_container;
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), empty_container);
}

// Bound tests
TEST_F(BucketMapTest, UnconstrainedBoundOperations) {
    TestBucketMap test_map;
    [[maybe_unused]] auto spread1 = test_map.spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = test_map.spread(20, 30, "test2");
    [[maybe_unused]] auto spread3 = test_map.spread(40, 50, "test3");

    // Should throw when called on unconstrained bucket_map
    bool lower_threw = false;
    bool upper_threw = false;
    
    try {
        [[maybe_unused]] auto lower = test_map.lower_bound();
    } catch (const std::runtime_error&) {
        lower_threw = true;
    }
    EXPECT_TRUE(lower_threw);

    try {
        [[maybe_unused]] auto upper = test_map.upper_bound();
    } catch (const std::runtime_error&) {
        upper_threw = true;
    }
    EXPECT_TRUE(upper_threw);
}

TEST_F(BucketMapTest, ConstrainedBoundOperations) {
    TestBucketMap test_map(0, 100);
    [[maybe_unused]] auto spread1 = test_map.spread(10, 20, "test1");
    [[maybe_unused]] auto spread2 = test_map.spread(30, 40, "test2");
    [[maybe_unused]] auto spread3 = test_map.spread(50, 60, "test3");

    // Should return the constraints, not the bucket bounds
    [[maybe_unused]] auto lower = test_map.lower_bound();
    [[maybe_unused]] auto upper = test_map.upper_bound();
    EXPECT_EQ(lower, 0);
    EXPECT_EQ(upper, 100);
}

// Edge cases and error conditions
TEST_F(BucketMapTest, OverlappingRanges) {
    TestBucketMap test_map;
    int count = test_map.spread(0, 5, "test1");
    EXPECT_EQ(count, 1);
    count = test_map.spread(5, 10, "test2");
    EXPECT_EQ(count, 1);
    count = test_map.spread(0, 10, "test3");
    EXPECT_EQ(count, 2);

    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 5);
    TestValueContainer expected1{"test1", "test3"};
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), expected1);

    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 5);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    TestValueContainer expected2{"test2", "test3"};
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), expected2);
}

TEST_F(BucketMapTest, ConstrainedRangeOperations) {
    TestBucketMap test_map(0, 100);
    
    // Test out-of-bounds operations
    auto result1 = test_map.spread(-1, 10, "test");
    EXPECT_EQ(result1, 1); // Should succeed but be constrained to [0, 10)
    
    auto result2 = test_map.spread(90, 110, "test");
    EXPECT_EQ(result2, 1); // Should succeed but be constrained to [90, 100)
    
    // Verify the constrained ranges
    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    
    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 90);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 100);
}

// Test automatic ordering
TEST_F(BucketMapTest, AutomaticOrdering) {
    TestBucketMap test_map;
    
    // Insert buckets in reverse order
    [[maybe_unused]] auto spread3 = test_map.spread(20, 30, "test3");
    [[maybe_unused]] auto spread2 = test_map.spread(10, 20, "test2");
    [[maybe_unused]] auto spread1 = test_map.spread(0, 10, "test1");
    
    // Verify they are stored in order
    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), TestValueContainer{"test1"});
    
    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 20);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), TestValueContainer{"test2"});
    
    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 20);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 30);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), TestValueContainer{"test3"});
}

// Test [[nodiscard]] attributes
TEST_F(BucketMapTest, NodiscardAttributes) {
    TestBucketMap test_map;
    
    // These should compile but generate warnings if the result is ignored
    [[maybe_unused]] auto spread_result = test_map.spread(0, 10, "test");
    [[maybe_unused]] auto cover_result = test_map.cover(10, 20, "test");
    [[maybe_unused]] auto erase_result = test_map.erase(0, 10);
    
    // Store iterators in variables to pass by reference
    auto begin_it = test_map.begin();
    auto end_it = test_map.end();
    [[maybe_unused]] auto splice_result = test_map.splice(0, 10, begin_it, end_it);
}

#if __cplusplus >= 202002L
// Test C++20 concepts
TEST_F(BucketMapTest, Concepts) {
    static_assert(std::totally_ordered<int>);
    static_assert(std::equality_comparable<int>);
}
#endif // __cplusplus >= 202002L

// ============================================================================
// INTERNAL IMPLEMENTATION TESTS
// These tests verify internal implementation details of bucket_map.
// They may be removed or modified if the internal implementation changes.
// ============================================================================

/**
 * @brief Test class for internal bucket_map implementation details.
 * This class is used to test protected/internal methods that are not part of the public API.
 * These tests may be removed or modified if the internal implementation changes.
 */
class BucketMapSpliceTest : public BucketMapTest {
protected:
    // No need for wrapper method since we're using TestBucketMapExposed
};

TEST_F(BucketMapSpliceTest, InternalSpliceOperation) {
    // Create a map for testing splice
    TestBucketMap test_map;
    [[maybe_unused]] auto spread1 = test_map.spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = test_map.spread(5, 15, "test2");

    // Get iterators for splicing from the same map
    TestBucketMap::iterator begin = test_map.begin();
    TestBucketMap::iterator end = test_map.end();

    // Test the internal splice operation
    EXPECT_TRUE(test_map.splice(5, 15, begin, end));
    EXPECT_EQ(test_map.size(), 2);

    // Verify the results
    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 5);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    TestValueContainer expected_values1;
    expected_values1.push_back("test1");
    expected_values1.push_back("test2");
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), expected_values1);

    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 15);
    TestValueContainer expected_values2;
    expected_values2.push_back("test2");
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), expected_values2);
}

TEST_F(BucketMapSpliceTest, InternalSpliceConstrainedBounds) {
    // Create a constrained map
    TestBucketMap test_map(0, 100);
    
    // Test splicing with out-of-bounds values
    TestBucketMap::iterator begin = test_map.begin();
    TestBucketMap::iterator end = test_map.end();
    
    // Should succeed but be constrained to [0, 100]
    EXPECT_TRUE(test_map.splice(-10, 110, begin, end));
    
    // Verify the results are constrained
    auto it = test_map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 100);
}

TEST_F(BucketMapSpliceTest, InternalSpliceEmptyRange) {
    TestBucketMap test_map;
    TestBucketMap::iterator begin = test_map.begin();
    TestBucketMap::iterator end = test_map.end();
    
    // Should fail for empty range
    EXPECT_FALSE(test_map.splice(10, 10, begin, end));
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 