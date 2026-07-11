#include <bucket/bucket_list.h>
#include <gtest/gtest.h>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <iostream>

// Specialization of bucket_value_traits for std::set
template <>
struct masutils::bucket_value_traits<std::string, std::set<std::string>> {
  using value_type = std::string;
  using value_container = std::set<std::string>;

  static void add(value_container &c, const value_type &v) {
    c.insert(v);
  }

  static void append(value_container &c, const value_container &other) {
    c.insert(other.begin(), other.end());
  }

  static void remove(value_container &c, const value_type &v) {
    c.erase(v);
  }
};

namespace masutils {
namespace test {

using container_type = bucket_list<int, std::string>;

class BucketListTest : public ::testing::Test {
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
    auto it = values.begin();
    for (const auto &expected : expected_values) {
      ASSERT_EQ(*it, expected);
      ++it;
    }
  }

  void print_actual_ranges(const bucket_list<int, std::string>& cont) {
    std::string actual;
    for (auto it = cont.begin(); it != cont.end(); ++it) {
      if (!actual.empty()) {
        actual += ",";
      }
      actual += "[" + std::to_string(it->low()) + "," + std::to_string(it->high()) + ")";
    }
    std::cout << "Current ranges: " << actual << std::endl;
  }

  void print_bucket_ranges(const std::string& expected, const bucket_list<int, std::string>& cont) {
    std::cout << "Expected ranges: " << expected << std::endl;
    print_actual_ranges(cont);
  }

  std::unique_ptr<container_type> container;
  container_type container_;
};

// Construction tests
TEST_F(BucketListTest, DefaultConstruction) {
  container_type test_container;
  EXPECT_FALSE(test_container.constrained());
  EXPECT_TRUE(test_container.empty());
  EXPECT_EQ(test_container.size(), 0);
}

TEST_F(BucketListTest, ConstrainedConstruction) {
  container_type test_container(0, 100);
  EXPECT_TRUE(test_container.constrained());
  EXPECT_TRUE(test_container.empty());
  EXPECT_EQ(test_container.size(), 0);
  EXPECT_EQ(test_container.low(), 0);
  EXPECT_EQ(test_container.high(), 100);
}

TEST_F(BucketListTest, InvalidConstrainedConstruction) {
  EXPECT_THROW(container_type test_container(100, 0), std::invalid_argument);
}

// Accessor tests
TEST_F(BucketListTest, AccessorFunctions) {
  container_type test_container;
  test_container.spread(0, 10, "test");

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

// Iterator tests
TEST_F(BucketListTest, IteratorFunctionality) {
  container_type test_container;
  test_container.spread(1, 3, "test1");
  test_container.spread(1, 3, "test2");
  test_container.spread(1, 3, "test3");

  auto it = test_container.begin();
  EXPECT_NE(it, test_container.end());
  EXPECT_EQ(it->low(), 1);
  EXPECT_EQ(it->high(), 3);
  const auto &bucket_values = *it;
  const auto &values = bucket_values.values();
  verifyContainerContents(values, {"test1", "test2", "test3"});
}

TEST_F(BucketListTest, ReverseIteratorFunctionality) {
  container_type test_container;
  test_container.spread(1, 3, "test1");
  test_container.spread(1, 3, "test2");
  test_container.spread(1, 3, "test3");

  auto rit = test_container.rbegin();
  EXPECT_NE(rit, test_container.rend());
  EXPECT_EQ(rit->low(), 1);
  EXPECT_EQ(rit->high(), 3);
  const auto &rbucket_values = *rit;
  const auto &rvalues = rbucket_values.values();
  verifyContainerContents(rvalues, {"test1", "test2", "test3"});
}

// Operation tests
TEST_F(BucketListTest, SpreadOperation) {
  container_type test_container;
  test_container.spread(0, 10, "test");
  EXPECT_EQ(test_container.size(), 1);
  EXPECT_FALSE(test_container.empty());

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketListTest, CoverOperation) {
  container_type test_container;
  test_container.cover(0, 10, "test");
  EXPECT_EQ(test_container.size(), 1);
  EXPECT_FALSE(test_container.empty());

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketListTest, EraseOperation) {
  container_type test_container;
  test_container.spread(0, 10, "test");
  test_container.erase(0, 10);
  EXPECT_TRUE(test_container.empty());
  EXPECT_EQ(test_container.size(), 0);
}

TEST_F(BucketListTest, EraseAllOperation) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  EXPECT_FALSE(test_container.empty());
  EXPECT_EQ(test_container.size(), 3);

  EXPECT_TRUE(test_container.erase());
  EXPECT_TRUE(test_container.empty());
  EXPECT_EQ(test_container.size(), 0);

  // Test erasing an already empty container
  EXPECT_FALSE(test_container.erase());
}

// Bounds tests
TEST_F(BucketListTest, UnconstrainedBoundOperations) {
  container_type test_container;
  test_container.spread(1, 3, "test1");
  test_container.spread(5, 7, "test2");
  test_container.spread(9, 11, "test3");

  EXPECT_THROW(
      { [[maybe_unused]] auto low = test_container.low(); }, std::runtime_error);
  EXPECT_THROW(
      { [[maybe_unused]] auto high = test_container.high(); }, std::runtime_error);
}

TEST_F(BucketListTest, ConstrainedBoundOperations) {
  container_type test_container(0, 100);
  test_container.spread(1, 3, "test1");
  test_container.spread(5, 7, "test2");
  test_container.spread(9, 11, "test3");

  EXPECT_EQ(test_container.low(), 0);
  EXPECT_EQ(test_container.high(), 100);
}

// Edge case tests
TEST_F(BucketListTest, OverlappingRangesSpread) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  // print_actual_ranges(test_container);

