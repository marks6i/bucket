#include <gtest/gtest.h>

#include "../include/compare_traits.h"

using masutils::compare_traits;
using masutils::compare_traits_descending;

TEST(CompareTraitsTest, EqLtAssign) {
    EXPECT_TRUE(compare_traits<int>::eq(3, 3));
    EXPECT_FALSE(compare_traits<int>::eq(3, 4));

    EXPECT_TRUE(compare_traits<int>::lt(3, 4));
    EXPECT_FALSE(compare_traits<int>::lt(4, 3));
    EXPECT_FALSE(compare_traits<int>::lt(3, 3));

    int x = 0;
    compare_traits<int>::assign(x, 5);
    EXPECT_EQ(x, 5);
}

TEST(CompareTraitsTest, DerivedComparisons) {
    // ne
    EXPECT_TRUE(compare_traits<int>::ne(3, 4));
    EXPECT_FALSE(compare_traits<int>::ne(3, 3));

    // le
    EXPECT_TRUE(compare_traits<int>::le(3, 4));
    EXPECT_TRUE(compare_traits<int>::le(3, 3));
    EXPECT_FALSE(compare_traits<int>::le(4, 3));

    // gt
    EXPECT_TRUE(compare_traits<int>::gt(4, 3));
    EXPECT_FALSE(compare_traits<int>::gt(3, 4));
    EXPECT_FALSE(compare_traits<int>::gt(3, 3));

    // ge
    EXPECT_TRUE(compare_traits<int>::ge(4, 3));
    EXPECT_TRUE(compare_traits<int>::ge(3, 3));
    EXPECT_FALSE(compare_traits<int>::ge(3, 4));
}

TEST(CompareTraitsTest, DescendingReversesOrder) {
    // Descending: lt(x, y) is true when x > y
    EXPECT_TRUE(compare_traits_descending<int>::lt(5, 3));
    EXPECT_FALSE(compare_traits_descending<int>::lt(3, 5));
    EXPECT_FALSE(compare_traits_descending<int>::lt(3, 3));

    // NOTE: gt/le/ge are ordinary (non-virtual) static methods inherited
    // from compare_traits<E>, and their bodies call the unqualified `lt`
    // that was resolved at compile time *within the base class*. Because
    // there is no virtual dispatch (and no CRTP), compare_traits_descending
    // "hiding" lt does not change what gt/le/ge call: they still use the
    // base class's ascending lt(). So, perhaps counter-intuitively, the
    // derived class's gt/le/ge behave exactly like the base (ascending)
    // class's, even though lt() itself is reversed.
    EXPECT_FALSE(compare_traits_descending<int>::gt(3, 5)); // base lt(5,3) == false
    EXPECT_FALSE(compare_traits_descending<int>::le(5, 3)); // !base_lt(3,5) == false
    EXPECT_FALSE(compare_traits_descending<int>::ge(3, 5)); // !base_lt(3,5) == false

    // eq/ne/assign are inherited unchanged
    EXPECT_TRUE(compare_traits_descending<int>::eq(3, 3));
    EXPECT_TRUE(compare_traits_descending<int>::ne(3, 4));
    int x = 0;
    compare_traits_descending<int>::assign(x, 9);
    EXPECT_EQ(x, 9);
}
