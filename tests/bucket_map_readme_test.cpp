#include <bucket/bucket_map.h>
#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace masutils;

namespace masutils {
namespace test {

class BucketMapReadmeTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test basic range behavior example from README
TEST_F(BucketMapReadmeTest, RangeBehavior) {
    bucket_map<int, std::string> map;
    map.spread(0, 5, "A");    // Covers indices [0,1,2,3,4]
    map.spread(5, 10, "B");   // Covers indices [5,6,7,8,9]
    
    // Verify first range
    auto it = map.begin();
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 5);
    EXPECT_EQ(it->values().front(), "A");
    
    // Verify second range
    ++it;
    EXPECT_EQ(it->low(), 5);
    EXPECT_EQ(it->high(), 10);
    EXPECT_EQ(it->values().front(), "B");
}

// Test basic operations example from README
TEST_F(BucketMapReadmeTest, BasicOperations) {
    bucket_map<int, std::string> map;
    
    bool exists = map.contains(2);
    EXPECT_FALSE(exists);
    
    // Test iteration
    map.spread(0, 5, "value1");
    map.spread(5, 10, "value2");
    
    std::stringstream output;
    for (const auto& bucket : map) {
        output << "Range [" << bucket.low() << ", " << bucket.high() << "): ";
        for (const auto& value : bucket.values()) {
            output << value << " ";
        }
        output << "\n";
    }
    
    std::string expected_output = "Range [0, 5): value1 \nRange [5, 10): value2 \n";
    EXPECT_EQ(output.str(), expected_output);
    
    // Test advanced operations
    map.erase();  // Clear all buckets
    EXPECT_EQ(map.begin(), map.end());
    
    map.spread(0, 10, "base");      // Creates bucket in range [0, 10)
    map.cover(3, 7, "middle");      // Creates bucket in range [3, 7)
    map.spread(5, 8, "overlap");    // Creates bucket in range [5, 8)
    
    // Verify final state matches actual behavior
    auto it = map.begin();
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 3);
    EXPECT_EQ(it->values().front(), "base");
    
    ++it;
    EXPECT_EQ(it->low(), 3);
    EXPECT_EQ(it->high(), 5);
    EXPECT_EQ(it->values().front(), "middle");
    
    ++it;
    EXPECT_EQ(it->low(), 5);
    EXPECT_EQ(it->high(), 7);
    ASSERT_EQ(it->values().size(), 2);
    auto values_it = it->values().begin();
    EXPECT_EQ(*values_it, "middle");
    ++values_it;
    EXPECT_EQ(*values_it, "overlap");
    
    ++it;
    EXPECT_EQ(it->low(), 7);
    EXPECT_EQ(it->high(), 8);
    ASSERT_EQ(it->values().size(), 2);
    values_it = it->values().begin();
    EXPECT_EQ(*values_it, "base");
    ++values_it;
    EXPECT_EQ(*values_it, "overlap");
    
    ++it;
    EXPECT_EQ(it->low(), 8);
    EXPECT_EQ(it->high(), 10);
    EXPECT_EQ(it->values().front(), "base");
}

// Test custom types example from README
TEST_F(BucketMapReadmeTest, CustomTypes) {
    struct CustomValue {
        int priority;
        std::string data;
        
        bool operator==(const CustomValue& other) const {
            return priority == other.priority && data == other.data;
        }
    };
    
    masutils::bucket_map<double, CustomValue> custom_map;
    custom_map.spread(0.0, 1.0, CustomValue{1, "low"});   // Creates bucket in range [0.0, 1.0)
    custom_map.spread(1.0, 2.0, CustomValue{2, "high"});  // Creates bucket in range [1.0, 2.0)
    
    // Test bucket lookup operations
    auto overlapping_buckets = custom_map.range(0.5, 1.5);
    
    std::stringstream output;
    for (const auto& bucket : overlapping_buckets) {
        output << "Bucket [" << bucket.low() << ", " << bucket.high() << "): ";
        for (const auto& value : bucket.values()) {
            output << value.priority << "," << value.data << " ";
        }
        output << "\n";
    }
    
    std::string expected_output = "Bucket [0, 1): 1,low \nBucket [1, 2): 2,high \n";
    EXPECT_EQ(output.str(), expected_output);
}

} // namespace test
} // namespace masutils 