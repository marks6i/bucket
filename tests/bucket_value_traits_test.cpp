#include <algorithm>
#include <bucket/detail/bucket_value_traits.h>
#include <gtest/gtest.h>
#include <list>
#include <set>
#include <string>
#include <vector>

using namespace masutils;

namespace masutils {
namespace test {

// Test for default container (std::list)
TEST(BucketValueTraitsTest, DefaultContainer) {
  using traits = bucket_value_traits<std::string>;
  typename traits::container_type container;

  // Test add
  traits::add(container, "test1");
  traits::add(container, "test2");
  traits::add(container, "test3");

  // Verify container contents
  EXPECT_EQ(container.size(), 3);
  auto it = container.begin();
  EXPECT_EQ(*it++, "test1");
  EXPECT_EQ(*it++, "test2");
  EXPECT_EQ(*it, "test3");

  // Test remove
  EXPECT_TRUE(traits::remove(container, "test2"));
  EXPECT_EQ(container.size(), 2);
  it = container.begin();
  EXPECT_EQ(*it++, "test1");
  EXPECT_EQ(*it, "test3");
  EXPECT_FALSE(traits::remove(container, "nonexistent"));

  // Test append
  typename traits::container_type other;
  other.push_back("test4");
  other.push_back("test5");
  traits::append(container, other);
  EXPECT_EQ(container.size(), 4);
  it = container.begin();
  EXPECT_EQ(*it++, "test1");
  EXPECT_EQ(*it++, "test3");
  EXPECT_EQ(*it++, "test4");
  EXPECT_EQ(*it, "test5");
}

} // namespace test
} // namespace masutils

// Test set-based container
template <>
struct masutils::bucket_value_traits<std::string, std::set<std::string>> {
  using container_type = std::set<std::string>;

  static void add(container_type &container, const std::string &value) {
    container.insert(value);
  }

  static bool remove(container_type &container, const std::string &value) {
    return container.erase(value) > 0;
  }

  template <class other_value_container>
  static void append(container_type &container,
                     const other_value_container &other) {
    container.insert(other.begin(), other.end());
  }

protected:
  ~bucket_value_traits() = default;
};

namespace masutils {
namespace test {

TEST(BucketValueTraitsTest, SetContainer) {
  using traits = bucket_value_traits<std::string, std::set<std::string>>;
  typename traits::container_type container;

  // Test add
  traits::add(container, "test1");
  traits::add(container, "test2");
  traits::add(container, "test3");

  // Verify container contents
  EXPECT_EQ(container.size(), 3);
  EXPECT_TRUE(container.find("test1") != container.end());
  EXPECT_TRUE(container.find("test2") != container.end());
  EXPECT_TRUE(container.find("test3") != container.end());

  // Test remove
  EXPECT_TRUE(traits::remove(container, "test2"));
  EXPECT_EQ(container.size(), 2);
  EXPECT_TRUE(container.find("test1") != container.end());
  EXPECT_TRUE(container.find("test2") == container.end());
  EXPECT_TRUE(container.find("test3") != container.end());
  EXPECT_FALSE(traits::remove(container, "nonexistent"));

  // Test append
  std::set<std::string> other{"test4", "test5"};
  traits::append(container, other);
  EXPECT_EQ(container.size(), 4);
  EXPECT_TRUE(container.find("test4") != container.end());
  EXPECT_TRUE(container.find("test5") != container.end());
}

} // namespace test
} // namespace masutils

// Test custom type with custom container
struct custom_value {
  int id;
  std::string name;

  bool operator==(const custom_value &other) const {
    return id == other.id && name == other.name;
  }
};

template <> struct masutils::bucket_value_traits<custom_value> {
  using container_type = std::list<custom_value>;

  static void add(container_type &container, const custom_value &value) {
    container.push_back(value);
  }

  static bool remove(container_type &container, const custom_value &value) {
    for (auto p = container.begin(); p != container.end(); ++p) {
      if (*p == value) {
        container.erase(p);
        return true;
      }
    }
    return false;
  }

  template <class other_value_container>
  static void append(container_type &container,
                     const other_value_container &other) {
    container.insert(container.end(), other.begin(), other.end());
  }

protected:
  ~bucket_value_traits() = default;
};

namespace masutils {
namespace test {

TEST(BucketValueTraitsTest, CustomType) {
  using traits = bucket_value_traits<custom_value>;
  typename traits::container_type container;

  custom_value v1{1, "test1"};
  custom_value v2{2, "test2"};
  custom_value v3{3, "test3"};

  // Test add
  traits::add(container, v1);
  traits::add(container, v2);
  traits::add(container, v3);

  // Verify container contents
  EXPECT_EQ(container.size(), 3);
  auto it = container.begin();
  EXPECT_EQ(it->id, 1);
  EXPECT_EQ(it->name, "test1");
  ++it;
  EXPECT_EQ(it->id, 2);
  EXPECT_EQ(it->name, "test2");
  ++it;
  EXPECT_EQ(it->id, 3);
  EXPECT_EQ(it->name, "test3");

  // Test remove
  EXPECT_TRUE(traits::remove(container, v2));
  EXPECT_EQ(container.size(), 2);
  it = container.begin();
  EXPECT_EQ(it->id, 1);
  EXPECT_EQ(it->name, "test1");
  ++it;
  EXPECT_EQ(it->id, 3);
  EXPECT_EQ(it->name, "test3");
  EXPECT_FALSE(traits::remove(container, custom_value{4, "test4"}));

  // Test append
  typename traits::container_type other;
  other.push_back(custom_value{4, "test4"});
  other.push_back(custom_value{5, "test5"});
  traits::append(container, other);
  EXPECT_EQ(container.size(), 4);
  it = container.begin();
  EXPECT_EQ(it->id, 1);
  EXPECT_EQ(it->name, "test1");
  ++it;
  EXPECT_EQ(it->id, 3);
  EXPECT_EQ(it->name, "test3");
  ++it;
  EXPECT_EQ(it->id, 4);
  EXPECT_EQ(it->name, "test4");
  ++it;
  EXPECT_EQ(it->id, 5);
  EXPECT_EQ(it->name, "test5");
}

#if __cplusplus >= 202002L
TEST(BucketValueTraitsTest, MoveSemantics) {
  using traits = bucket_value_traits<std::string>;
  typename traits::container_type container;

  // Test move add
  std::string str = "test1";
  traits::add(container, std::move(str));
  EXPECT_EQ(container.size(), 1);
  EXPECT_EQ(container.front(), "test1");
  EXPECT_TRUE(str.empty());

  // Test move append
  typename traits::container_type other;
  other.push_back("test2");
  other.push_back("test3");
  traits::append(container, std::move(other));
  EXPECT_EQ(container.size(), 3);
  auto it = container.begin();
  EXPECT_EQ(*it++, "test1");
  EXPECT_EQ(*it++, "test2");
  EXPECT_EQ(*it, "test3");
  EXPECT_TRUE(other.empty());
}
#endif // __cplusplus >= 202002L

} // namespace test
} // namespace masutils