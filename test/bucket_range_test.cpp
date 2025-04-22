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
#include <vector>
#include <string>

#include "bucket/bucket_list.h"
#include "bucket/bucket_map.h"
#include "bucket/bucket_range.h"

using namespace masutils;

// Test fixture for bucket_range tests
class BucketRangeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up a bucket_list with some test data
        list_buckets.spread(0, 10, "zero_to_ten");
        list_buckets.spread(20, 30, "twenty_to_thirty");
        list_buckets.spread(40, 50, "forty_to_fifty");
        
        // Set up a bucket_map with some test data
        map_buckets.spread(0, 10, "zero_to_ten");
        map_buckets.spread(20, 30, "twenty_to_thirty");
        map_buckets.spread(40, 50, "forty_to_fifty");
    }
    
    bucket_list<int, std::string> list_buckets;
    bucket_map<int, std::string> map_buckets;
};

// Test bucket_range with bucket_list
TEST_F(BucketRangeTest, ListRangeIteration) {
    // Create a range from 15 to 35
    bucket_range<bucket_list<int, std::string>, false> range(list_buckets, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 20);
    EXPECT_EQ(it->second.first, 30);
    EXPECT_EQ(it->second.second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test bucket_range with bucket_map
TEST_F(BucketRangeTest, MapRangeIteration) {
    // Create a range from 15 to 35
    bucket_range<bucket_map<int, std::string>, false> range(map_buckets, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 20);
    EXPECT_EQ(it->second.first, 30);
    EXPECT_EQ(it->second.second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test bucket_range with bucket_list and reverse iteration
TEST_F(BucketRangeTest, ListReverseIteration) {
    // Create a range from 15 to 35
    bucket_range<bucket_list<int, std::string>, false> range(list_buckets, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30 in reverse
    auto it = range.rbegin();
    ASSERT_NE(it, range.rend());
    EXPECT_EQ(it->first, 20);
    EXPECT_EQ(it->second.first, 30);
    EXPECT_EQ(it->second.second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.rend());
}

// Test bucket_range with bucket_map and reverse iteration
TEST_F(BucketRangeTest, MapReverseIteration) {
    // Create a range from 15 to 35
    bucket_range<bucket_map<int, std::string>, false> range(map_buckets, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30 in reverse
    auto it = range.rbegin();
    ASSERT_NE(it, range.rend());
    EXPECT_EQ(it->first, 20);
    EXPECT_EQ(it->second.first, 30);
    EXPECT_EQ(it->second.second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.rend());
}

// Test bucket_range with bucket_list and empty range
TEST_F(BucketRangeTest, ListEmptyRange) {
    // Create a range from 15 to 15 (empty range)
    bucket_range<bucket_list<int, std::string>, false> range(list_buckets, 15, 15);
    
    // Should be empty
    EXPECT_EQ(range.begin(), range.end());
}

// Test bucket_range with bucket_map and empty range
TEST_F(BucketRangeTest, MapEmptyRange) {
    // Create a range from 15 to 15 (empty range)
    bucket_range<bucket_map<int, std::string>, false> range(map_buckets, 15, 15);
    
    // Should be empty
    EXPECT_EQ(range.begin(), range.end());
}

// Test bucket_range with bucket_list and range covering multiple buckets
TEST_F(BucketRangeTest, ListMultipleBuckets) {
    // Create a range from 5 to 45 (covers all buckets)
    bucket_range<bucket_list<int, std::string>, false> range(list_buckets, 5, 45);
    
    // Should iterate over all buckets
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 0);
    EXPECT_EQ(it->second.first, 10);
    EXPECT_EQ(it->second.second.front(), "zero_to_ten");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 20);
    EXPECT_EQ(it->second.first, 30);
    EXPECT_EQ(it->second.second.front(), "twenty_to_thirty");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 40);
    EXPECT_EQ(it->second.first, 50);
    EXPECT_EQ(it->second.second.front(), "forty_to_fifty");
    
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test bucket_range with bucket_map and range covering multiple buckets
TEST_F(BucketRangeTest, MapMultipleBuckets) {
    // Create a range from 5 to 45 (covers all buckets)
    bucket_range<bucket_map<int, std::string>, false> range(map_buckets, 5, 45);
    
    // Should iterate over all buckets
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 0);
    EXPECT_EQ(it->second.first, 10);
    EXPECT_EQ(it->second.second.front(), "zero_to_ten");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 20);
    EXPECT_EQ(it->second.first, 30);
    EXPECT_EQ(it->second.second.front(), "twenty_to_thirty");
    
    ++it;
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 40);
    EXPECT_EQ(it->second.first, 50);
    EXPECT_EQ(it->second.second.front(), "forty_to_fifty");
    
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test bucket_range with bucket_list and const iteration
TEST_F(BucketRangeTest, ListConstIteration) {
    // Create a const range from 15 to 35
    bucket_range<bucket_list<int, std::string>, true> range(list_buckets, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 20);
    EXPECT_EQ(it->second.first, 30);
    EXPECT_EQ(it->second.second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.end());
}

// Test bucket_range with bucket_map and const iteration
TEST_F(BucketRangeTest, MapConstIteration) {
    // Create a const range from 15 to 35
    bucket_range<bucket_map<int, std::string>, true> range(map_buckets, 15, 35);
    
    // Should only iterate over the bucket from 20 to 30
    auto it = range.begin();
    ASSERT_NE(it, range.end());
    EXPECT_EQ(it->first, 20);
    EXPECT_EQ(it->second.first, 30);
    EXPECT_EQ(it->second.second.front(), "twenty_to_thirty");
    
    // Should be the end after one iteration
    ++it;
    EXPECT_EQ(it, range.end());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 