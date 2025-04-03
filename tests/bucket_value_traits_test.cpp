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
    EXPECT_TRUE(traits::remove(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test1"));
    EXPECT_FALSE(traits::contains(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test3"));
    EXPECT_FALSE(traits::remove(container, "nonexistent"));

    // Test append
    typename traits::container_type other;
    other.push_back("test4");
    other.push_back("test5");
    traits::append(container, other);
    EXPECT_TRUE(traits::contains(container, "test4"));
    EXPECT_TRUE(traits::contains(container, "test5"));
}

// Test set-based container
namespace masutils {
template<>
struct bucket_value_traits<std::string, std::set<std::string> > {
    using container_type = std::set<std::string>;
    
    static void add(container_type& container, const std::string& value) {
        container.insert(value);
    }
    
    static bool remove(container_type& container, const std::string& value) {
        return container.erase(value) > 0;
    }
    
    static bool contains(const container_type& container, const std::string& value) {
        return container.find(value) != container.end();
    }

    template <class other_value_container>
    static void append(container_type& container, const other_value_container& other) {
        container.insert(other.begin(), other.end());
    }

protected:
    ~bucket_value_traits() = default;
};
} // namespace masutils

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
    EXPECT_TRUE(traits::remove(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test1"));
    EXPECT_FALSE(traits::contains(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test3"));
    EXPECT_FALSE(traits::remove(container, "nonexistent"));

    // Test append
    std::set<std::string> other{"test4", "test5"};
    traits::append(container, other);
    EXPECT_TRUE(traits::contains(container, "test4"));
    EXPECT_TRUE(traits::contains(container, "test5"));
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

    static bool remove(container_type& container, const custom_value& value) {
        for (auto p = container.begin(); p != container.end(); ++p) {
            if (*p == value) {
                container.erase(p);
                return true;
            }
        }
        return false;
    }

    template <class other_value_container>
    static void append(container_type& container, const other_value_container& other) {
        container.insert(container.end(), other.begin(), other.end());
    }

protected:
    ~bucket_value_traits() = default;
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
    EXPECT_TRUE(traits::remove(container, v2));
    EXPECT_TRUE(traits::contains(container, v1));
    EXPECT_FALSE(traits::contains(container, v2));
    EXPECT_TRUE(traits::contains(container, v3));
    EXPECT_FALSE(traits::remove(container, custom_value{4, "test4"}));

    // Test append
    typename traits::container_type other;
    other.push_back(custom_value{4, "test4"});
    other.push_back(custom_value{5, "test5"});
    traits::append(container, other);
    EXPECT_TRUE(traits::contains(container, custom_value{4, "test4"}));
    EXPECT_TRUE(traits::contains(container, custom_value{5, "test5"}));
}

#if __cplusplus >= 202002L
TEST(BucketValueTraitsTest, MoveSemantics) {
    using traits = bucket_value_traits<std::string>;
    typename traits::container_type container;

    // Test move add
    std::string str = "test1";
    traits::add(container, std::move(str));
    EXPECT_TRUE(traits::contains(container, "test1"));
    EXPECT_TRUE(str.empty());

    // Test move append
    typename traits::container_type other;
    other.push_back("test2");
    other.push_back("test3");
    traits::append(container, std::move(other));
    EXPECT_TRUE(traits::contains(container, "test2"));
    EXPECT_TRUE(traits::contains(container, "test3"));
    EXPECT_TRUE(other.empty());
}
#endif // __cplusplus >= 202002L 