  test_container.spread(20, 30, "test2");
  // print_actual_ranges(test_container);

  test_container.spread(5, 25, "test3");
  // print_bucket_ranges("[0,5),[5,10),[10,20),[20,25),[25,30)", test_container);

  auto it = test_container.begin();
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
  container_type test_container;

  // Test overlapping ranges with gap between buckets
  test_container.cover(0, 10, "test1");
  test_container.cover(20, 30, "test2");
  test_container.cover(5, 25, "test3");

  EXPECT_EQ(test_container.size(), 3);

  auto it = test_container.begin();
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
  container_type test_container;

  // Test overlapping ranges with gap between buckets
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.erase(5, 25);

  EXPECT_EQ(test_container.size(), 2);

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 5);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 25);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
}

TEST_F(BucketListTest, OverlappingRangesWithConstraints) {
  container_type test_container(0, 100);

  // Test overlapping ranges with gap between buckets
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(5, 25, "test3");

  EXPECT_EQ(test_container.size(), 5);

  auto it = test_container.begin();
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

TEST_F(BucketListTest, OverlappingRangesWithHighConstraint) {
  container_type test_container(0, 100);

  // Test overlapping ranges with gap between buckets
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(5, 25, "test3");

  EXPECT_EQ(test_container.size(), 5);

  auto it = test_container.begin();
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

TEST_F(BucketListTest, OverlappingRangesWithLowConstraintIntersection) {
  container_type test_container(0, 100);

  // Test overlapping ranges with gap between buckets
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(5, 25, "test3");

  EXPECT_EQ(test_container.size(), 5);

  auto it = test_container.begin();
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

TEST_F(BucketListTest, OverlappingRangesWithHighConstraintIntersection) {
  container_type test_container(0, 100);

  // Test overlapping ranges with gap between buckets
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(5, 25, "test3");

  EXPECT_EQ(test_container.size(), 5);

  auto it = test_container.begin();
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

TEST_F(BucketListTest, ConstrainedRangeOperations) {
  container_type test_container(0, 100);

  // Test range operations with constraints
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  EXPECT_EQ(test_container.size(), 3);
  EXPECT_EQ(test_container.low(), 0);
  EXPECT_EQ(test_container.high(), 100);
}

TEST_F(BucketListTest, AutomaticOrdering) {
  container_type test_container;

  // Test automatic ordering of buckets
  test_container.spread(20, 30, "test2");
  test_container.spread(0, 10, "test1");
  test_container.spread(40, 50, "test3");

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});

  ++it;
  EXPECT_EQ(it->low(), 40);
  EXPECT_EQ(it->high(), 50);
  verifyContainerContents(it->values(), {"test3"});
}

TEST_F(BucketListTest, MultipleValuesInSameRange) {
  container_type test_container;

  // Test multiple values in the same range
  test_container.spread(0, 10, "test1");
  test_container.spread(0, 10, "test2");
  test_container.spread(0, 10, "test3");

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2", "test3"});
}

TEST_F(BucketListTest, DuplicateValuesInSameRange) {
  container_type test_container;

  // Test duplicate values in the same range
  test_container.spread(0, 10, "test");
  test_container.spread(0, 10, "test");
  test_container.spread(0, 10, "test");

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test", "test", "test"});
}

TEST_F(BucketListTest, ConstrainedFunction) {
  container_type test_container(0, 100);
  EXPECT_TRUE(test_container.constrained());
  EXPECT_EQ(test_container.low(), 0);
  EXPECT_EQ(test_container.high(), 100);
}

TEST_F(BucketListTest, LowerBoundFunction) {
  container_type test_container(0, 100);
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  EXPECT_EQ(test_container.lower_bound(), 0);
}

TEST_F(BucketListTest, UpperBoundFunction) {
  container_type test_container(0, 100);
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  EXPECT_EQ(test_container.upper_bound(), 100);
}

TEST_F(BucketListTest, SpreadWithBucketType) {
  container_type test_container;
  container_type::value_container values;
  bucket_value_traits<std::string, std::list<std::string>>::add(values, "test");
  container_type::bucket_type bucket(0, 10, values);
  test_container.spread(bucket);

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketListTest, CoverWithBucketType) {
  container_type test_container;
  container_type::value_container values;
  bucket_value_traits<std::string, std::list<std::string>>::add(values, "test");
  container_type::bucket_type bucket(0, 10, values);
  test_container.cover(bucket);

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketListTest, SpreadWithContainer) {
  container_type test_container1;
  test_container1.spread(0, 10, "test1");
  test_container1.spread(20, 30, "test2");

  container_type test_container2;
  test_container2.spread(5, 25, "test3");
  test_container2.spread(40, 50, "test4");

  test_container1.spread(test_container2);

  EXPECT_EQ(test_container1.size(), 6);

  auto it = test_container1.begin();
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

  ++it;
  EXPECT_EQ(it->low(), 40);
  EXPECT_EQ(it->high(), 50);
  verifyContainerContents(it->values(), {"test4"});
}

TEST_F(BucketListTest, CoverWithContainer) {
  container_type test_container1;
  test_container1.spread(0, 10, "test1");
  test_container1.spread(20, 30, "test2");

  container_type test_container2;
  test_container2.spread(5, 25, "test3");
  test_container2.spread(40, 50, "test4");

  test_container1.cover(test_container2);

  EXPECT_EQ(test_container1.size(), 4);

  auto it = test_container1.begin();
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

  ++it;
  EXPECT_EQ(it->low(), 40);
  EXPECT_EQ(it->high(), 50);
  verifyContainerContents(it->values(), {"test4"});
}

TEST_F(BucketListTest, IteratorOperations) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  // Test iterator operations
  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});

  --it;
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});

  ++it;
  ++it;
  EXPECT_EQ(it->low(), 40);
  EXPECT_EQ(it->high(), 50);
  verifyContainerContents(it->values(), {"test3"});

  --it;
  --it;
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});
}

