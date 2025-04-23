/**
 * @file  bucket_object_test.cpp
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
 * @brief Tests for the bucket_object class.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <string>

#include "bucket/bucket_object.h"
#include "mock_bucket_container.h"

using namespace masutils;
using namespace masutils::test;
using ::testing::Return;
using ::testing::_;

class BucketObjectTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up mock container with test data
        mock_container.add_bucket(0, 10, "zero_to_ten");
        mock_container.add_bucket(20, 30, "twenty_to_thirty");
        mock_container.add_bucket(40, 50, "forty_to_fifty");
    }
    
    MockBucketContainer<int, std::string> mock_container;
};

// Test bucket_object construction
TEST_F(BucketObjectTest, Construction) {
    std::vector<std::string> values = {"test_value"};
    bucket_object<int, std::vector<std::string>> obj(0, 10, values);
    
    EXPECT_EQ(obj.low(), 0);
    EXPECT_EQ(obj.high(), 10);
    EXPECT_EQ(obj.values().front(), "test_value");
}

// Test bucket_object const accessors
TEST_F(BucketObjectTest, ConstAccessors) {
    std::vector<std::string> values = {"test_value"};
    const bucket_object<int, std::vector<std::string>> obj(0, 10, values);
    
    EXPECT_EQ(obj.low(), 0);
    EXPECT_EQ(obj.high(), 10);
    EXPECT_EQ(obj.values().front(), "test_value");
}

// Test bucket_object non-const accessors
TEST_F(BucketObjectTest, NonConstAccessors) {
    std::vector<std::string> values = {"test_value"};
    bucket_object<int, std::vector<std::string>> obj(0, 10, values);
    
    obj.low() = 5;
    obj.high() = 15;
    obj.values().front() = "new_value";
    
    EXPECT_EQ(obj.low(), 5);
    EXPECT_EQ(obj.high(), 15);
    EXPECT_EQ(obj.values().front(), "new_value");
}

// Test bucket_object with multiple values
TEST_F(BucketObjectTest, MultipleValues) {
    std::vector<std::string> values = {"value1", "value2", "value3"};
    bucket_object<int, std::vector<std::string>> obj(0, 10, values);
    
    EXPECT_EQ(obj.low(), 0);
    EXPECT_EQ(obj.high(), 10);
    EXPECT_EQ(obj.values().size(), 3);
    EXPECT_EQ(obj.values()[0], "value1");
    EXPECT_EQ(obj.values()[1], "value2");
    EXPECT_EQ(obj.values()[2], "value3");
}

// Test bucket_object move semantics
TEST_F(BucketObjectTest, MoveSemantics) {
    std::vector<std::string> values = {"test_value"};
    bucket_object<int, std::vector<std::string>> obj(0, 10, std::move(values));
    
    EXPECT_EQ(obj.low(), 0);
    EXPECT_EQ(obj.high(), 10);
    EXPECT_EQ(obj.values().front(), "test_value");
    EXPECT_TRUE(values.empty()); // values should be moved from
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 