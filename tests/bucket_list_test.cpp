#include "gtest/gtest.h"
#include "../include/bucket/bucket_list.h"
#include <vector>
#include <string>

using namespace masutils;

// Test-specific derived class that exposes protected members
class TestBucketListExposed : public bucket_list<int, std::string> {
public:
    using bucket_list<int, std::string>::splice;
    
    // Expose constructors
    TestBucketListExposed() : bucket_list<int, std::string>() {}
    explicit TestBucketListExposed(int low, int high) : bucket_list<int, std::string>(low, high) {}
};

// Test fixture for bucket_list tests
class BucketListTest : public ::testing::Test {
protected:
    using TestBucketList = TestBucketListExposed;
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

// ============================================================================
// INTERNAL IMPLEMENTATION TESTS
// These tests verify internal implementation details of bucket_list.
// They may be removed or modified if the internal implementation changes.
// ============================================================================

/**
 * @brief Test class for internal bucket_list implementation details.
 * This class is used to test protected/internal methods that are not part of the public API.
 * These tests may be removed or modified if the internal implementation changes.
 */
class BucketListSpliceTest : public BucketListTest {
protected:
    // No need for wrapper method since we're using TestBucketListExposed
};

TEST_F(BucketListSpliceTest, InternalSpliceOperation) {
    // Create two lists for testing splice
    TestBucketList list1;
    TestBucketList list2;
    [[maybe_unused]] auto spread1 = list1.spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = list2.spread(5, 15, "test2");

    // Get iterators for splicing
    TestBucketList::iterator it1 = list1.begin();
    TestBucketList::iterator it2 = list2.begin();
    TestBucketList::iterator end2 = list2.end();

    // Test the internal splice operation
    EXPECT_TRUE(list1.splice(5, 15, it2, end2));
    EXPECT_EQ(list1.size(), 2);

    // Verify the results
    auto it = list1.begin();
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 5);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test1"});

    ++it;
    EXPECT_EQ(TestBucketList::accessor::low(*it), 5);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 15);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test2"});
}

TEST_F(BucketListSpliceTest, InternalSpliceConstrainedBounds) {
    // Create a constrained list
    TestBucketList list(0, 100);
    
    // Test splicing with out-of-bounds values
    TestBucketList::iterator begin = list.begin();
    TestBucketList::iterator end = list.end();
    
    // Should succeed but be constrained to [0, 100]
    EXPECT_TRUE(list.splice(-10, 110, begin, end));
    
    // Verify the results are constrained
    auto it = list.begin();
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 100);
}

TEST_F(BucketListSpliceTest, InternalSpliceEmptyRange) {
    TestBucketList list;
    TestBucketList::iterator begin = list.begin();
    TestBucketList::iterator end = list.end();
    
    // Should fail for empty range
    EXPECT_FALSE(list.splice(10, 10, begin, end));
}

TEST_F(BucketListSpliceTest, InternalSpliceOverlappingRanges) {
    TestBucketList list;
    [[maybe_unused]] auto spread1 = list.spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = list.spread(5, 15, "test2");

    TestBucketList::iterator begin = list.begin();
    TestBucketList::iterator end = list.end();

    // Test splicing with overlapping ranges
    EXPECT_TRUE(list.splice(3, 7, begin, end));
    
    // Verify the results show proper splitting
    auto it = list.begin();
    EXPECT_EQ(TestBucketList::accessor::low(*it), 0);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 3);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test1"});

    ++it;
    EXPECT_EQ(TestBucketList::accessor::low(*it), 3);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 7);
    TestValueContainer expected_values = {"test1", "test2"};
    EXPECT_EQ(TestBucketList::accessor::values(*it), expected_values);

    ++it;
    EXPECT_EQ(TestBucketList::accessor::low(*it), 7);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 10);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test1"});

    ++it;
    EXPECT_EQ(TestBucketList::accessor::low(*it), 10);
    EXPECT_EQ(TestBucketList::accessor::high(*it), 15);
    EXPECT_EQ(TestBucketList::accessor::values(*it), TestValueContainer{"test2"});
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 