TEST_F(BucketListTest, IteratorComparison) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  // Test iterator comparison
  auto it1 = test_container.begin();
  auto it2 = test_container.begin();
  EXPECT_EQ(it1, it2);

  ++it1;
  EXPECT_NE(it1, it2);

  ++it2;
  EXPECT_EQ(it1, it2);

  auto it3 = test_container.end();
  EXPECT_NE(it1, it3);
  EXPECT_NE(it2, it3);
}

TEST_F(BucketListTest, IteratorArithmetic) {
  container_type test_container;
  test_container.spread(1, 2, "test1");
  test_container.spread(2, 3, "test2");
  test_container.spread(3, 4, "test3");

  auto it = test_container.begin();

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

TEST_F(BucketListTest, FindEdgeCases) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");

  // Test finding values at exact bucket boundaries
  EXPECT_NO_THROW({  // Lower bound is inclusive
    auto it = test_container.find(0);
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 10);
    verifyContainerContents(it->values(), {"test1"});
  });
  EXPECT_THROW({[[maybe_unused]] auto it = test_container.find(10);}, std::out_of_range); // high boundary is exclusive
  
  EXPECT_NO_THROW({
    auto it = test_container.find(5);
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 10);
    verifyContainerContents(it->values(), {"test1"});
  });

  // Test finding in gaps
  EXPECT_THROW({[[maybe_unused]] auto it = test_container.find(15);}, std::out_of_range);

  // Test finding in empty container
  container_type empty_container;
  EXPECT_THROW({[[maybe_unused]] auto it = empty_container.find(5);}, std::out_of_range);
}

