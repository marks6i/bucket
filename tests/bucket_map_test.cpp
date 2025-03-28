#include "../external/googletest/googletest/include/gtest/gtest.h"
#include "../include/bucket/bucket_map.h"
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <limits>

using namespace masutils;

using TestValueContainer = std::list<std::string>;
using TestBucketMap = bucket_map<int, std::string>;

class BucketMapTest : public ::testing::Test {
protected:
    TestBucketMap bucket_map_;

    BucketMapTest() : bucket_map_() {} // Initialize with default constructor

    void SetUp() override {
        // Clear all buckets between tests
        bucket_map_.erase(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
    }

    void TearDown() override {
        // Clean up any resources if needed
    }
};

// Construction tests
TEST_F(BucketMapTest, DefaultConstruction) {
    TestBucketMap map;
    EXPECT_FALSE(map.constrained());
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

TEST_F(BucketMapTest, ConstrainedConstruction) {
    TestBucketMap map(0, 100);
    EXPECT_TRUE(map.constrained());
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.low(), 0);
    EXPECT_EQ(map.high(), 100);
}

TEST_F(BucketMapTest, InvalidConstrainedConstruction) {
    EXPECT_THROW(TestBucketMap map(100, 0), std::invalid_argument);
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
    TestBucketMap map;
    TestValueContainer values = {"test"};
    [[maybe_unused]] auto spread_result = map.spread(0, 10, "test");
    
    // Forward iteration
    auto it = map.begin();
    EXPECT_NE(it, map.end());
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), values);
    
    // Reverse iteration
    auto rit = map.rbegin();
    EXPECT_NE(rit, map.rend());
    EXPECT_EQ(TestBucketMap::accessor::low(rit->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(rit->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(rit->second), values);
}

// Basic operations tests
TEST_F(BucketMapTest, SpreadOperation) {
    TestBucketMap map;
    auto result = map.spread(0, 10, "test");
    EXPECT_EQ(result, 1);
    EXPECT_EQ(map.size(), 1);
    EXPECT_FALSE(map.empty());
    
    auto it = map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), TestValueContainer{"test"});
}

TEST_F(BucketMapTest, CoverOperation) {
    TestBucketMap map;
    auto result = map.cover(0, 10, "test");
    EXPECT_EQ(result, 1);
    EXPECT_EQ(map.size(), 1);
    EXPECT_FALSE(map.empty());
    
    auto it = map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    EXPECT_EQ(TestBucketMap::accessor::values(it->second), TestValueContainer{"test"});
}

TEST_F(BucketMapTest, EraseOperation) {
    TestBucketMap map;
    [[maybe_unused]] auto spread_result = map.spread(0, 10, "test");
    EXPECT_TRUE(map.erase(0, 10));
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
}

// Edge cases and error conditions
TEST_F(BucketMapTest, OverlappingRanges) {
    TestBucketMap map;
    int count = map.spread(0, 5, "test1");
    EXPECT_EQ(count, 1);
    count = map.spread(5, 10, "test2");
    EXPECT_EQ(count, 1);
    count = map.spread(0, 10, "test3");
    EXPECT_EQ(count, 2);

    auto it = map.begin();
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
    TestBucketMap map(0, 100);
    
    // Test out-of-bounds operations
    auto result1 = map.spread(-1, 10, "test");
    EXPECT_EQ(result1, 1); // Should succeed but be constrained to [0, 10)
    
    auto result2 = map.spread(90, 110, "test");
    EXPECT_EQ(result2, 1); // Should succeed but be constrained to [90, 100)
    
    // Verify the constrained ranges
    auto it = map.begin();
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 0);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 10);
    
    ++it;
    EXPECT_EQ(TestBucketMap::accessor::low(it->second), 90);
    EXPECT_EQ(TestBucketMap::accessor::high(it->second), 100);
}

// Test automatic ordering
TEST_F(BucketMapTest, AutomaticOrdering) {
    TestBucketMap map;
    
    // Insert buckets in reverse order
    [[maybe_unused]] auto spread3 = map.spread(20, 30, "test3");
    [[maybe_unused]] auto spread2 = map.spread(10, 20, "test2");
    [[maybe_unused]] auto spread1 = map.spread(0, 10, "test1");
    
    // Verify they are stored in order
    auto it = map.begin();
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

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 