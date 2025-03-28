#include <gtest/gtest.h>
#include <bucket/bucket_value_traits.h>
#include <string>
#include <set>
#include <list>
#include <algorithm>

using namespace masutils;

// Test default container type (list)
TEST(BucketValueTraitsTest, DefaultContainer) {
    using traits = bucket_value_traits<std::string>;
    typename traits::container_type container;

    // Test add
    traits::add(container, "test1");
    traits::add(container, "test2");
    traits::add(container, "test3");

    // Test contains
    EXPECT_TRUE(traits::contains(container, "test1"));
    EXPECT_TRUE(traits::contains(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test3"));
    EXPECT_FALSE(traits::contains(container, "test4"));

    // Test remove
    traits::remove(container, "test2");
    EXPECT_TRUE(traits::contains(container, "test1"));
    EXPECT_FALSE(traits::contains(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test3"));
}

// Test set-based container
template<>
struct bucket_value_traits<std::string> {
    using container_type = std::set<std::string>;
    
    static void add(container_type& container, const std::string& value) {
        container.insert(value);
    }
    
    static void remove(container_type& container, const std::string& value) {
        container.erase(value);
    }
    
    static bool contains(const container_type& container, const std::string& value) {
        return container.find(value) != container.end();
    }
};

TEST(BucketValueTraitsTest, SetContainer) {
    using traits = bucket_value_traits<std::string, std::set<std::string>>;
    typename traits::container_type container;

    // Test add
    traits::add(container, "test1");
    traits::add(container, "test2");
    traits::add(container, "test3");

    // Test contains
    EXPECT_TRUE(traits::contains(container, "test1"));
    EXPECT_TRUE(traits::contains(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test3"));
    EXPECT_FALSE(traits::contains(container, "test4"));

    // Test remove
    traits::remove(container, "test2");
    EXPECT_TRUE(traits::contains(container, "test1"));
    EXPECT_FALSE(traits::contains(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test3"));
}

// Test custom type with custom container
struct custom_value {
    int id;
    std::string name;

    bool operator==(const custom_value& other) const {
        return id == other.id && name == other.name;
    }
};

namespace masutils {
template<>
struct bucket_value_traits<custom_value> {
    using container_type = std::list<custom_value>;

    static void add(container_type& container, const custom_value& value) {
        container.push_back(value);
    }

    static bool contains(const container_type& container, const custom_value& value) {
        return std::find(container.begin(), container.end(), value) != container.end();
    }

    static void remove(container_type& container, const custom_value& value) {
        container.remove(value);
    }
};
}

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

    // Test contains
    EXPECT_TRUE(traits::contains(container, v1));
    EXPECT_TRUE(traits::contains(container, v2));
    EXPECT_TRUE(traits::contains(container, v3));
    EXPECT_FALSE(traits::contains(container, custom_value{4, "test4"}));

    // Test remove
    traits::remove(container, v2);
    EXPECT_TRUE(traits::contains(container, v1));
    EXPECT_FALSE(traits::contains(container, v2));
    EXPECT_TRUE(traits::contains(container, v3));
} 