TEST_F(BucketListTest, FindInConstrainedBuckets) {
  container_type test_container(0, 100);
  test_container.spread(0, 10, "test1");
  test_container.spread(90, 100, "test2");

  // Test finding at constraint boundaries
  EXPECT_NO_THROW({  // Lower bound is inclusive
    auto it = test_container.find(0);
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 10);
    verifyContainerContents(it->values(), {"test1"});
  });
  EXPECT_THROW({[[maybe_unused]] auto it = test_container.find(100);}, std::out_of_range); // Upper bound is exclusive

  // Test finding within valid ranges
  EXPECT_NO_THROW({
    auto it = test_container.find(5);
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 10);
    verifyContainerContents(it->values(), {"test1"});
  });

  EXPECT_NO_THROW({
    auto it = test_container.find(95);
    EXPECT_EQ(it->low(), 90);
    EXPECT_EQ(it->high(), 100);
    verifyContainerContents(it->values(), {"test2"});
  });
}

TEST_F(BucketListTest, FindMethod) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  // Test finding existing values
  EXPECT_NO_THROW({  // Lower bound is inclusive
    auto it = test_container.find(0);
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 10);
    verifyContainerContents(it->values(), {"test1"});
  });

  EXPECT_NO_THROW({
    auto it = test_container.find(25);
    EXPECT_EQ(it->low(), 20);
    EXPECT_EQ(it->high(), 30);
    verifyContainerContents(it->values(), {"test2"});
  });

  // Test finding non-existent values
  EXPECT_THROW({[[maybe_unused]] auto it = test_container.find(15);}, std::out_of_range);
  EXPECT_THROW({[[maybe_unused]] auto it = test_container.find(35);}, std::out_of_range);
  EXPECT_THROW({[[maybe_unused]] auto it = test_container.find(55);}, std::out_of_range);

  // Test const version
  const container_type& const_container = test_container;
  EXPECT_NO_THROW({
    auto const_it = const_container.find(45);
    EXPECT_EQ(const_it->low(), 40);
    EXPECT_EQ(const_it->high(), 50);
    verifyContainerContents(const_it->values(), {"test3"});
  });
  EXPECT_THROW({[[maybe_unused]] auto const_it = const_container.find(15);}, std::out_of_range);
}

