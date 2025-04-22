#include "gtest/gtest.h"
#include "../include/bucket/bucket_list.h"
#include <vector>
#include <string>

using namespace masutils;

// Test fixture for bucket_list tests
class BucketListTest : public ::testing::Test {
protected:
    using TestBucketList = bucket_list<int, std::string>;
    using TestBucket = TestBucketList::bucket_type;
    using TestValueContainer = typename TestBucketList::value_container;

    void SetUp() override {
        // Common setup code if needed
    }

    void TearDown() override {
        // Common cleanup code if needed
    }

    // Helper function to verify container contents
    void verifyContainerContents(const TestValueContainer& container, const std::vector<std::string>& expected_values) {
        ASSERT_EQ(container.size(), expected_values.size());
        auto it = container.begin();
        for (const auto& expected : expected_values) {
            ASSERT_EQ(*it, expected);
            ++it;
        }
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
    TestBucketList list;
    [[maybe_unused]] auto spread_result = list.spread(0, 10, "test");
    
    auto it = list.begin();
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 10);
    verifyContainerContents(TestBucketList::accessor::values(*it), {"test"});
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
    verifyContainerContents(TestBucketList::accessor::values(*it), {"test"});
    
    // Reverse iteration
    auto rit = list.rbegin();
    EXPECT_NE(rit, list.rend());
    EXPECT_EQ(TestBucketList::accessor::low(*rit), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*rit), 10);
    verifyContainerContents(TestBucketList::accessor::values(*rit), {"test"});
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
    verifyContainerContents(TestBucketList::accessor::values(*it), {"test"});
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
    verifyContainerContents(TestBucketList::accessor::values(*it), {"test"});
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
    verifyContainerContents(TestBucketList::accessor::values(*it), {"test1"});
    
    ++it;
    // Second bucket: [5, 10) with both test1 and test2
    EXPECT_EQ(TestBucketList::accessor::low(*it), 5);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 10);
    verifyContainerContents(TestBucketList::accessor::values(*it), {"test1", "test2"});
    
    ++it;
    // Third bucket: [10, 15) with test2
    EXPECT_EQ(TestBucketList::accessor::low(*it), 10);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 15);
    verifyContainerContents(TestBucketList::accessor::values(*it), {"test2"});
}

TEST_F(BucketListTest, RangeIteratorFunctionality) {
    TestBucketList list;
    [[maybe_unused]] auto spread1 = list.spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = list.spread(20, 30, "test2");
    [[maybe_unused]] auto spread3 = list.spread(40, 50, "test3");

    // Test forward range iteration over [15, 35]
    auto range_it = list.beginRange<false>(15, 35);
    auto range_end = list.endRange<false>(15, 35);
    EXPECT_NE(range_it, range_end);
    EXPECT_EQ(TestBucketList::accessor::low(*range_it), 20);
    EXPECT_EQ(TestBucketList::accessor::high(*range_it), 30);
    verifyContainerContents(TestBucketList::accessor::values(*range_it), {"test2"});

    // Test reverse range iteration over [15, 35]
    auto range_rit = list.rbeginRange<true>(15, 35);
    auto range_rend = list.rendRange<true>(15, 35);
    EXPECT_NE(range_rit, range_rend);
    EXPECT_EQ(TestBucketList::accessor::low(*range_rit), 20);
    EXPECT_EQ(TestBucketList::accessor::high(*range_rit), 30);
    verifyContainerContents(TestBucketList::accessor::values(*range_rit), {"test2"});
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

// Bound tests
TEST_F(BucketListTest, UnconstrainedBoundOperations) {
    TestBucketList list;
    [[maybe_unused]] auto spread1 = list.spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = list.spread(20, 30, "test2");
    [[maybe_unused]] auto spread3 = list.spread(40, 50, "test3");

    // Should throw when called on unconstrained bucket_list
    bool lower_threw = false;
    bool upper_threw = false;
    
    try {
        [[maybe_unused]] auto lower = list.lower_bound();
    } catch (const std::runtime_error&) {
        lower_threw = true;
    }
    EXPECT_TRUE(lower_threw);

    try {
        [[maybe_unused]] auto upper = list.upper_bound();
    } catch (const std::runtime_error&) {
        upper_threw = true;
    }
    EXPECT_TRUE(upper_threw);
}

TEST_F(BucketListTest, ConstrainedBoundOperations) {
    TestBucketList list(0, 100);
    [[maybe_unused]] auto spread1 = list.spread(10, 20, "test1");
    [[maybe_unused]] auto spread2 = list.spread(30, 40, "test2");
    [[maybe_unused]] auto spread3 = list.spread(50, 60, "test3");

    // Should return the constraints, not the bucket bounds
    [[maybe_unused]] auto lower = list.lower_bound();
    [[maybe_unused]] auto upper = list.upper_bound();
    EXPECT_EQ(lower, 0);
    EXPECT_EQ(upper, 100);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 