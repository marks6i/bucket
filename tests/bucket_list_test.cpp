#include <gtest/gtest.h>
#include "bucket/bucket_list.h"
#include "bucket/bucket_range.h"
#include "bucket/bucket_compare_traits.h"
#include "bucket/bucket_value_traits.h"
#include "bucket/bucket_object.h"
#include <list>
#include <string>
#include <vector>
#include <iostream>
#include <limits>
#include <concepts>

namespace masutils
{

    // Test fixture for bucket_list tests
    class BucketListTest : public ::testing::Test
    {
    protected:
        using TestBucketList = bucket_list<int, std::string>;
        using TestBucket = TestBucketList::bucket_type;
        using TestValueContainer = TestBucketList::value_container;

        std::unique_ptr<TestBucketList> list;
        TestBucketList bucket_list_;

        BucketListTest() : bucket_list_() {} // Initialize with default constructor

        void SetUp() override
        {
            list = std::make_unique<TestBucketList>();
            [[maybe_unused]] auto spread1 = list->spread(0, 10, "test1");
            [[maybe_unused]] auto spread2 = list->spread(20, 30, "test2");
            [[maybe_unused]] auto spread3 = list->spread(40, 50, "test3");
        }

        void TearDown() override
        {
            // Common cleanup code if needed
        }

        // Helper function to verify container contents
        void verifyContainerContents(const TestValueContainer &container, const std::vector<std::string> &expected_values)
        {
            ASSERT_EQ(container.size(), expected_values.size());
            auto it = container.begin();
            for (const auto &expected : expected_values)
            {
                ASSERT_EQ(*it, expected);
                ++it;
            }
        }
    };

    // Construction tests
    TEST_F(BucketListTest, DefaultConstruction)
    {
        TestBucketList test_list;
        EXPECT_FALSE(test_list.constrained());
        EXPECT_TRUE(test_list.empty());
        EXPECT_EQ(test_list.size(), 0);
    }

    TEST_F(BucketListTest, ConstrainedConstruction)
    {
        TestBucketList test_list(0, 100);
        EXPECT_TRUE(test_list.constrained());
        EXPECT_TRUE(test_list.empty());
        EXPECT_EQ(test_list.size(), 0);
        EXPECT_EQ(test_list.low(), 0);
        EXPECT_EQ(test_list.high(), 100);
    }

    TEST_F(BucketListTest, InvalidConstrainedConstruction)
    {
        EXPECT_THROW(TestBucketList test_list(100, 0), std::invalid_argument);
    }

    // Accessor tests
    TEST_F(BucketListTest, AccessorFunctions)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto spread_result = test_list.spread(0, 10, "test");