TEST_F(BucketListTest, FindMethodWithOverlappingRanges) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(5, 25, "test3");

  // Test finding values in the first non-overlapping segment [0,5)
  EXPECT_NO_THROW({
    auto it = test_container.find(2);
    EXPECT_EQ(it->low(), 0);
    EXPECT_EQ(it->high(), 5);
    verifyContainerContents(it->values(), {"test1"});
  });

  // Test finding values in the first overlapping segment [5,10)
  EXPECT_NO_THROW({
    auto it = test_container.find(7);
    EXPECT_EQ(it->low(), 5);
    EXPECT_EQ(it->high(), 10);
    verifyContainerContents(it->values(), {"test1", "test3"});
  });

  // Test finding values in the middle segment [10,20)
  EXPECT_NO_THROW({
    auto it = test_container.find(15);
    EXPECT_EQ(it->low(), 10);
    EXPECT_EQ(it->high(), 20);
    verifyContainerContents(it->values(), {"test3"});
  });

  // Test finding values in the second overlapping segment [20,25)
  EXPECT_NO_THROW({
    auto it = test_container.find(22);
    EXPECT_EQ(it->low(), 20);
    EXPECT_EQ(it->high(), 25);
    verifyContainerContents(it->values(), {"test2", "test3"});
  });

  // Test finding values in the last non-overlapping segment [25,30)
  EXPECT_NO_THROW({
    auto it = test_container.find(27);
    EXPECT_EQ(it->low(), 25);
    EXPECT_EQ(it->high(), 30);
    verifyContainerContents(it->values(), {"test2"});
  });

  // Test finding values in gaps between ranges
  EXPECT_THROW({[[maybe_unused]] auto it = test_container.find(35);}, std::out_of_range);
}

// Add new test case for std::set
TEST_F(BucketListTest, SetContainerType) {
  using set_container_type = bucket_list<int, std::string, bucket_compare_traits<int>, bucket_value_traits<std::string, std::set<std::string>>>;
  set_container_type test_container;
  set_container_type::value_container values;
  bucket_value_traits<std::string, std::set<std::string>>::add(values, "test1");
  bucket_value_traits<std::string, std::set<std::string>>::add(values, "test2");
  bucket_value_traits<std::string, std::set<std::string>>::add(values, "test1"); // Duplicate value should be ignored in set
  
  set_container_type::bucket_type bucket(0, 10, values);
  test_container.spread(bucket);

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1", "test2"});
}

TEST_F(BucketListTest, SpreadWithConstrainedBoundsOutOfRange) {
  container_type test_container(10, 40);
  
  // Test spreading range completely before constrained bounds
  test_container.spread(0, 5, "test1");
  EXPECT_TRUE(test_container.empty());

  // Test spreading range completely after constrained bounds
  test_container.spread(45, 50, "test2");
  EXPECT_TRUE(test_container.empty());

  // Test spreading range partially before constrained bounds
  test_container.spread(5, 15, "test3");
  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 10);
  EXPECT_EQ(it->high(), 15);
  verifyContainerContents(it->values(), {"test3"});

  // Test spreading range partially after constrained bounds
  test_container.spread(35, 45, "test4");
  it = test_container.begin();
  ++it;
  EXPECT_EQ(it->low(), 35);
  EXPECT_EQ(it->high(), 40);
  verifyContainerContents(it->values(), {"test4"});
}

TEST_F(BucketListTest, SpreadWithGapBeforeExistingBucket) {
  container_type test_container;
  test_container.spread(20, 30, "test1");
  
  // Test spreading with a gap before existing bucket
  test_container.spread(5, 15, "test2");
  
  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 5);
  EXPECT_EQ(it->high(), 15);
  verifyContainerContents(it->values(), {"test2"});
  
  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test1"});
}

TEST_F(BucketListTest, SpreadWithExactBucketSplit) {
  container_type test_container;
  test_container.spread(0, 30, "test1");
  
  // Test exact splitting points
  test_container.spread(10, 20, "test2");
  
  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});
  
  ++it;
  EXPECT_EQ(it->low(), 10);
  EXPECT_EQ(it->high(), 20);
  verifyContainerContents(it->values(), {"test1", "test2"});
  
  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test1"});
}

