/**
 * @file  bucket_range_test.cpp
 * @copyright
 * Copyright 2024 Mark Solinski
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @brief Tests for the bucket_range class.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <string>

#include "bucket/bucket_range.h"
#include "mock_bucket_container.h"

using namespace masutils;
using namespace masutils::test;
using ::testing::Return;
using ::testing::_;

// Test fixture for bucket_range tests
class BucketRangeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up mock container with test data
        mock_container.add_bucket(0, 10, "zero_to_ten");
        mock_container.add_bucket(20, 30, "twenty_to_thirty");
        mock_container.add_bucket(40, 50, "forty_to_fifty");
    }
    
    MockBucketContainer<int, std::string> mock_container;
};

// Test bucket_range iteration
TEST_F(BucketRangeTest, RangeIteration) {
    // Create a range from 15 to 35
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 20);
    EXPECT_EQ(it->first.second, 30);
    EXPECT_EQ(it->second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test bucket_range with reverse iteration
TEST_F(BucketRangeTest, ReverseIteration) {
    // Create a range from 15 to 35
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30 in reverse
    auto it = range.rbegin();
    ASSERT_NE(it, range.rend());
    EXPECT_EQ(it->first.first, 20);
    EXPECT_EQ(it->first.second, 30);
    EXPECT_EQ(it->second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.rend());
}

// Test bucket_range with empty range
TEST_F(BucketRangeTest, EmptyRange) {
    // Create a range from 15 to 15 (empty range)
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 15, 15);
    
    // Should be empty
    EXPECT_EQ(range.begin(), range.end());
}

// Test bucket_range with range covering multiple buckets
TEST_F(BucketRangeTest, MultipleBuckets) {
    // Create a range from 5 to 45 (covers all buckets)
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 5, 45);
    
    // Should iterate over all buckets
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 0);
    EXPECT_EQ(it->first.second, 10);
    EXPECT_EQ(it->second.front(), "zero_to_ten");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 20);
    EXPECT_EQ(it->first.second, 30);
    EXPECT_EQ(it->second.front(), "twenty_to_thirty");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 40);
    EXPECT_EQ(it->first.second, 50);
    EXPECT_EQ(it->second.front(), "forty_to_fifty");
    
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test bucket_range with const iteration
TEST_F(BucketRangeTest, ConstIteration) {
    // Create a const range from 15 to 35
    bucket_range<MockBucketContainer<int, std::string>, true> range(mock_container, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 20);
    EXPECT_EQ(it->first.second, 30);
    EXPECT_EQ(it->second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test that forward and reverse iterations return the same buckets in reverse order
TEST_F(BucketRangeTest, ForwardReverseEquivalence) {
    // Create a range from 5 to 45 (covers all buckets)
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 5, 45);
    
    // Collect buckets from forward iteration
    std::vector<std::pair<int, int>> forward_buckets;
    for (auto it = range.begin(); it != range.end(); ++it) {
        forward_buckets.push_back({it->first.first, it->first.second});
    }
    
    // Collect buckets from reverse iteration
    std::vector<std::pair<int, int>> reverse_buckets;
    for (auto it = range.rbegin(); it != range.rend(); ++it) {
        reverse_buckets.push_back({it->first.first, it->first.second});
    }
    
    // Verify reverse_buckets is the reverse of forward_buckets
    ASSERT_EQ(forward_buckets.size(), reverse_buckets.size());
    for (size_t i = 0; i < forward_buckets.size(); ++i) {
        EXPECT_EQ(forward_buckets[i], reverse_buckets[forward_buckets.size() - 1 - i]);
    }
}

// Test buckets that begin or end on range boundaries
TEST_F(BucketRangeTest, BoundaryBuckets) {
    // Create a new container for this test
    MockBucketContainer<int, std::string> mock_container;
    mock_container.add_bucket(15, 25, "boundary_start");
    mock_container.add_bucket(30, 35, "boundary_end");
    
    // Create a range from 15 to 35
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 15, 35);
    
    // Should include both boundary buckets
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 15);
    EXPECT_EQ(it->first.second, 25);
    EXPECT_EQ(it->second.front(), "boundary_start");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 30);
    EXPECT_EQ(it->first.second, 35);
    EXPECT_EQ(it->second.front(), "boundary_end");
    
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test buckets that partially overlap with the range
TEST_F(BucketRangeTest, PartialOverlap) {
    // Create a new container for this test
    MockBucketContainer<int, std::string> mock_container;
    mock_container.add_bucket(10, 20, "overlap_start");
    mock_container.add_bucket(25, 35, "overlap_end");
    
    // Create a range from 15 to 30
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 15, 30);
    
    // Should include both partially overlapping buckets
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 10);
    EXPECT_EQ(it->first.second, 20);
    EXPECT_EQ(it->second.front(), "overlap_start");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 25);
    EXPECT_EQ(it->first.second, 35);
    EXPECT_EQ(it->second.front(), "overlap_end");
    
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test non-contiguous buckets
TEST_F(BucketRangeTest, NonContiguousBuckets) {
    // Create a new container for this test
    MockBucketContainer<int, std::string> mock_container;
    mock_container.add_bucket(15, 20, "gap1");
    mock_container.add_bucket(25, 30, "gap2");
    mock_container.add_bucket(35, 40, "gap3");
    
    // Create a range from 15 to 40
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 15, 40);
    
    // Should include all non-contiguous buckets
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 15);
    EXPECT_EQ(it->first.second, 20);
    EXPECT_EQ(it->second.front(), "gap1");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 25);
    EXPECT_EQ(it->first.second, 30);
    EXPECT_EQ(it->second.front(), "gap2");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 35);
    EXPECT_EQ(it->first.second, 40);
    EXPECT_EQ(it->second.front(), "gap3");
    
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test buckets outside the range are excluded
TEST_F(BucketRangeTest, ExcludedBuckets) {
    // Create a new container for this test
    MockBucketContainer<int, std::string> mock_container;
    mock_container.add_bucket(5, 10, "before");
    mock_container.add_bucket(15, 20, "inside");
    mock_container.add_bucket(25, 30, "inside");
    mock_container.add_bucket(35, 40, "after");
    
    // Create a range from 15 to 30
    bucket_range<MockBucketContainer<int, std::string>, false> range(mock_container, 15, 30);
    
    // Should only include buckets that overlap with the range
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 15);
    EXPECT_EQ(it->first.second, 20);
    EXPECT_EQ(it->second.front(), "inside");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first.first, 25);
    EXPECT_EQ(it->first.second, 30);
    EXPECT_EQ(it->second.front(), "inside");
    
    ++it;
    EXPECT_EQ(it, range.end());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 