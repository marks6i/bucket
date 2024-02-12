#include "pch.h"

#include "../include/buckets.h"
#include "../include/buckets_supp.h"

#include "../include/test/support.h"

using namespace std;
using namespace masutils;
using namespace mastest;

using CharBucket = buckets<int,    char*>;
using WorkBucket = buckets<time_t, char*>;

using DescendCharBucket = buckets<int, char*, bucket_traits_descending<int>>;
using MostRecentBucket  = buckets<int, char*, bucket_traits<int>, most_recent_bucket_value_traits<char*>>;
using AddIntBucket      = buckets<int, int, bucket_traits<int>, bucket_value_add_traits<int>>;
using AddDoubleBucket   = buckets<int, double, bucket_traits<int>, bucket_value_add_traits<double>>;

#if BUCKET_USING_GTEST_
// Converting this to Google Test

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

#else // BUCKET_USING_GTEST_

//#define _GENERIC_PRINTING
#define _TEST1
#define _TEST2
#define _TEST3
#define _TEST4
#define _TEST5
#define _TEST6
#define _TEST7
#define _TEST8
#define _TEST9
#define _TEST10
#define _TEST11
#define _TEST12


template<class _E>
void printBucket(_E& bucket)
{
	using value_type = _E;

	cout << "----template<class _E> void printBucket(_E& bucket)" << endl;

	for (typename value_type::iterator p = bucket.begin();
		p != bucket.end();
		++p)
	{
		typename value_type::triplet_type& triplet = *p;
		cout << "start: " << triplet.first
			<< ", end: " << triplet.second
			<< ", value: (";

		for (typename value_type::value_container::iterator b = triplet.third.begin();
			b != triplet.third.end();
			++b)
		{
			typename value_type::value_type val = (*b);
			cout << val << ", ";
		}

		cout << ")" << endl;
	}
}

#ifndef _GENERIC_PRINTING
template <class _VT, class _V>
void printBucket(Glossary<_VT, _V>& bucket)
{
	using value_type = Glossary<_VT, _V>;

	cout << "----void printBucket(Glossary& bucket)" << endl;

	for (typename value_type::iterator p = bucket.begin();
		p != bucket.end();
		++p)
	{
		typename value_type::triplet_type& triplet = *p;
		cout << "starting with: '" << triplet.first << "'"
			<< ", value: (";

		for (typename value_type::value_container::iterator b = triplet.third.begin();
			b != triplet.third.end();
			++b)
		{
			typename value_type::value_type val = (*b);
			cout << val << ", ";
		}

		cout << ")" << endl;
	}
}
#endif // _GENERIC_PRINTING


#ifndef _GENERIC_PRINTING
void printBucket(WorkBucket& bucket)
{
	using value_type = WorkBucket;

	cout << "----void printBucket(WorkBucket& bucket)" << endl;

	for (auto p = bucket.begin();
		p != bucket.end();
		++p)
	{
		value_type::triplet_type& triplet = *p;

		cout << "start:  " << time_to_string(triplet.first) << endl;
		cout << "end:    " << time_to_string(triplet.second) << endl;

		cout << "values: ";

		for (auto b = triplet.third.begin();
			b != triplet.third.end();
			++b)
		{
			char const* val = (*b);
			cout << val << endl;
			cout << "        ";
		}

		cout << endl;
	}
}
#endif // _GENERIC_PRINTING

const std::vector<char *> words = {
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December",
	"orange",
	"pineapple",
	"mango",
	"apricot",
	"apple",
	"banana",
	"aPricot"
};

// ======================= begin tests ================================

void test1() {
#ifdef _TEST1
	CharBucket bucket(18, 72); // constrained bucket

	cout << "============ test1 ============" << endl;

	bucket.spread(9, 10, "9-10");
	printBucket(bucket);
	bucket.spread(10, 20, "10-20");
	printBucket(bucket);
	bucket.spread(30, 40, "30-40");
	printBucket(bucket);
	bucket.spread(30, 31, "30-31");
	printBucket(bucket);
	bucket.spread(50, 60, "50-60");
	printBucket(bucket);
	bucket.spread(59, 60, "59-60");
	printBucket(bucket);
	bucket.spread(70, 80, "70-80");
	printBucket(bucket);
	bucket.spread(80, 81, "80-81");
	printBucket(bucket);
	bucket.spread(15, 75, "15-75");
	printBucket(bucket);
#endif // #ifdef _TEST1
}

void test2() {
#ifdef _TEST2
	MostRecentBucket bucket; // unconstrained bucket

	cout << "============ test2 ============" << endl;

	bucket.spread(9, 10, "9-10");
	bucket.spread(10, 20, "10-20");
	bucket.spread(30, 40, "30-40");
	bucket.spread(30, 31, "30-31");
	bucket.spread(50, 60, "50-60");
	bucket.spread(59, 60, "59-60");
	bucket.spread(70, 80, "70-80");
	bucket.spread(80, 81, "80-81");
	bucket.spread(15, 75, "15-75");
	printBucket(bucket);
#endif // #ifdef _TEST2

}