TEST_F(BucketListTest, SpreadWithGapAfterLastBucket) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  
  // Test spreading with a gap after the last bucket
  test_container.spread(20, 30, "test2");
  
  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});
  
  ++it;
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});
}

TEST_F(BucketListTest, SpreadWithEmptyContainer) {
  container_type test_container;
  
  // Test spreading into empty container
  test_container.spread(10, 20, "test1");
  
  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 10);
  EXPECT_EQ(it->high(), 20);
  verifyContainerContents(it->values(), {"test1"});
}

TEST_F(BucketListTest, SpreadWithInvalidRange) {
  container_type test_container;
  
  // Test spreading with high < low
  test_container.spread(20, 10, "test1");
  EXPECT_TRUE(test_container.empty());
  
  // Test spreading with equal bounds
  test_container.spread(10, 10, "test2");
  EXPECT_TRUE(test_container.empty());
}

TEST_F(BucketListTest, BucketRangeOperations) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  // Test range that includes multiple buckets
  auto range = test_container.range(5, 35);
  auto range_it = range.begin();
  EXPECT_EQ(range_it->low(), 0);
  EXPECT_EQ(range_it->high(), 10);
  verifyContainerContents(range_it->values(), {"test1"});

  ++range_it;
  EXPECT_EQ(range_it->low(), 20);
  EXPECT_EQ(range_it->high(), 30);
  verifyContainerContents(range_it->values(), {"test2"});

  ++range_it;
  EXPECT_EQ(range_it, range.end());

  // Test empty range between buckets
  auto empty_range = test_container.range(12, 18);
  EXPECT_EQ(empty_range.begin(), empty_range.end());

  // Test const range
  const container_type& const_container = test_container;
  auto const_range = const_container.range(15, 45);
  auto const_it = const_range.begin();
  EXPECT_EQ(const_it->low(), 20);
  EXPECT_EQ(const_it->high(), 30);
  verifyContainerContents(const_it->values(), {"test2"});
}

TEST_F(BucketListTest, CustomIndexType) {
  // Test with a custom index type (e.g., std::pair<int, int>)
  using custom_container_type = bucket_list<std::pair<int, int>, std::string>;
  custom_container_type test_container;

  std::pair<int, int> low(0, 0);
  std::pair<int, int> high(10, 10);
  test_container.spread(low, high, "test");

  auto it = test_container.find(std::pair<int, int>(5, 5));
  EXPECT_NE(it, test_container.end());
  EXPECT_EQ(it->low(), low);
  EXPECT_EQ(it->high(), high);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(BucketListTest, CustomValueContainer) {
  // Test with a custom value container (e.g., std::set)
  using set_container_type = bucket_list<int, std::string, 
    bucket_compare_traits<int>, 
    bucket_value_traits<std::string, std::set<std::string>>>;
  
  set_container_type test_container;
  
  // Sets should deduplicate values
  test_container.spread(0, 10, "test");
  test_container.spread(0, 10, "test");
  test_container.spread(0, 10, "different");

  auto it = test_container.begin();
  EXPECT_EQ(it->values().size(), 2); // Only unique values
  EXPECT_TRUE(it->values().contains("test"));
  EXPECT_TRUE(it->values().contains("different"));
}

TEST_F(BucketListTest, ContainsMethod) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");

  // Test existing values
  EXPECT_TRUE(test_container.contains(0));  // Lower bound is inclusive
  EXPECT_TRUE(test_container.contains(5));
  EXPECT_TRUE(test_container.contains(20)); // Lower bound is inclusive
  EXPECT_TRUE(test_container.contains(25));

  // Test non-existing values
  EXPECT_FALSE(test_container.contains(15));
  EXPECT_FALSE(test_container.contains(35));

  // Test boundary conditions
  EXPECT_TRUE(test_container.contains(0));   // Lower bound is inclusive
  EXPECT_FALSE(test_container.contains(10)); // Upper bound is exclusive
  EXPECT_TRUE(test_container.contains(20));  // Lower bound is inclusive
  EXPECT_FALSE(test_container.contains(30)); // Upper bound is exclusive

  // Test empty container
  container_type empty_container;
  EXPECT_FALSE(empty_container.contains(5));
}

