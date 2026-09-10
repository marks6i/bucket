#include <gtest/gtest.h>
#include <bucket/bucket_list.h>
#include <bucket/bucket_map.h>
#include <vector>
#include <string>

using namespace masutils;

// Test fixture for bucket_list tests
class BucketListTest : public ::testing::Test {
protected:
    using TestBucketList = bucket_list<int, std::string>;
    using TestBucket = TestBucketList::bucket_type;
    using TestValueContainer = TestBucketList::value_container;

    void SetUp() override {
        // Common setup code if needed
    }

    void TearDown() override {
        // Common cleanup code if needed
    }
};

// Construction tests
TEST_F(BucketListTest, DefaultConstruction) {
    TestBucketList list;
    EXPECT_FALSE(list.constrained());
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

TEST_F(BucketListTest, ConstrainedConstruction) {
    TestBucketList list(0, 100);
    EXPECT_TRUE(list.constrained());
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
    EXPECT_EQ(list.low(), 0);
    EXPECT_EQ(list.high(), 100);
}

TEST_F(BucketListTest, InvalidConstrainedConstruction) {
    EXPECT_THROW(TestBucketList list(100, 0), std::invalid_argument);
}

// Accessor tests
TEST_F(BucketListTest, AccessorFunctions) {
    TestValueContainer values = {"test"};
    TestBucket bucket = TestBucketList::make_bucket(0, 10, values);
    
    EXPECT_EQ(TestBucketList::accessor::low(bucket), 0);
    EXPECT_EQ(TestBucketList::accessor::high(bucket), 10);
    EXPECT_EQ(TestBucketList::accessor::values(bucket), values);
}

// Iterator tests
TEST_F(BucketListTest, IteratorFunctionality) {
    TestBucketList list;
    TestValueContainer values = {"test"};
    [[maybe_unused]] auto spread_result = list.spread(0, 10, "test");
    
    // Forward iteration
    auto it = list.begin();
    EXPECT_NE(it, list.end());
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 10);
    EXPECT_EQ(TestBucketList::accessor::values(*it), values);
    
    // Reverse iteration
    auto rit = list.rbegin();
    EXPECT_NE(rit, list.rend());
    EXPECT_EQ(TestBucketList::accessor::low(*rit), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*rit), 10);
    EXPECT_EQ(TestBucketList::accessor::values(*rit), values);
}

// Basic operations tests
TEST_F(BucketListTest, SpreadOperation) {
    TestBucketList list;
    auto result = list.spread(0, 10, "test");
    EXPECT_EQ(result, 1);
    EXPECT_EQ(list.size(), 1);
    EXPECT_FALSE(list.empty());
    
    auto it = list.begin();
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 10);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test"});
}

TEST_F(BucketListTest, CoverOperation) {
    TestBucketList list;
    auto result = list.cover(0, 10, "test");
    EXPECT_EQ(result, 1);
    EXPECT_EQ(list.size(), 1);
    EXPECT_FALSE(list.empty());
    
    auto it = list.begin();
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 10);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test"});
}

TEST_F(BucketListTest, EraseOperation) {
    TestBucketList list;
    [[maybe_unused]] auto spread_result = list.spread(0, 10, "test");
    EXPECT_TRUE(list.erase(0, 10));
    EXPECT_TRUE(list.empty());
    EXPECT_EQ(list.size(), 0);
}

// Edge cases and error conditions
TEST_F(BucketListTest, OverlappingRanges) {
    TestBucketList list;
    [[maybe_unused]] auto first_spread = list.spread(0, 10, "test1");
    auto second_spread = list.spread(5, 15, "test2");
    EXPECT_EQ(second_spread, 2);
    EXPECT_EQ(list.size(), 3);
    
    auto it = list.begin();
    // First bucket: [0, 5) with test1
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 5);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test1"});
    
    ++it;
    // Second bucket: [5, 10) with both test1 and test2
    EXPECT_EQ(TestBucketList::accessor::low(*it), 5);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 10);
    TestValueContainer expected_values = {"test1", "test2"};
    EXPECT_EQ(TestBucketList::accessor::values(*it), expected_values);
    
    ++it;
    // Third bucket: [10, 15) with test2
    EXPECT_EQ(TestBucketList::accessor::low(*it), 10);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 15);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test2"});
}

TEST_F(BucketListTest, ConstrainedRangeOperations) {
    TestBucketList list(0, 100);
    
    // Test out-of-bounds operations
    auto result1 = list.spread(-1, 10, "test");
    EXPECT_EQ(result1, 1); // Should succeed but be constrained to [0, 10)
    
    auto result2 = list.spread(90, 110, "test");
    EXPECT_EQ(result2, 1); // Should succeed but be constrained to [90, 100)
    
    // Verify the constrained ranges
    auto it = list.begin();
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 10);
    
    ++it;
    EXPECT_EQ(TestBucketList::accessor::low(*it), 90);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 100);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 