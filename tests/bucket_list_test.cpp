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

    // Test fixture for bucket tests
    class BucketTest : public ::testing::Test
    {
    protected:
        using TestBucket = bucket_list<int, std::string>;
        using TestBucketType = TestBucket::bucket_type;
        using TestValueContainer = TestBucket::value_container;

        std::unique_ptr<TestBucket> bucket;
        TestBucket bucket_;

        BucketTest() : bucket_() {} // Initialize with default constructor

        void SetUp() override
        {
            bucket = std::make_unique<TestBucket>();
            [[maybe_unused]] auto spread1 = bucket->spread(0, 10, "test1");
            [[maybe_unused]] auto spread2 = bucket->spread(20, 30, "test2");
            [[maybe_unused]] auto spread3 = bucket->spread(40, 50, "test3");
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
    TEST_F(BucketTest, DefaultConstruction)
    {
        TestBucket test_bucket;
        EXPECT_FALSE(test_bucket.constrained());
        EXPECT_TRUE(test_bucket.empty());
        EXPECT_EQ(test_bucket.size(), 0);
    }

    TEST_F(BucketTest, ConstrainedConstruction)
    {
        TestBucket test_bucket(0, 100);
        EXPECT_TRUE(test_bucket.constrained());
        EXPECT_TRUE(test_bucket.empty());
        EXPECT_EQ(test_bucket.size(), 0);
        EXPECT_EQ(test_bucket.low(), 0);
        EXPECT_EQ(test_bucket.high(), 100);
    }

    TEST_F(BucketTest, InvalidConstrainedConstruction)
    {
        EXPECT_THROW(TestBucket test_bucket(100, 0), std::invalid_argument);
    }

    // Accessor tests
    TEST_F(BucketTest, AccessorFunctions)
    {
        TestBucket test_bucket;
        [[maybe_unused]] auto spread_result = test_bucket.spread(0, 10, "test");

        auto it = test_bucket.begin();
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 10);
        verifyContainerContents(it->values(), {"test"});
    }

    // Iterator tests
    TEST_F(BucketTest, IteratorFunctionality)
    {
        TestBucket test_bucket;
        [[maybe_unused]] auto spread_result = test_bucket.spread(1, 3, "test1");
        [[maybe_unused]] auto spread_result2 = test_bucket.spread(1, 3, "test2");
        [[maybe_unused]] auto spread_result3 = test_bucket.spread(1, 3, "test3");

        // Forward iteration
        auto it = test_bucket.begin();
        EXPECT_NE(it, test_bucket.end());
        EXPECT_EQ(it->low(), 1);
        EXPECT_EQ(it->high(), 3);
        const auto &bucket_values = *it;
        const auto &values = bucket_values.values();
        verifyContainerContents(values, {"test1", "test2", "test3"});

        // Reverse iteration
        auto rit = test_bucket.rbegin();
        EXPECT_NE(rit, test_bucket.rend());
        EXPECT_EQ(rit->low(), 1);
        EXPECT_EQ(rit->high(), 3);
        const auto &rbucket_values = *rit;
        const auto &rvalues = rbucket_values.values();
        verifyContainerContents(rvalues, {"test1", "test2", "test3"});
    }

    // Basic operations tests
    TEST_F(BucketTest, SpreadOperation)
    {
        TestBucket test_bucket;
        auto result = test_bucket.spread(0, 10, "test");
        EXPECT_EQ(result, 1);
        EXPECT_EQ(test_bucket.size(), 1);
        EXPECT_FALSE(test_bucket.empty());

        auto it = test_bucket.begin();
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 10);
        verifyContainerContents(it->values(), {"test"});
    }

    TEST_F(BucketTest, CoverOperation)
    {
        TestBucket test_bucket;
        auto result = test_bucket.cover(0, 10, "test");
        EXPECT_EQ(result, 1);
        EXPECT_EQ(test_bucket.size(), 1);
        EXPECT_FALSE(test_bucket.empty());

        auto it = test_bucket.begin();
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 10);
        verifyContainerContents(it->values(), {"test"});
    }

    TEST_F(BucketTest, EraseOperation)
    {
        TestBucket test_bucket;
        [[maybe_unused]] auto spread_result = test_bucket.spread(0, 10, "test");
        EXPECT_TRUE(test_bucket.erase(0, 10));
        EXPECT_TRUE(test_bucket.empty());
        EXPECT_EQ(test_bucket.size(), 0);
    }

    // Edge cases and error conditions
    TEST_F(BucketTest, OverlappingRanges)
    {
        TestBucket test_bucket;
        [[maybe_unused]] auto first_spread = test_bucket.spread(0, 10, "test1");
        auto second_spread = test_bucket.spread(5, 15, "test2");
        EXPECT_EQ(second_spread, 2);
        EXPECT_EQ(test_bucket.size(), 3);

        auto it = test_bucket.begin();
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

    TEST_F(BucketTest, RangeIteratorFunctionality)
    {
        TestBucket test_bucket;
        [[maybe_unused]] auto spread1 = test_bucket.spread(0, 10, "test1");
        [[maybe_unused]] auto spread2 = test_bucket.spread(20, 30, "test2");
        [[maybe_unused]] auto spread3 = test_bucket.spread(40, 50, "test3");

        auto range = test_bucket.range(15, 35);

        // Test forward range iteration over [15, 35]
        auto range_it = range.begin();
        auto range_end = range.end();

        EXPECT_NE(range_it, range_end);
        EXPECT_EQ(range_it->low(), 20);
        EXPECT_EQ(range_it->high(), 30);
        verifyContainerContents(range_it->values(), {"test2"});

        // Test reverse range iteration over [15, 35]
        auto range_rit = range.rbegin();
        auto range_rend = range.rend();

        EXPECT_NE(range_rit, range_rend);
        EXPECT_EQ(range_rit->low(), 20);
        EXPECT_EQ(range_rit->high(), 30);
        verifyContainerContents(range_rit->values(), {"test2"});
    }

    TEST_F(BucketTest, ConstrainedRangeOperations)
    {
        TestBucket test_bucket(0, 100);
        [[maybe_unused]] auto spread1 = test_bucket.spread(0, 10, "test1");
        [[maybe_unused]] auto spread2 = test_bucket.spread(20, 30, "test2");
        [[maybe_unused]] auto spread3 = test_bucket.spread(40, 50, "test3");

        // Test range within bounds
        auto range1 = test_bucket.range(15, 35);
        EXPECT_NE(range1.begin(), range1.end());
        EXPECT_EQ(range1.begin()->low(), 20);
        EXPECT_EQ(range1.begin()->high(), 30);

        // Test range at bounds
        auto range2 = test_bucket.range(0, 100);
        EXPECT_NE(range2.begin(), range2.end());
        EXPECT_EQ(range2.begin()->low(), 0);
        EXPECT_EQ(range2.begin()->high(), 10);
    }

    TEST_F(BucketTest, UnconstrainedBoundOperations)
    {
        TestBucket test_bucket;
        [[maybe_unused]] auto spread1 = test_bucket.spread(1, 3, "test1");
        [[maybe_unused]] auto spread2 = test_bucket.spread(5, 7, "test2");
        [[maybe_unused]] auto spread3 = test_bucket.spread(9, 11, "test3");

        // Should throw when called on unconstrained bucket
        EXPECT_THROW({ 
            [[maybe_unused]] auto low = test_bucket.low(); 
        }, std::runtime_error);
        EXPECT_THROW({ 
            [[maybe_unused]] auto high = test_bucket.high(); 
        }, std::runtime_error);
    }

    TEST_F(BucketTest, ConstrainedBoundOperations)
    {
        TestBucket test_bucket(0, 100);
        [[maybe_unused]] auto spread1 = test_bucket.spread(1, 3, "test1");
        [[maybe_unused]] auto spread2 = test_bucket.spread(5, 7, "test2");
        [[maybe_unused]] auto spread3 = test_bucket.spread(9, 11, "test3");

        EXPECT_EQ(test_bucket.low(), 0);
        EXPECT_EQ(test_bucket.high(), 100);
    }

    TEST_F(BucketTest, AutomaticOrdering)
    {
        TestBucket test_bucket;
        [[maybe_unused]] auto spread1 = test_bucket.spread(5, 10, "test1");
        [[maybe_unused]] auto spread2 = test_bucket.spread(0, 5, "test2");
        [[maybe_unused]] auto spread3 = test_bucket.spread(10, 15, "test3");

        auto it = test_bucket.begin();
        EXPECT_EQ(it->low(), 0);
        EXPECT_EQ(it->high(), 5);
        verifyContainerContents(it->values(), {"test2"});

        ++it;
        EXPECT_EQ(it->low(), 5);
        EXPECT_EQ(it->high(), 10);
        verifyContainerContents(it->values(), {"test1"});

        ++it;
        EXPECT_EQ(it->low(), 10);
        EXPECT_EQ(it->high(), 15);
        verifyContainerContents(it->values(), {"test3"});
    }

    TEST_F(BucketTest, NodiscardAttributes)
    {
        TestBucket test_bucket;
        // These functions return size_t, not void
        size_t spread_result = test_bucket.spread(0, 10, "test");
        size_t cover_result = test_bucket.cover(0, 10, "test");
        bool erase_result = test_bucket.erase(0, 10);
        
        // Use the results to avoid warnings
        EXPECT_GE(spread_result, 0);
        EXPECT_GE(cover_result, 0);
        EXPECT_TRUE(erase_result);
    }

    TEST_F(BucketTest, ReverseIteratorFunctionality)
    {
        TestBucket test_bucket;
        [[maybe_unused]] auto spread1 = test_bucket.spread(0, 10, "test1");
        [[maybe_unused]] auto spread2 = test_bucket.spread(20, 30, "test2");
        [[maybe_unused]] auto spread3 = test_bucket.spread(40, 50, "test3");

        auto rit = test_bucket.rbegin();
        EXPECT_NE(rit, test_bucket.rend());
        EXPECT_EQ(rit->low(), 40);
        EXPECT_EQ(rit->high(), 50);
        verifyContainerContents(rit->values(), {"test3"});

        ++rit;
        EXPECT_NE(rit, test_bucket.rend());
        EXPECT_EQ(rit->low(), 20);
        EXPECT_EQ(rit->high(), 30);
        verifyContainerContents(rit->values(), {"test2"});

        ++rit;
        EXPECT_NE(rit, test_bucket.rend());
        EXPECT_EQ(rit->low(), 0);
        EXPECT_EQ(rit->high(), 10);
        verifyContainerContents(rit->values(), {"test1"});

        ++rit;
        EXPECT_EQ(rit, test_bucket.rend());
    }

    int main(int argc, char **argv)
    {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
} // namespace masutils