        auto it = test_list.begin();
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 10);
        verifyContainerContents(it->values(), {"test"});
    }

    // Iterator tests
    TEST_F(BucketListTest, IteratorFunctionality)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto spread_result = test_list.spread(1, 3, "test1");
        [[maybe_unused]] auto spread_result2 = test_list.spread(1, 3, "test2");
        [[maybe_unused]] auto spread_result3 = test_list.spread(1, 3, "test3");

        // Forward iteration
        auto it = test_list.begin();
        EXPECT_NE(it, test_list.end());
        EXPECT_EQ(it->low(), 1);
        EXPECT_EQ(it->high(), 3);
        const auto &bucket = *it;
        const auto &values = bucket.values();
        verifyContainerContents(values, {"test1", "test2", "test3"});

        // Reverse iteration
        auto rit = test_list.rbegin();
        EXPECT_NE(rit, test_list.rend());
        EXPECT_EQ(rit->low(), 1);
        EXPECT_EQ(rit->high(), 3);
        const auto &rbucket = *rit;
        const auto &rvalues = rbucket.values();
        verifyContainerContents(rvalues, {"test1", "test2", "test3"});
    }

    // Basic operations tests
    TEST_F(BucketListTest, SpreadOperation)
    {
        TestBucketList test_list;
        auto result = test_list.spread(0, 10, "test");
        EXPECT_EQ(result, 1);
        EXPECT_EQ(test_list.size(), 1);
        EXPECT_FALSE(test_list.empty());

        auto it = test_list.begin();
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 10);
        verifyContainerContents(it->values(), {"test"});
    }

    TEST_F(BucketListTest, CoverOperation)
    {
        TestBucketList test_list;
        auto result = test_list.cover(0, 10, "test");
        EXPECT_EQ(result, 1);
        EXPECT_EQ(test_list.size(), 1);
        EXPECT_FALSE(test_list.empty());

        auto it = test_list.begin();
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 10);
        verifyContainerContents(it->values(), {"test"});
    }

    TEST_F(BucketListTest, EraseOperation)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto spread_result = test_list.spread(0, 10, "test");
        EXPECT_TRUE(test_list.erase(0, 10));
        EXPECT_TRUE(test_list.empty());
        EXPECT_EQ(test_list.size(), 0);
    }

    // Edge cases and error conditions
    TEST_F(BucketListTest, OverlappingRanges)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto first_spread = test_list.spread(0, 10, "test1");
        auto second_spread = test_list.spread(5, 15, "test2");
        EXPECT_EQ(second_spread, 2);
        EXPECT_EQ(test_list.size(), 3);

        auto it = test_list.begin();
        // First bucket: [0, 5) with test1
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 5);
        verifyContainerContents(it->values(), {"test1"});

        ++it;
        // Second bucket: [5, 10) with both test1 and test2
        EXPECT_EQ(it->low(), 5);
        EXPECT_EQ(it->high(), 10);
        verifyContainerContents(it->values(), {"test1", "test2"});

        ++it;
        // Third bucket: [10, 15) with test2
        EXPECT_EQ(it->low(), 10);
        EXPECT_EQ(it->high(), 15);
        verifyContainerContents(it->values(), {"test2"});
    }

    TEST_F(BucketListTest, RangeIteratorFunctionality)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto spread1 = test_list.spread(0, 10, "test1");
        [[maybe_unused]] auto spread2 = test_list.spread(20, 30, "test2");
        [[maybe_unused]] auto spread3 = test_list.spread(40, 50, "test3");

        auto range = test_list.range(15, 35);

        // Test forward range iteration over [15, 35]
        auto range_it = range.begin();
        auto range_end = range.end();

        EXPECT_NE(range_it, range_end);
        EXPECT_EQ(range_it->low(), 20);
        EXPECT_EQ(range_it->high(), 30);

        const auto &bucket = *range_it;
        const auto &values = bucket.values();
        verifyContainerContents(values, {"test2"});

        // Test reverse range iteration over [15, 35]
        auto range_rit = range.rbegin();
        auto range_rend = range.rend();

        EXPECT_NE(range_rit, range_rend);
        EXPECT_EQ(range_rit->low(), 20);
        EXPECT_EQ(range_rit->high(), 30);

        const auto &rbucket = *range_rit;
        const auto &rvalues = rbucket.values();
        verifyContainerContents(rvalues, {"test2"});
    }

    TEST_F(BucketListTest, ConstrainedRangeOperations)
    {
        TestBucketList test_list(0, 100);

        // Test out-of-bounds operations
        auto result1 = test_list.spread(-1, 10, "test");
        EXPECT_EQ(result1, 1); // Should succeed but be constrained to [0, 10)

        auto result2 = test_list.spread(90, 110, "test");
        EXPECT_EQ(result2, 1); // Should succeed but be constrained to [90, 100)

        // Verify the constrained ranges
        auto it = test_list.begin();
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 10);

        ++it;
        EXPECT_EQ(it->low(), 90);
        EXPECT_EQ(it->high(), 100);
    }

    // Bound tests
    TEST_F(BucketListTest, UnconstrainedBoundOperations)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto spread1 = test_list.spread(0, 10, "test1");
        [[maybe_unused]] auto spread2 = test_list.spread(20, 30, "test2");
        [[maybe_unused]] auto spread3 = test_list.spread(40, 50, "test3");

        // Should throw when called on unconstrained bucket_list
        bool lower_threw = false;
        bool upper_threw = false;

        try
        {
            [[maybe_unused]] auto lower = test_list.lower_bound();
        }
        catch (const std::runtime_error &)
        {
            lower_threw = true;
        }
        EXPECT_TRUE(lower_threw);

        try
        {
            [[maybe_unused]] auto upper = test_list.upper_bound();
        }
        catch (const std::runtime_error &)
        {
            upper_threw = true;
        }
        EXPECT_TRUE(upper_threw);
    }

    TEST_F(BucketListTest, ConstrainedBoundOperations)
    {
        TestBucketList test_list(0, 100);
        [[maybe_unused]] auto spread1 = test_list.spread(10, 20, "test1");
        [[maybe_unused]] auto spread2 = test_list.spread(30, 40, "test2");
        [[maybe_unused]] auto spread3 = test_list.spread(50, 60, "test3");

        // Should return the constraints, not the bucket bounds
        [[maybe_unused]] auto lower = test_list.lower_bound();
        [[maybe_unused]] auto upper = test_list.upper_bound();
        EXPECT_EQ(lower, 0);
        EXPECT_EQ(upper, 100);
    }

    TEST_F(BucketListTest, AutomaticOrdering)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto spread1 = test_list.spread(5, 7, "test3");
        [[maybe_unused]] auto spread2 = test_list.spread(1, 3, "test1");
        [[maybe_unused]] auto spread3 = test_list.spread(9, 11, "test5");

        auto it = test_list.begin();
        EXPECT_EQ(it->low(), 1);
        EXPECT_EQ(it->high(), 3);
        const auto &bucket1 = *it;
        const auto &values1 = bucket1.values();
        verifyContainerContents(values1, {"test1"});

        ++it;
        EXPECT_EQ(it->low(), 5);
        EXPECT_EQ(it->high(), 7);
        const auto &bucket2 = *it;
        const auto &values2 = bucket2.values();
        verifyContainerContents(values2, {"test3"});

        ++it;
        EXPECT_EQ(it->low(), 9);
        EXPECT_EQ(it->high(), 11);
        const auto &bucket3 = *it;
        const auto &values3 = bucket3.values();
        verifyContainerContents(values3, {"test5"});
    }

    TEST_F(BucketListTest, NodiscardAttributes)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto spread_result = test_list.spread(1, 3, "test");
        EXPECT_EQ(spread_result, 1);
    }

    TEST_F(BucketListTest, Concepts)
    {
        EXPECT_TRUE(std::totally_ordered<TestBucketList::index_type>);
        EXPECT_TRUE(std::equality_comparable<TestBucketList::index_type>);
    }

    TEST_F(BucketListTest, ReverseIteratorFunctionality)
    {
        TestBucketList test_list;
        [[maybe_unused]] auto spread1 = test_list.spread(1, 3, "test1");
        [[maybe_unused]] auto spread2 = test_list.spread(5, 7, "test2");
        [[maybe_unused]] auto spread3 = test_list.spread(9, 11, "test3");

        // Test reverse iteration
        auto rit = test_list.rbegin();
        EXPECT_NE(rit, test_list.rend());

        // First bucket should be [9, 11)
        EXPECT_EQ(rit->low(), 9);
        EXPECT_EQ(rit->high(), 11);
        const auto &bucket1 = *rit;
        const auto &values1 = bucket1.values();
        verifyContainerContents(values1, {"test3"});

        // Second bucket should be [5, 7)
        ++rit;
        EXPECT_NE(rit, test_list.rend());
        EXPECT_EQ(rit->low(), 5);
        EXPECT_EQ(rit->high(), 7);
        const auto &bucket2 = *rit;
        const auto &values2 = bucket2.values();
        verifyContainerContents(values2, {"test2"});

        // Third bucket should be [1, 3)
        ++rit;
        EXPECT_NE(rit, test_list.rend());
        EXPECT_EQ(rit->low(), 1);
        EXPECT_EQ(rit->high(), 3);
        const auto &bucket3 = *rit;
        const auto &values3 = bucket3.values();
        verifyContainerContents(values3, {"test1"});

        // Should be at the end
        ++rit;
        EXPECT_EQ(rit, test_list.rend());
    }

    int main(int argc, char **argv)
    {
        testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

}