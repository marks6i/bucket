#include <bucket/bucket_list.h>
#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <sstream>

using namespace masutils;

namespace masutils {
namespace test {

class BucketListReadmeTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

// Test basic range behavior example from README
TEST_F(BucketListReadmeTest, RangeBehavior) {
    bucket_list<int, std::string> list;
    list.spread(0, 5, "A");   // Covers [0, 1, 2, 3, 4] but not 5
    list.spread(5, 10, "B");  // Covers [5, 6, 7, 8, 9] but not 10
    
    // Verify first range
    auto it = list.begin();
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
TEST_F(BucketListReadmeTest, BasicOperations) {
    masutils::bucket_list<int, std::string> list;

    // Basic bucket operations
    list.spread(0, 5, "segment1");    // Add bucket in range [0, 5)
    list.cover(2, 4, "segment2");     // Override bucket in range [2, 4)
    list.erase(1, 3);                // Remove buckets in range [1, 3)

    // Querying and accessing
    bool has_value = list.contains(3);          // Check if index is in any bucket
    EXPECT_TRUE(has_value);
    
    if (has_value) {
        auto& values = list.at(3);              // Get values at index (throws if not found)
        EXPECT_EQ(values.front(), "segment2");
    }

    // Using find with exception handling
    try {
        auto it = list.find(3);                 // Find bucket containing index
        EXPECT_NE(it, list.end());
        EXPECT_EQ(it->values().front(), "segment2");
    } catch (const std::out_of_range& e) {
        FAIL() << "Unexpected exception: " << e.what();
    }

    // Safe pattern for accessing values
    if (list.contains(3)) {                     // Check first
        try {
            auto& values = list.at(3);          // Then access
            EXPECT_EQ(values.front(), "segment2");
        } catch (const std::out_of_range& e) {
            FAIL() << "This should never happen if contains() returned true";
        }
    }

    // Iterating through buckets in order
    std::stringstream output;
    for (const auto& bucket : list) {
        output << "Range [" << bucket.low() << ", " << bucket.high() << "): ";
        for (const auto& value : bucket.values()) {
            output << value << " ";
        }
        output << "\n";
    }
    
    std::string expected_output = "Range [0, 1): segment1 \nRange [3, 4): segment2 \nRange [4, 5): segment1 \n";
    EXPECT_EQ(output.str(), expected_output);
}

// Test advanced operations example from README
TEST_F(BucketListReadmeTest, AdvancedOperations) {
    bucket_list<int, std::string> list;
    
    list.erase();  // Clear all buckets
    EXPECT_EQ(list.begin(), list.end());

    // Building a sequence of buckets
    list.spread(0, 3, "first");    // Creates bucket in range [0, 3)
    list.spread(3, 6, "second");   // Creates bucket in range [3, 6)
    list.spread(6, 9, "third");    // Creates bucket in range [6, 9)

    // Modifying multiple buckets
    list.cover(2, 7, "overlap");   // Covers buckets in range [2, 7)
    list.erase(4, 8);             // Removes buckets in range [4, 8)

    // Verify final state
    auto it = list.begin();
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 2);
    EXPECT_EQ(it->values().front(), "first");

    ++it;
    EXPECT_EQ(it->low(), 2);
    EXPECT_EQ(it->high(), 4);
    EXPECT_EQ(it->values().front(), "overlap");

    ++it;
    EXPECT_EQ(it->low(), 8);
    EXPECT_EQ(it->high(), 9);
    EXPECT_EQ(it->values().front(), "third");
}

// Test custom types example from README
TEST_F(BucketListReadmeTest, CustomTypes) {
    struct TimeSegment {
        std::string activity;
        int duration;
        
        bool operator==(const TimeSegment& other) const {
            return activity == other.activity && duration == other.duration;
        }
    };

    masutils::bucket_list<int, TimeSegment> schedule;
    schedule.spread(0, 2, TimeSegment{"meeting", 120});  // Creates bucket in range [0, 2)
    schedule.spread(2, 4, TimeSegment{"lunch", 120});    // Creates bucket in range [2, 4)
    schedule.spread(4, 8, TimeSegment{"work", 240});     // Creates bucket in range [4, 8)

    // Verify first segment
    auto it = schedule.begin();
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 2);
    EXPECT_EQ(it->values().front(), (TimeSegment{"meeting", 120}));

    // Verify second segment
    ++it;
    EXPECT_EQ(it->low(), 2);
    EXPECT_EQ(it->high(), 4);
    EXPECT_EQ(it->values().front(), (TimeSegment{"lunch", 120}));

    // Verify third segment
    ++it;
    EXPECT_EQ(it->low(), 4);
    EXPECT_EQ(it->high(), 8);
    EXPECT_EQ(it->values().front(), (TimeSegment{"work", 240}));
}

} // namespace test
} // namespace masutils 