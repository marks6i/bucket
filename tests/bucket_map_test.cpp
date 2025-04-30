#include "bucket/bucket_compare_traits.h"
#include "bucket/bucket_map.h"
#include "bucket/bucket_object.h"
#include "bucket/bucket_range.h"
#include "bucket/bucket_value_traits.h"
#include <gtest/gtest.h>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace masutils {
namespace test {

using bucket_map_type = bucket_map<int, std::string>;

class BucketMapTest : public ::testing::Test {
protected:
  void SetUp() override {
    bucket = std::make_unique<bucket_map_type>();
    [[maybe_unused]] auto spread1 = bucket->spread(0, 10, "test1");
    [[maybe_unused]] auto spread2 = bucket->spread(20, 30, "test2");
    [[maybe_unused]] auto spread3 = bucket->spread(40, 50, "test3");
  }

  void TearDown() override {
    // Cleanup code if needed
  }

  // Helper function to verify container contents
  void
  verifyContainerContents(const std::list<std::string> &container,
                          const std::vector<std::string> &expected_values) {
    ASSERT_EQ(container.size(), expected_values.size());
    auto it = container.begin();
    for (const auto &expected : expected_values) {
      ASSERT_EQ(*it, expected);
      ++it;
    }
  }

  std::unique_ptr<bucket_map_type> bucket;
  bucket_map_type bucket_;
};

// Construction tests
TEST_F(BucketMapTest, DefaultConstruction) {
  bucket_map_type test_bucket;
  EXPECT_FALSE(test_bucket.constrained());
  EXPECT_TRUE(test_bucket.empty());
  EXPECT_EQ(test_bucket.size(), 0);
}

TEST_F(BucketMapTest, ConstrainedConstruction) {
  bucket_map_type test_bucket(0, 100);
  EXPECT_TRUE(test_bucket.constrained());
  EXPECT_TRUE(test_bucket.empty());
  EXPECT_EQ(test_bucket.size(), 0);
  EXPECT_EQ(test_bucket.low(), 0);
  EXPECT_EQ(test_bucket.high(), 100);
}

TEST_F(BucketMapTest, InvalidConstrainedConstruction) {
  EXPECT_THROW(bucket_map_type test_bucket(100, 0), std::invalid_argument);
}

// Accessor tests
TEST_F(BucketMapTest, AccessorFunctions) {
  bucket_map_type test_bucket;
  [[maybe_unused]] auto spread_result = test_bucket.spread(0, 10, "test");

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

// Iterator tests
TEST_F(BucketMapTest, IteratorFunctionality) {
  bucket_map_type test_bucket;
  [[maybe_unused]] auto spread_result = test_bucket.spread(1, 3, "test1");
  [[maybe_unused]] auto spread_result2 = test_bucket.spread(1, 3, "test2");
  [[maybe_unused]] auto spread_result3 = test_bucket.spread(1, 3, "test3");

  auto it = test_bucket.begin();
  EXPECT_NE(it, test_bucket.end());
  EXPECT_EQ(it->low(), 1);
  EXPECT_EQ(it->high(), 3);
  const auto &bucket_values = *it;
  const auto &values = bucket_values.values();
  verifyContainerContents(values, {"test1", "test2", "test3"});
}

TEST_F(BucketMapTest, ReverseIteratorFunctionality) {
  bucket_map_type test_bucket;
  [[maybe_unused]] auto spread_result = test_bucket.spread(1, 3, "test1");
  [[maybe_unused]] auto spread_result2 = test_bucket.spread(1, 3, "test2");
  [[maybe_unused]] auto spread_result3 = test_bucket.spread(1, 3, "test3");

  auto rit = test_bucket.rbegin();
  EXPECT_NE(rit, test_bucket.rend());
  EXPECT_EQ(rit->low(), 1);
  EXPECT_EQ(rit->high(), 3);
  const auto &rbucket_values = *rit;
  const auto &rvalues = rbucket_values.values();
  verifyContainerContents(rvalues, {"test1", "test2", "test3"});
}

// Operation tests
TEST_F(BucketMapTest, SpreadOperation) {
  bucket_map_type test_bucket;
  [[maybe_unused]] auto result = test_bucket.spread(0, 10, "test");
  EXPECT_EQ(test_bucket.size(), 1);
  EXPECT_FALSE(test_bucket.empty());

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketMapTest, CoverOperation) {
  bucket_map_type test_bucket;
  [[maybe_unused]] auto result = test_bucket.cover(0, 10, "test");
  EXPECT_EQ(test_bucket.size(), 1);
  EXPECT_FALSE(test_bucket.empty());

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketMapTest, EraseOperation) {
  bucket_map_type test_bucket;
  [[maybe_unused]] auto spread_result = test_bucket.spread(0, 10, "test");
  [[maybe_unused]] auto erase_result = test_bucket.erase(0, 10);
  EXPECT_TRUE(erase_result);
  EXPECT_TRUE(test_bucket.empty());
  EXPECT_EQ(test_bucket.size(), 0);
}

// Bounds tests
TEST_F(BucketMapTest, UnconstrainedBoundOperations) {
  bucket_map_type test_bucket;
  [[maybe_unused]] auto spread1 = test_bucket.spread(1, 3, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(5, 7, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(9, 11, "test3");

  EXPECT_THROW(
      { [[maybe_unused]] auto low = test_bucket.low(); }, std::runtime_error);
  EXPECT_THROW(
      { [[maybe_unused]] auto high = test_bucket.high(); }, std::runtime_error);
}

TEST_F(BucketMapTest, ConstrainedBoundOperations) {
  bucket_map_type test_bucket(0, 100);
  [[maybe_unused]] auto spread1 = test_bucket.spread(1, 3, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(5, 7, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(9, 11, "test3");

  EXPECT_EQ(test_bucket.low(), 0);
  EXPECT_EQ(test_bucket.high(), 100);
}

// Edge case tests
TEST_F(BucketMapTest, OverlappingRangesSpread) {
  bucket_map_type test_bucket;

  // Test overlapping ranges with gap between buckets
  [[maybe_unused]] auto spread1 = test_bucket.spread(0, 10, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(20, 30, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(5, 25, "test3");

  EXPECT_EQ(test_bucket.size(), 5);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 5);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 5);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 10);
  EXPECT_EQ(it->high(), 20);
  verifyContainerContents(it->values(), {"test3"});

  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 25);
  verifyContainerContents(it->values(), {"test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 25);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
}

TEST_F(BucketMapTest, OverlappingRangesCover) {
  bucket_map_type test_bucket;

  // Test overlapping ranges with gap between buckets
  [[maybe_unused]] auto cover1 = test_bucket.cover(0, 10, "test1");
  [[maybe_unused]] auto cover2 = test_bucket.cover(20, 30, "test2");
  [[maybe_unused]] auto cover3 = test_bucket.cover(5, 25, "test3");

  EXPECT_EQ(test_bucket.size(), 3);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 5);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 5);
  EXPECT_EQ(it->high(), 25);
  verifyContainerContents(it->values(), {"test3"});

  ++it;
  EXPECT_EQ(it->low(), 25);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
}

TEST_F(BucketMapTest, OverlappingRangesErase) {
  bucket_map_type test_bucket;

  // Test overlapping ranges with gap between buckets
  [[maybe_unused]] auto spread1 = test_bucket.spread(0, 10, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(20, 30, "test2");
  [[maybe_unused]] auto erase_result = test_bucket.erase(5, 25);

  EXPECT_EQ(test_bucket.size(), 2);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 5);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
}

TEST_F(BucketMapTest, OverlappingRangesWithConstraints) {
  bucket_map_type test_bucket(0, 100);

  // Test overlapping ranges with constraints
  [[maybe_unused]] auto spread1 = test_bucket.spread(10, 20, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(15, 25, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(5, 30, "test3");

  EXPECT_EQ(test_bucket.size(), 5);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 5);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test3"});

  ++it;
  EXPECT_EQ(it->low(), 10);
  EXPECT_EQ(it->high(), 15);
  verifyContainerContents(it->values(), {"test1", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 15);
  EXPECT_EQ(it->high(), 20);
  verifyContainerContents(it->values(), {"test1", "test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 25);
  verifyContainerContents(it->values(), {"test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 25);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test3"});
}

TEST_F(BucketMapTest, OverlappingRangesWithHighConstraint) {
  bucket_map_type test_bucket(0, 100);

  // Test overlapping ranges with high constraint
  [[maybe_unused]] auto spread1 = test_bucket.spread(80, 90, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(85, 95, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(75, 100, "test3");

  EXPECT_EQ(test_bucket.size(), 4);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 75);
  EXPECT_EQ(it->high(), 80);
  verifyContainerContents(it->values(), {"test3"});

  ++it;
  EXPECT_EQ(it->low(), 80);
  EXPECT_EQ(it->high(), 85);
  verifyContainerContents(it->values(), {"test1", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 85);
  EXPECT_EQ(it->high(), 90);
  verifyContainerContents(it->values(), {"test1", "test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 90);
  EXPECT_EQ(it->high(), 95);
  verifyContainerContents(it->values(), {"test2", "test3"});
}

TEST_F(BucketMapTest, OverlappingRangesWithLowConstraintIntersection) {
  bucket_map_type test_bucket(0, 100);

  // Test overlapping ranges with low constraint intersection
  [[maybe_unused]] auto spread1 = test_bucket.spread(0, 10, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(5, 15, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(-5, 20, "test3");

  EXPECT_EQ(test_bucket.size(), 4);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 5);
  verifyContainerContents(it->values(), {"test1", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 5);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 10);
  EXPECT_EQ(it->high(), 15);
  verifyContainerContents(it->values(), {"test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 15);
  EXPECT_EQ(it->high(), 20);
  verifyContainerContents(it->values(), {"test3"});
}

TEST_F(BucketMapTest, OverlappingRangesWithHighConstraintIntersection) {
  bucket_map_type test_bucket(0, 100);

  // Test overlapping ranges with high constraint intersection
  [[maybe_unused]] auto spread1 = test_bucket.spread(80, 90, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(85, 95, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(75, 105, "test3");

  EXPECT_EQ(test_bucket.size(), 4);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 75);
  EXPECT_EQ(it->high(), 80);
  verifyContainerContents(it->values(), {"test3"});

  ++it;
  EXPECT_EQ(it->low(), 80);
  EXPECT_EQ(it->high(), 85);
  verifyContainerContents(it->values(), {"test1", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 85);
  EXPECT_EQ(it->high(), 90);
  verifyContainerContents(it->values(), {"test1", "test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 90);
  EXPECT_EQ(it->high(), 95);
  verifyContainerContents(it->values(), {"test2", "test3"});
}

TEST_F(BucketMapTest, ConstrainedRangeOperations) {
  bucket_map_type test_bucket(0, 100);

  // Test constrained range operations
  [[maybe_unused]] auto spread1 = test_bucket.spread(10, 20, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(30, 40, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(50, 60, "test3");

  EXPECT_EQ(test_bucket.size(), 3);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 10);
  EXPECT_EQ(it->high(), 20);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 30);
  EXPECT_EQ(it->high(), 40);
  verifyContainerContents(it->values(), {"test2"});

  ++it;
  EXPECT_EQ(it->low(), 50);
  EXPECT_EQ(it->high(), 60);
  verifyContainerContents(it->values(), {"test3"});
}

TEST_F(BucketMapTest, AutomaticOrdering) {
  bucket_map_type test_bucket;

  // Test automatic ordering of ranges
  [[maybe_unused]] auto spread1 = test_bucket.spread(30, 40, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(10, 20, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(50, 60, "test3");

  EXPECT_EQ(test_bucket.size(), 3);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 10);
  EXPECT_EQ(it->high(), 20);
  verifyContainerContents(it->values(), {"test2"});

  ++it;
  EXPECT_EQ(it->low(), 30);
  EXPECT_EQ(it->high(), 40);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 50);
  EXPECT_EQ(it->high(), 60);
  verifyContainerContents(it->values(), {"test3"});
}

TEST_F(BucketMapTest, MultipleValuesInSameRange) {
  bucket_map_type test_bucket;

  // Test multiple values in the same range
  [[maybe_unused]] auto spread1 = test_bucket.spread(0, 10, "test1");
  [[maybe_unused]] auto spread2 = test_bucket.spread(0, 10, "test2");
  [[maybe_unused]] auto spread3 = test_bucket.spread(0, 10, "test3");

  EXPECT_EQ(test_bucket.size(), 1);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2", "test3"});
}

TEST_F(BucketMapTest, DuplicateValuesInSameRange) {
  bucket_map_type test_bucket;

  // Test duplicate values in the same range
  [[maybe_unused]] auto spread1 = test_bucket.spread(0, 10, "test");
  [[maybe_unused]] auto spread2 = test_bucket.spread(0, 10, "test");
  [[maybe_unused]] auto spread3 = test_bucket.spread(0, 10, "test");

  EXPECT_EQ(test_bucket.size(), 1);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test", "test", "test"});
}

// Additional tests for untested public members
TEST_F(BucketMapTest, ConstrainedFunction) {
  bucket_map_type unconstrained_bucket;
  EXPECT_FALSE(unconstrained_bucket.constrained());

  bucket_map_type constrained_bucket(0, 100);
  EXPECT_TRUE(constrained_bucket.constrained());
}

TEST_F(BucketMapTest, LowerBoundFunction) {
  bucket_map_type constrained_bucket(0, 100);
  EXPECT_EQ(constrained_bucket.lower_bound(), 0);

  bucket_map_type unconstrained_bucket;
  EXPECT_THROW(
      { [[maybe_unused]] auto bound = unconstrained_bucket.lower_bound(); },
      std::runtime_error);
}

TEST_F(BucketMapTest, UpperBoundFunction) {
  bucket_map_type constrained_bucket(0, 100);
  EXPECT_EQ(constrained_bucket.upper_bound(), 100);

  bucket_map_type unconstrained_bucket;
  EXPECT_THROW(
      { [[maybe_unused]] auto bound = unconstrained_bucket.upper_bound(); },
      std::runtime_error);
}

TEST_F(BucketMapTest, SpreadWithBucketType) {
  bucket_map_type test_bucket;
  bucket_map_type::bucket_type source_bucket(0, 10, {"test1", "test2"});

  [[maybe_unused]] auto result = test_bucket.spread(source_bucket);
  EXPECT_EQ(test_bucket.size(), 1);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2"});
}

TEST_F(BucketMapTest, CoverWithBucketType) {
  bucket_map_type test_bucket;
  bucket_map_type::bucket_type source_bucket(0, 10, {"test1", "test2"});

  [[maybe_unused]] auto result = test_bucket.cover(source_bucket);
  EXPECT_EQ(test_bucket.size(), 1);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2"});
}

TEST_F(BucketMapTest, SpreadWithBucketMap) {
  bucket_map_type source_bucket;
  [[maybe_unused]] auto spread1 = source_bucket.spread(0, 10, "test1");
  [[maybe_unused]] auto spread2 = source_bucket.spread(20, 30, "test2");

  bucket_map_type target_bucket;
  [[maybe_unused]] auto result = target_bucket.spread(source_bucket);

  EXPECT_EQ(target_bucket.size(), 2);

  auto it = target_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
}

TEST_F(BucketMapTest, CoverWithBucketMap) {
  bucket_map_type source_bucket;
  [[maybe_unused]] auto spread1 = source_bucket.spread(0, 10, "test1");
  [[maybe_unused]] auto spread2 = source_bucket.spread(20, 30, "test2");

  bucket_map_type target_bucket;
  [[maybe_unused]] auto result = target_bucket.cover(source_bucket);

  EXPECT_EQ(target_bucket.size(), 2);

  auto it = target_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
}

} // namespace test
} // namespace masutils