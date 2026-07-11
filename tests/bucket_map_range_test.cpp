#include <bucket/bucket_map.h>
#include <gtest/gtest.h>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <iostream>

namespace masutils::test {

using container_type = bucket_map<int, std::string>;

class BucketMapRangeTest : public ::testing::Test {
protected:
  void SetUp() override {
    container = std::make_unique<container_type>();
    container->spread(0, 10, "test1");
    container->spread(20, 30, "test2");
    container->spread(40, 50, "test3");
  }

  void TearDown() override {
    // Cleanup code if needed
  }

  // Helper function to verify container contents
  template <typename Container>
  void verifyContainerContents(const Container &values,
                              const std::vector<std::string> &expected_values) {
    ASSERT_EQ(values.size(), expected_values.size());
    auto it = values.cbegin();
    for (const auto &expected : expected_values) {
      ASSERT_EQ(*it, expected);
      ++it;
    }
  }

  std::unique_ptr<container_type> container;
};

// Basic range tests
TEST_F(BucketMapRangeTest, EmptyRange) {
  auto range = container->range(12, 18);
  EXPECT_EQ(range.begin(), range.end());
}

TEST_F(BucketMapRangeTest, SingleBucketRange) {
  auto range = container->range(5, 15);
  auto it = range.begin();
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});
  ++it;
  EXPECT_EQ(it, range.end());
}

TEST_F(BucketMapRangeTest, MultipleBucketRange) {
  auto range = container->range(5, 35);
  auto it = range.begin();
  
  // First bucket
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});
  
  // Second bucket
  ++it;
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
  
  // End
  ++it;
  EXPECT_EQ(it, range.end());
}

TEST_F(BucketMapRangeTest, ReverseIteratorRange) {
  auto range = container->range(5, 35);
  auto it = range.rbegin();
  
  // Last bucket
  EXPECT_NE(it, range.rend());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
  
  // First bucket
  ++it;
  EXPECT_NE(it, range.rend());
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});
  
  // End
  ++it;
  EXPECT_EQ(it, range.rend());
}

// Range method tests
TEST_F(BucketMapRangeTest, ContainsMethod) {
  auto range = container->range(5, 35);
  
  // Test points within range
  EXPECT_TRUE(range.contains(7));  // In first bucket
  EXPECT_TRUE(range.contains(25)); // In second bucket
  
  // Test points outside range but in container
  EXPECT_FALSE(range.contains(45)); // In third bucket but outside range
  
  // Test points in gaps
  EXPECT_FALSE(range.contains(15)); // In gap between buckets
  EXPECT_FALSE(range.contains(2));  // Before range
  EXPECT_FALSE(range.contains(32)); // After range
}

TEST_F(BucketMapRangeTest, FindMethod) {
  auto range = container->range(5, 35);
  
  // Find in first bucket
  auto it = range.find(7);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  
  // Find in second bucket
  it = range.find(25);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  
  // Find in gap
  it = range.find(15);
  EXPECT_EQ(it, range.end());
  
  // Find outside range
  EXPECT_THROW(range.find(45), std::out_of_range);
  EXPECT_THROW(range.find(2), std::out_of_range);
}

TEST_F(BucketMapRangeTest, AtMethod) {
  auto range = container->range(5, 35);
  
  // At in first bucket
  auto it = range.at(7);
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  
  // At in second bucket
  it = range.at(25);
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  
  // At in gap
  EXPECT_THROW(range.at(15), std::out_of_range);
  
  // At outside range
  EXPECT_THROW(range.at(45), std::out_of_range);
  EXPECT_THROW(range.at(2), std::out_of_range);
}

TEST_F(BucketMapRangeTest, NextMethod) {
  auto range = container->range(5, 35);
  
  // Next from first bucket
  auto it = range.next(7);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  
  // Next from gap
  it = range.next(15);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  
  // Next from second bucket
  it = range.next(25);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  
  // Next from after last bucket in range
  it = range.next(32);
  EXPECT_EQ(it, range.end());
  
  // Next from outside range
  it = range.next(45);
  EXPECT_EQ(it, range.end());
}

TEST_F(BucketMapRangeTest, PreviousMethod) {
  auto range = container->range(5, 35);
  
  // Previous from second bucket
  auto it = range.previous(25);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  
  // Previous from gap
  it = range.previous(15);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  
  // Previous from first bucket
  it = range.previous(7);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  
  // Previous from before first bucket in range
  it = range.previous(2);
  EXPECT_EQ(it, range.end());
  
  // Previous from outside range
  it = range.previous(45);
  EXPECT_EQ(it, range.end());
}

TEST_F(BucketMapRangeTest, ConstMethods) {
  const auto& const_container = *container;
  auto range = const_container.range(5, 35);
  
  // Test const contains
  EXPECT_TRUE(range.contains(7));
  EXPECT_FALSE(range.contains(45));
  
  // Test const find
  auto it = range.find(7);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 0);
  
  // Test const at
  it = range.at(7);
  EXPECT_EQ(it->low(), 0);
  
  // Test const next
  it = range.next(7);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 0);
  
  // Test const previous
  it = range.previous(25);
  EXPECT_NE(it, range.end());
  EXPECT_EQ(it->low(), 20);
}

} // namespace masutils::test // namespace masutils 