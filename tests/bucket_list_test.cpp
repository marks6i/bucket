#include <bucket/bucket_compare_traits.h>
#include <bucket/bucket_list.h>
#include <bucket/bucket_object.h>
#include <bucket/bucket_range.h>
#include <bucket/bucket_value_traits.h>
#include <gtest/gtest.h>
#include <list>
#include <memory>
#include <string>
#include <vector>

namespace masutils {
namespace test {

using bucket_list_type = bucket_list<int, std::string>;

class BucketListTest : public ::testing::Test {
protected:
  void SetUp() override {
    bucket = std::make_unique<bucket_list_type>();
    bucket->spread(0, 10, "test1");
    bucket->spread(20, 30, "test2");
    bucket->spread(40, 50, "test3");
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

  std::unique_ptr<bucket_list_type> bucket;
  bucket_list_type bucket_;
};

// Construction tests
TEST_F(BucketListTest, DefaultConstruction) {
  bucket_list_type test_bucket;
  EXPECT_FALSE(test_bucket.constrained());
  EXPECT_TRUE(test_bucket.empty());
  EXPECT_EQ(test_bucket.size(), 0);
}

TEST_F(BucketListTest, ConstrainedConstruction) {
  bucket_list_type test_bucket(0, 100);
  EXPECT_TRUE(test_bucket.constrained());
  EXPECT_TRUE(test_bucket.empty());
  EXPECT_EQ(test_bucket.size(), 0);
  EXPECT_EQ(test_bucket.low(), 0);
  EXPECT_EQ(test_bucket.high(), 100);
}

TEST_F(BucketListTest, InvalidConstrainedConstruction) {
  EXPECT_THROW(bucket_list_type test_bucket(100, 0), std::invalid_argument);
}

// Accessor tests
TEST_F(BucketListTest, AccessorFunctions) {
  bucket_list_type test_bucket;
  test_bucket.spread(0, 10, "test");

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

// Iterator tests
TEST_F(BucketListTest, IteratorFunctionality) {
  bucket_list_type test_bucket;
  test_bucket.spread(1, 3, "test1");
  test_bucket.spread(1, 3, "test2");
  test_bucket.spread(1, 3, "test3");

  auto it = test_bucket.begin();
  EXPECT_NE(it, test_bucket.end());
  EXPECT_EQ(it->low(), 1);
  EXPECT_EQ(it->high(), 3);
  const auto &bucket_values = *it;
  const auto &values = bucket_values.values();
  verifyContainerContents(values, {"test1", "test2", "test3"});
}

TEST_F(BucketListTest, ReverseIteratorFunctionality) {
  bucket_list_type test_bucket;
  test_bucket.spread(1, 3, "test1");
  test_bucket.spread(1, 3, "test2");
  test_bucket.spread(1, 3, "test3");

  auto rit = test_bucket.rbegin();
  EXPECT_NE(rit, test_bucket.rend());
  EXPECT_EQ(rit->low(), 1);
  EXPECT_EQ(rit->high(), 3);
  const auto &rbucket_values = *rit;
  const auto &rvalues = rbucket_values.values();
  verifyContainerContents(rvalues, {"test1", "test2", "test3"});
}

// Operation tests
TEST_F(BucketListTest, SpreadOperation) {
  bucket_list_type test_bucket;
  test_bucket.spread(0, 10, "test");
  EXPECT_EQ(test_bucket.size(), 1);
  EXPECT_FALSE(test_bucket.empty());

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketListTest, CoverOperation) {
  bucket_list_type test_bucket;
  test_bucket.cover(0, 10, "test");
  EXPECT_EQ(test_bucket.size(), 1);
  EXPECT_FALSE(test_bucket.empty());

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketListTest, EraseOperation) {
  bucket_list_type test_bucket;
  test_bucket.spread(0, 10, "test");
  test_bucket.erase(0, 10);
  EXPECT_TRUE(test_bucket.empty());
  EXPECT_EQ(test_bucket.size(), 0);
}

TEST_F(BucketListTest, EraseAllOperation) {
  bucket_list_type test_bucket;
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(20, 30, "test2");
  test_bucket.spread(40, 50, "test3");

  EXPECT_FALSE(test_bucket.empty());
  EXPECT_EQ(test_bucket.size(), 3);

  EXPECT_TRUE(test_bucket.erase());
  EXPECT_TRUE(test_bucket.empty());
  EXPECT_EQ(test_bucket.size(), 0);

  // Test erasing an already empty container
  EXPECT_FALSE(test_bucket.erase());
}

// Bounds tests
TEST_F(BucketListTest, UnconstrainedBoundOperations) {
  bucket_list_type test_bucket;
  test_bucket.spread(1, 3, "test1");
  test_bucket.spread(5, 7, "test2");
  test_bucket.spread(9, 11, "test3");

  EXPECT_THROW(
      { [[maybe_unused]] auto low = test_bucket.low(); }, std::runtime_error);
  EXPECT_THROW(
      { [[maybe_unused]] auto high = test_bucket.high(); }, std::runtime_error);
}

TEST_F(BucketListTest, ConstrainedBoundOperations) {
  bucket_list_type test_bucket(0, 100);
  test_bucket.spread(1, 3, "test1");
  test_bucket.spread(5, 7, "test2");
  test_bucket.spread(9, 11, "test3");

  EXPECT_EQ(test_bucket.low(), 0);
  EXPECT_EQ(test_bucket.high(), 100);
}

// Edge case tests
TEST_F(BucketListTest, OverlappingRangesSpread) {
  bucket_list_type test_bucket;

  // Test overlapping ranges with gap between buckets
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(20, 30, "test2");
  test_bucket.spread(5, 25, "test3");

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

TEST_F(BucketListTest, OverlappingRangesCover) {
  bucket_list_type test_bucket;

  // Test overlapping ranges with gap between buckets
  test_bucket.cover(0, 10, "test1");
  test_bucket.cover(20, 30, "test2");
  test_bucket.cover(5, 25, "test3");

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

TEST_F(BucketListTest, OverlappingRangesErase) {
  bucket_list_type test_bucket;

  // Test overlapping ranges with gap between buckets
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(20, 30, "test2");
  test_bucket.erase(5, 25);

  EXPECT_EQ(test_bucket.size(), 2);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 5);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 25);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
}

TEST_F(BucketListTest, OverlappingRangesWithConstraints) {
  bucket_list_type test_bucket(0, 100);

  // Test overlapping ranges with low value at constraint boundary
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(20, 30, "test2");
  test_bucket.spread(0, 25, "test3");

  EXPECT_EQ(test_bucket.size(), 4);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
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

TEST_F(BucketListTest, OverlappingRangesWithHighConstraint) {
  bucket_list_type test_bucket(0, 100);

  // Test overlapping ranges with high value at constraint boundary
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(20, 30, "test2");
  test_bucket.spread(5, 100, "test3");

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
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 30);
  EXPECT_EQ(it->high(), 100);
  verifyContainerContents(it->values(), {"test3"});
}

TEST_F(BucketListTest, OverlappingRangesWithLowConstraintIntersection) {
  bucket_list_type test_bucket(0, 100);

  // Test overlapping ranges with low value below constraint - should be clamped
  // to constraint
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(20, 30, "test2");
  test_bucket.spread(-10, 25, "test3");

  EXPECT_EQ(test_bucket.size(), 4);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0); // Clamped to constraint
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

TEST_F(BucketListTest, OverlappingRangesWithHighConstraintIntersection) {
  bucket_list_type test_bucket(0, 100);

  // Test overlapping ranges with high value above constraint - should be
  // clamped to constraint
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(20, 30, "test2");
  test_bucket.spread(5, 110, "test3");

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
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2", "test3"});

  ++it;
  EXPECT_EQ(it->low(), 30);
  EXPECT_EQ(it->high(), 100); // Clamped to constraint
  verifyContainerContents(it->values(), {"test3"});
}

TEST_F(BucketListTest, ConstrainedRangeOperations) {
  bucket_list_type test_bucket(0, 100);
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(20, 30, "test2");
  test_bucket.spread(40, 50, "test3");

  auto range1 = test_bucket.range(15, 35);
  EXPECT_NE(range1.begin(), range1.end());
  EXPECT_EQ(range1.begin()->low(), 20);
  EXPECT_EQ(range1.begin()->high(), 30);

  auto range2 = test_bucket.range(0, 100);
  EXPECT_NE(range2.begin(), range2.end());
  EXPECT_EQ(range2.begin()->low(), 0);
  EXPECT_EQ(range2.begin()->high(), 10);
}

TEST_F(BucketListTest, AutomaticOrdering) {
  bucket_list_type test_bucket;
  test_bucket.spread(5, 10, "test1");
  test_bucket.spread(0, 5, "test2");
  test_bucket.spread(10, 15, "test3");

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

// Multiple values in the same range
TEST_F(BucketListTest, MultipleValuesInSameRange) {
  bucket_list_type test_bucket;
  test_bucket.spread(0, 10, "test1");
  test_bucket.spread(0, 10, "test2");

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2"});
}

TEST_F(BucketListTest, DuplicateValuesInSameRange) {
  bucket_list_type test_bucket;
  test_bucket.spread(0, 10, "test");
  test_bucket.spread(0, 10, "test");

  auto it = test_bucket.begin();
  EXPECT_EQ(it->values().size(), 2);
  verifyContainerContents(it->values(), {"test", "test"});
}

// Additional tests for untested public members
TEST_F(BucketListTest, ConstrainedFunction) {
  bucket_list_type unconstrained_bucket;
  EXPECT_FALSE(unconstrained_bucket.constrained());

  bucket_list_type constrained_bucket(0, 100);
  EXPECT_TRUE(constrained_bucket.constrained());
}

TEST_F(BucketListTest, LowerBoundFunction) {
  bucket_list_type constrained_bucket(0, 100);
  EXPECT_EQ(constrained_bucket.lower_bound(), 0);

  bucket_list_type unconstrained_bucket;
  EXPECT_THROW(
      { [[maybe_unused]] auto bound = unconstrained_bucket.lower_bound(); },
      std::runtime_error);
}

TEST_F(BucketListTest, UpperBoundFunction) {
  bucket_list_type constrained_bucket(0, 100);
  EXPECT_EQ(constrained_bucket.upper_bound(), 100);

  bucket_list_type unconstrained_bucket;
  EXPECT_THROW(
      { [[maybe_unused]] auto bound = unconstrained_bucket.upper_bound(); },
      std::runtime_error);
}

TEST_F(BucketListTest, SpreadWithBucketType) {
  bucket_list_type test_bucket;
  bucket_list_type::bucket_type source_bucket(0, 10, {"test1", "test2"});

  test_bucket.spread(source_bucket);
  EXPECT_EQ(test_bucket.size(), 1);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2"});
}

TEST_F(BucketListTest, CoverWithBucketType) {
  bucket_list_type test_bucket;
  bucket_list_type::bucket_type source_bucket(0, 10, {"test1", "test2"});

  test_bucket.cover(source_bucket);
  EXPECT_EQ(test_bucket.size(), 1);

  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2"});
}

TEST_F(BucketListTest, SpreadWithBucketList) {
  bucket_list_type source_bucket;
  source_bucket.spread(0, 10, "test1");
  source_bucket.spread(20, 30, "test2");

  bucket_list_type target_bucket;
  target_bucket.spread(source_bucket);

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

TEST_F(BucketListTest, CoverWithBucketList) {
  bucket_list_type source_bucket;
  source_bucket.spread(0, 10, "test1");
  source_bucket.spread(20, 30, "test2");

  bucket_list_type target_bucket;
  target_bucket.cover(source_bucket);

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

TEST_F(BucketListTest, IteratorOperations) {
  // Initialize test data
  bucket_list_type test_bucket;
  test_bucket.spread(1, 2, "test1");
  test_bucket.spread(2, 3, "test2");
  test_bucket.spread(3, 4, "test3");

  // Test forward iteration
  auto it = test_bucket.begin();
  EXPECT_EQ(it->low(), 1);
  EXPECT_EQ(it->high(), 2);
  verifyContainerContents(it->values(), {"test1"});
  ++it;
  EXPECT_EQ(it->low(), 2);
  EXPECT_EQ(it->high(), 3);
  verifyContainerContents(it->values(), {"test2"});
  ++it;
  EXPECT_EQ(it->low(), 3);
  EXPECT_EQ(it->high(), 4);
  verifyContainerContents(it->values(), {"test3"});
  ++it;
  EXPECT_EQ(it, test_bucket.end());

  // Test const forward iteration
  const auto &const_bucket = test_bucket;
  auto cit = const_bucket.begin();
  EXPECT_EQ(cit->low(), 1);
  EXPECT_EQ(cit->high(), 2);
  verifyContainerContents(cit->values(), {"test1"});
  ++cit;
  EXPECT_EQ(cit->low(), 2);
  EXPECT_EQ(cit->high(), 3);
  verifyContainerContents(cit->values(), {"test2"});
  ++cit;
  EXPECT_EQ(cit->low(), 3);
  EXPECT_EQ(cit->high(), 4);
  verifyContainerContents(cit->values(), {"test3"});
  ++cit;
  EXPECT_EQ(cit, const_bucket.end());

  // Test reverse iteration
  auto rit = test_bucket.rbegin();
  EXPECT_EQ(rit->low(), 3);
  EXPECT_EQ(rit->high(), 4);
  verifyContainerContents(rit->values(), {"test3"});
  ++rit;
  EXPECT_EQ(rit->low(), 2);
  EXPECT_EQ(rit->high(), 3);
  verifyContainerContents(rit->values(), {"test2"});
  ++rit;
  EXPECT_EQ(rit->low(), 1);
  EXPECT_EQ(rit->high(), 2);
  verifyContainerContents(rit->values(), {"test1"});
  ++rit;
  EXPECT_EQ(rit, test_bucket.rend());

  // Test const reverse iteration
  auto crit = const_bucket.rbegin();
  EXPECT_EQ(crit->low(), 3);
  EXPECT_EQ(crit->high(), 4);
  verifyContainerContents(crit->values(), {"test3"});
  ++crit;
  EXPECT_EQ(crit->low(), 2);
  EXPECT_EQ(crit->high(), 3);
  verifyContainerContents(crit->values(), {"test2"});
  ++crit;
  EXPECT_EQ(crit->low(), 1);
  EXPECT_EQ(crit->high(), 2);
  verifyContainerContents(crit->values(), {"test1"});
  ++crit;
  EXPECT_EQ(crit, const_bucket.rend());
}

TEST_F(BucketListTest, IteratorComparison) {
  bucket_list_type test_bucket;
  test_bucket.spread(1, 2, "test1");
  test_bucket.spread(2, 3, "test2");
  test_bucket.spread(3, 4, "test3");

  auto it1 = test_bucket.begin();
  auto it2 = test_bucket.begin();
  auto it3 = ++test_bucket.begin();

  // Test equality
  EXPECT_EQ(it1, it2);
  EXPECT_NE(it1, it3);

  // Test const iterator equality
  const auto &const_bucket = test_bucket;
  auto cit1 = const_bucket.begin();
  auto cit2 = const_bucket.begin();
  auto cit3 = ++const_bucket.begin();

  EXPECT_EQ(cit1, cit2);
  EXPECT_NE(cit1, cit3);
}

TEST_F(BucketListTest, IteratorArithmetic) {
  bucket_list_type test_bucket;
  test_bucket.spread(1, 2, "test1");
  test_bucket.spread(2, 3, "test2");
  test_bucket.spread(3, 4, "test3");

  auto it = test_bucket.begin();

  // Test increment
  auto it2 = it;
  ++it2;
  EXPECT_EQ(it2->low(), 2);
  EXPECT_EQ(it2->high(), 3);
  verifyContainerContents(it2->values(), {"test2"});

  // Test decrement
  --it2;
  EXPECT_EQ(it2->low(), 1);
  EXPECT_EQ(it2->high(), 2);
  verifyContainerContents(it2->values(), {"test1"});
}

} // namespace test
} // namespace masutils