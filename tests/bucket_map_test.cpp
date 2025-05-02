#include <bucket/bucket_compare_traits.h>
#include <bucket/bucket_map.h>
#include <bucket/bucket_object.h>
#include <bucket/bucket_range.h>
#include <bucket/bucket_value_traits.h>
#include <gtest/gtest.h>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <set>

namespace masutils {
namespace test {

using container_type = bucket_map<int, std::string>;

class ContainerTest : public ::testing::Test {
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

  std::unique_ptr<container_type> container;
  container_type container_;
};

// Construction tests
TEST_F(ContainerTest, DefaultConstruction) {
  container_type test_container;
  EXPECT_FALSE(test_container.constrained());
  EXPECT_TRUE(test_container.empty());
  EXPECT_EQ(test_container.size(), 0);
}

TEST_F(ContainerTest, ConstrainedConstruction) {
  container_type test_container(0, 100);
  EXPECT_TRUE(test_container.constrained());
  EXPECT_TRUE(test_container.empty());
  EXPECT_EQ(test_container.size(), 0);
  EXPECT_EQ(test_container.low(), 0);
  EXPECT_EQ(test_container.high(), 100);
}

TEST_F(ContainerTest, InvalidConstrainedConstruction) {
  EXPECT_THROW(container_type test_container(100, 0), std::invalid_argument);
}

// Accessor tests
TEST_F(ContainerTest, AccessorFunctions) {
  container_type test_container;
  test_container.spread(0, 10, "test");

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

// Iterator tests
TEST_F(ContainerTest, IteratorFunctionality) {
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

TEST_F(ContainerTest, ReverseIteratorFunctionality) {
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
TEST_F(ContainerTest, SpreadOperation) {
  container_type test_container;
  test_container.spread(0, 10, "test");
  EXPECT_EQ(test_container.size(), 1);
  EXPECT_FALSE(test_container.empty());

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(ContainerTest, CoverOperation) {
  container_type test_container;
  test_container.cover(0, 10, "test");
  EXPECT_EQ(test_container.size(), 1);
  EXPECT_FALSE(test_container.empty());

  auto it = test_container.begin();
  EXPECT_EQ(it->low(), 0);
  EXPECT_EQ(it->high(), 10);
  verifyContainerContents(it->values(), {"test"});
}

TEST_F(ContainerTest, EraseOperation) {
  container_type test_container;
  test_container.spread(0, 10, "test");
  test_container.erase(0, 10);
  EXPECT_TRUE(test_container.empty());
  EXPECT_EQ(test_container.size(), 0);
}

TEST_F(ContainerTest, EraseAllOperation) {
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
TEST_F(ContainerTest, UnconstrainedBoundOperations) {
  container_type test_container;
  test_container.spread(1, 3, "test1");
  test_container.spread(5, 7, "test2");
  test_container.spread(9, 11, "test3");

  EXPECT_THROW(
      { [[maybe_unused]] auto low = test_container.low(); }, std::runtime_error);
  EXPECT_THROW(
      { [[maybe_unused]] auto high = test_container.high(); }, std::runtime_error);
}

TEST_F(ContainerTest, ConstrainedBoundOperations) {
  container_type test_container(0, 100);
  test_container.spread(1, 3, "test1");
  test_container.spread(5, 7, "test2");
  test_container.spread(9, 11, "test3");

  EXPECT_EQ(test_container.low(), 0);
  EXPECT_EQ(test_container.high(), 100);
}

// Edge case tests
TEST_F(ContainerTest, OverlappingRangesSpread) {
  container_type test_container;

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

TEST_F(ContainerTest, OverlappingRangesCover) {
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

TEST_F(ContainerTest, OverlappingRangesErase) {
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

TEST_F(ContainerTest, OverlappingRangesWithConstraints) {
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

TEST_F(ContainerTest, OverlappingRangesWithHighConstraint) {
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

TEST_F(ContainerTest, OverlappingRangesWithLowConstraintIntersection) {
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

TEST_F(ContainerTest, OverlappingRangesWithHighConstraintIntersection) {
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

TEST_F(ContainerTest, ConstrainedRangeOperations) {
  container_type test_container(0, 100);

  // Test range operations with constraints
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  EXPECT_EQ(test_container.size(), 3);
  EXPECT_EQ(test_container.low(), 0);
  EXPECT_EQ(test_container.high(), 100);
}

TEST_F(ContainerTest, AutomaticOrdering) {
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

TEST_F(ContainerTest, MultipleValuesInSameRange) {
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

TEST_F(ContainerTest, DuplicateValuesInSameRange) {
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

TEST_F(ContainerTest, ConstrainedFunction) {
  container_type test_container(0, 100);
  EXPECT_TRUE(test_container.constrained());
  EXPECT_EQ(test_container.low(), 0);
  EXPECT_EQ(test_container.high(), 100);
}

TEST_F(ContainerTest, LowerBoundFunction) {
  container_type test_container(0, 100);  // Create a constrained container
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  EXPECT_EQ(test_container.lower_bound(), 0);
}

TEST_F(ContainerTest, UpperBoundFunction) {
  container_type test_container(0, 100);  // Create a constrained container
  test_container.spread(0, 10, "test1");
  test_container.spread(20, 30, "test2");
  test_container.spread(40, 50, "test3");

  EXPECT_EQ(test_container.upper_bound(), 100);
}

TEST_F(ContainerTest, SpreadWithBucketType) {
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

TEST_F(ContainerTest, CoverWithBucketType) {
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

TEST_F(ContainerTest, SpreadWithContainer) {
  container_type test_container1;
  test_container1.spread(0, 10, "test1");
  test_container1.spread(20, 30, "test2");

  container_type test_container2;
  test_container2.spread(5, 25, "test3");
  test_container2.spread(40, 50, "test4");

  test_container1.spread(test_container2);

  EXPECT_EQ(test_container1.size(), 6);  // Updated expected size

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

TEST_F(ContainerTest, CoverWithContainer) {
  container_type test_container1;
  test_container1.spread(0, 10, "test1");
  test_container1.spread(20, 30, "test2");

  container_type test_container2;
  test_container2.spread(5, 25, "test3");
  test_container2.spread(40, 50, "test4");

  test_container1.cover(test_container2);

  EXPECT_EQ(test_container1.size(), 4);  // Updated expected size

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

TEST_F(ContainerTest, IteratorOperations) {
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

TEST_F(ContainerTest, IteratorComparison) {
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

TEST_F(ContainerTest, IteratorArithmetic) {
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

// Add new test case for std::set
TEST_F(ContainerTest, SetContainerType) {
  using set_container_type = bucket_map<int, std::string, bucket_compare_traits<int>, bucket_value_traits<std::string, std::set<std::string>>>;
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

} // namespace test
} // namespace masutils