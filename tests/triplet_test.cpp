#include <gtest/gtest.h>

#include "../include/triplet.h"

#include <string>

using masutils::triplet;
using masutils::make_triplet;

TEST(TripletTest, DefaultConstruction) {
    triplet<int, double, std::string> t;
    EXPECT_EQ(t.first, 0);
    EXPECT_EQ(t.second, 0.0);
    EXPECT_EQ(t.third, "");
}

TEST(TripletTest, ValueConstruction) {
    triplet<int, double, std::string> t(1, 2.5, "hello");
    EXPECT_EQ(t.first, 1);
    EXPECT_EQ(t.second, 2.5);
    EXPECT_EQ(t.third, "hello");
}

TEST(TripletTest, CopyConstruction) {
    triplet<int, double, std::string> t1(1, 2.5, "hello");
    triplet<int, double, std::string> t2(t1);
    EXPECT_EQ(t2.first, 1);
    EXPECT_EQ(t2.second, 2.5);
    EXPECT_EQ(t2.third, "hello");
}

TEST(TripletTest, CopyAssignment) {
    triplet<int, double, std::string> t1(1, 2.5, "hello");
    triplet<int, double, std::string> t2;
    t2 = t1;
    EXPECT_EQ(t2.first, 1);
    EXPECT_EQ(t2.second, 2.5);
    EXPECT_EQ(t2.third, "hello");
}

TEST(TripletTest, MoveConstruction) {
    triplet<int, double, std::string> t1(1, 2.5, "hello");
    triplet<int, double, std::string> t2(std::move(t1));
    EXPECT_EQ(t2.first, 1);
    EXPECT_EQ(t2.second, 2.5);
    EXPECT_EQ(t2.third, "hello");
}

TEST(TripletTest, MoveAssignment) {
    triplet<int, double, std::string> t1(1, 2.5, "hello");
    triplet<int, double, std::string> t2;
    t2 = std::move(t1);
    EXPECT_EQ(t2.first, 1);
    EXPECT_EQ(t2.second, 2.5);
    EXPECT_EQ(t2.third, "hello");
}

TEST(TripletTest, MakeTriplet) {
    auto t = make_triplet(3, 4.5, std::string("world"));
    EXPECT_EQ(t.first, 3);
    EXPECT_EQ(t.second, 4.5);
    EXPECT_EQ(t.third, "world");
}
