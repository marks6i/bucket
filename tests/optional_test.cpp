#include <gtest/gtest.h>
#include <ostream>

#include "../include/optional.h"

#include <sstream>
#include <string>

using masutils::optional;

TEST(OptionalTest, DefaultConstruction) {
    optional<int> o;
    EXPECT_FALSE(static_cast<bool>(o));
    EXPECT_EQ(o.value(), 0); // default_value{} for int is 0
}

TEST(OptionalTest, ValueConstruction) {
    optional<int> o(42);
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_EQ(o.value(), 42);
}

TEST(OptionalTest, CopyConstruction) {
    optional<int> o1(7);
    optional<int> o2(o1);
    EXPECT_TRUE(static_cast<bool>(o2));
    EXPECT_EQ(o2.value(), 7);
}

TEST(OptionalTest, CopyAssignmentFromOptional) {
    optional<int> o1(7);
    optional<int> o2;
    o2 = o1;
    EXPECT_TRUE(static_cast<bool>(o2));
    EXPECT_EQ(o2.value(), 7);
}

TEST(OptionalTest, AssignmentFromValue) {
    optional<int> o;
    o = 99;
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_EQ(o.value(), 99);
}

TEST(OptionalTest, StreamOutputWhenSet) {
    optional<int> o(5);
    std::ostringstream oss;
    oss << o;
    EXPECT_EQ(oss.str(), "5");
}

TEST(OptionalTest, StreamOutputWhenUnset) {
    optional<int> o;
    std::ostringstream oss;
    oss << o;
    EXPECT_EQ(oss.str(), "<?>");
}

TEST(OptionalTest, StringValueType) {
    optional<std::string> o(std::string("hello"));
    EXPECT_TRUE(static_cast<bool>(o));
    EXPECT_EQ(o.value(), "hello");

    optional<std::string> empty;
    EXPECT_FALSE(static_cast<bool>(empty));
    EXPECT_EQ(empty.value(), "");
}