TEST_F(BucketListTest, AtMethod) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");

  // Test accessing existing values
  EXPECT_NO_THROW({
    auto& values = test_container.at(5);
    verifyContainerContents(values, {"test1"});
  });

  EXPECT_NO_THROW({
    auto& values = test_container.at(25);
    verifyContainerContents(values, {"test2"});
  });

  // Test accessing non-existing values
  EXPECT_THROW({[[maybe_unused]] auto& values = test_container.at(15);}, std::out_of_range);
  EXPECT_THROW({[[maybe_unused]] auto& values = test_container.at(35);}, std::out_of_range);

  // Test boundary conditions
  EXPECT_NO_THROW({  // Lower bound is inclusive
    auto& values = test_container.at(0);
    verifyContainerContents(values, {"test1"});
  });
  EXPECT_THROW({[[maybe_unused]] auto& values = test_container.at(10);}, std::out_of_range); // Upper bound is exclusive
  EXPECT_NO_THROW({  // Lower bound is inclusive
    auto& values = test_container.at(20);
    verifyContainerContents(values, {"test2"});
  });
  EXPECT_THROW({[[maybe_unused]] auto& values = test_container.at(30);}, std::out_of_range); // Upper bound is exclusive

  // Test empty container
  container_type empty_container;
  EXPECT_THROW({[[maybe_unused]] auto& values = empty_container.at(5);}, std::out_of_range);

  // Test const version
  const container_type& const_container = test_container;
  EXPECT_NO_THROW({
    const auto& values = const_container.at(5);
    verifyContainerContents(values, {"test1"});
  });
  EXPECT_THROW({[[maybe_unused]] const auto& values = const_container.at(15);}, std::out_of_range);
}

TEST_F(BucketListTest, NextMethod) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  // Test finding current bucket
  auto it = test_container.next(5);
  EXPECT_NE(it, test_container.end());
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test1"});

  // Test finding next bucket
  it = test_container.next(15);
  EXPECT_NE(it, test_container.end());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});

  // Test when index is beyond all buckets
  it = test_container.next(55);
  EXPECT_EQ(it, test_container.end());

  // Test const version
  const container_type& const_container = test_container;
  auto const_it = const_container.next(25);
  EXPECT_NE(const_it, const_container.end());
  EXPECT_EQ(const_it->low(), 20);
  EXPECT_EQ(const_it->high(), 30);
  verifyContainerContents(const_it->values(), {"test2"});
}

TEST_F(BucketListTest, PreviousMethod) {
  container_type test_container;
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  // Test finding current bucket
  auto it = test_container.previous(25);
  EXPECT_NE(it, test_container.end());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});

  // Test finding previous bucket
  it = test_container.previous(35);
  EXPECT_NE(it, test_container.end());
  EXPECT_EQ(it->low(), 20);
  EXPECT_EQ(it->high(), 30);
  verifyContainerContents(it->values(), {"test2"});

  // Test when index is before all buckets
  it = test_container.previous(-5);
  EXPECT_EQ(it, test_container.end());

  // Test when index is beyond all buckets
  it = test_container.previous(55);
  EXPECT_NE(it, test_container.end());
  EXPECT_EQ(it->low(), 40);
  EXPECT_EQ(it->high(), 50);
  verifyContainerContents(it->values(), {"test3"});

  // Test const version
  const container_type& const_container = test_container;
  auto const_it = const_container.previous(45);
  EXPECT_NE(const_it, const_container.end());
  EXPECT_EQ(const_it->low(), 40);
  EXPECT_EQ(const_it->high(), 50);
  verifyContainerContents(const_it->values(), {"test3"});
}

} // namespace test
} // namespace masutils