void test3() {
#ifdef _TEST3
	DescendCharBucket bucket(72, 18); // constrained bucket

	cout << "============ test3 ============" << endl;

	bucket.spread(10, 9, "9-10");
	bucket.spread(20, 10, "10-20");
	bucket.spread(40, 30, "30-40");
	bucket.spread(31, 30, "30-31");
	bucket.spread(60, 50, "50-60");
	bucket.spread(60, 59, "59-60");
	bucket.spread(80, 70, "70-80");
	bucket.spread(81, 80, "80-81");
	bucket.spread(75, 15, "15-75");
	printBucket(bucket);
#endif // #ifdef _TEST3
}

void test4() {
#ifdef _TEST4
	AddIntBucket bucket(0, 100);

	cout << "============ test4 ============" << endl;

	bucket.spread(10, 100, 10);

	bucket.spread(20, 100, 10);
	bucket.spread(30, 100, 10);
	bucket.spread(40, 100, 10);
	bucket.spread(50, 100, 10);
	bucket.spread(60, 100, 10);
	bucket.spread(70, 100, 10);
	bucket.spread(80, 100, 10);
	bucket.spread(90, 100, 10);
	printBucket(bucket);
#endif // #ifdef _TEST4
}

void test5() {
#ifdef _TEST5
	AddDoubleBucket bucket(0, 100);

	cout << "============ test5 ============" << endl;

	bucket.spread(10, 100, 10.25);

	bucket.spread(20, 100, 10);
	bucket.spread(30, 100, 10);
	bucket.spread(40, 100, 10);
	bucket.spread(50, 100, 10);
	bucket.spread(60, 100, 10);
	bucket.spread(70, 100, 10);
	bucket.spread(80, 100, 10);
	bucket.spread(90, 100, 10);
	bucket.spread(15, 95, 5);
	printBucket(bucket);
#endif // #ifdef _TEST5
}

void test6() {
#ifdef _TEST6
	WorkBucket bucket; // unconstrained bucket

	cout << "============ test6 ============" << endl;

	for (int day = 13; day <= 17; day++)
	{
		bucket.spread(
			make_time(2003, 11, day, 9, 0, 0),
			make_time(2003, 11, day, 17, 0, 0),
			"Open"
		);
	}
	bucket.spread(
		make_time(2003, 11, 17, 12, 0, 0),
		make_time(2003, 11, 17, 13, 0, 0),
		"Lunch"
	);

	printBucket(bucket);
#endif // #ifdef _TEST6
}

void test7() {
#ifdef _TEST7
	WorkBucket bucket; // unconstrained bucket

	cout << "============ test7 ============" << endl;

	for (int day = 13; day <= 17; day++)
	{
		bucket.spread(
			make_time(2003, 11, day, 9, 0, 0),
			make_time(2003, 11, day, 17, 0, 0),
			"Open"
		);
	}
	bucket.cover(
		make_time(2003, 11, 17, 12, 0, 0),
		make_time(2003, 11, 17, 13, 0, 0),
		"Lunch"
	);

	printBucket(bucket);
#endif // #ifdef _TEST7
}

void test8() {
#ifdef _TEST8
	WorkBucket bucket; // unconstrained bucket

	cout << "============ test8 ============" << endl;

	for (int day = 13; day <= 17; day++)
	{
		bucket.spread(
			make_time(2003, 11, day, 9, 0, 0),
			make_time(2003, 11, day, 17, 0, 0),
			"Open"
		);
	}
	bucket.cover(
		make_time(2003, 11, 17, 12, 0, 0),
		make_time(2003, 11, 17, 13, 0, 0),
		"Lunch"
	);

	bucket.cover(
		make_time(2003, 11, 17, 12, 30, 0),
		make_time(2003, 11, 17, 13, 30, 0),
		"Lunch2"
	);

	bucket.cover(
		make_time(2003, 11, 17, 12, 15, 0),
		make_time(2003, 11, 17, 13, 15, 0),
		"Lunch3"
	);

	printBucket(bucket);
#endif // #ifdef _TEST8
}

void test9() {
#ifdef _TEST9
	Glossary<char> bucket; // unconstrained bucket

	cout << "============ test9 ============" << endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	printBucket(bucket);
#endif // #ifdef _TEST9
}

void test10() {
#ifdef _TEST10
	Glossary<char> bucket('M', 'P'); // constrained bucket

	cout << "============ test10 ============" << endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	printBucket(bucket);
#endif // #ifdef _TEST10
}

void test11() {
#ifdef _TEST11
	Glossary<char, basic_string<char>, caseInsensitiveLess<basic_string<char>>> bucket; // unconstrained bucket

	cout << "============ test11 ============" << endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	printBucket(bucket);
#endif // #ifdef _TEST11
}

void test12() {
#ifdef _TEST12
	Glossary<char, char*, caseInsensitiveLess<basic_string<char>>> bucket; // unconstrained bucket

	cout << "============ test12 ============" << endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	printBucket(bucket);
#endif // #ifdef _TEST12
}

int main()
{
	cout << "start timestamp: " << time_stamp() << endl << endl;

	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test7();
	test8();
	test9();
	test10();
	test11();
	test12();

	cout << endl << "end timestamp: " << time_stamp() << endl;

	return 0;
}

#endif // BUCKET_USING_GTEST_
