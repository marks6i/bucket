#include "pch.h"

#include "../include/buckets.h"
#include "../include/buckets_supp.h"

TEST(TestCaseName, TestName) {
  EXPECT_EQ(1, 1);
  EXPECT_TRUE(true);
}

////#define _GENERIC_PRINTING
//#define _TEST1
//#define _TEST2
//#define _TEST3
//#define _TEST4
//#define _TEST5
//#define _TEST6
//#define _TEST7
//#define _TEST8
//#define _TEST9
//#define _TEST10
//#define _TEST11
//
//using namespace std;
//using namespace masutils;
//using namespace masxtra;
//
//typedef buckets<int, char*> CharBucket;
//typedef buckets<time_t, char*> WorkBucket;
//
//typedef buckets<int, char*, bucket_traits_descending<int>                                         > DescendCharBucket;
//typedef buckets<int, char*, bucket_traits_descending<int>                                         > DescendCharBucket;
//typedef buckets<int, char*, bucket_traits<int>, most_recent_bucket_value_traits<char*> > MostRecentBucket;
//typedef buckets<int, int, bucket_traits<int>, bucket_value_add_traits<int>           > AddIntBucket;
//typedef buckets<int, double, bucket_traits<int>, bucket_value_add_traits<double>        > AddDoubleBucket;
//
//// Glossary only works because I've done a "plus 1" to the char value to create the range. This would
//// work if the char whose bucket we were trying to create was equal to maximum char value.
//template <class _V>
//struct Glossary
//	: public buckets<char, _V> {
//
//	typedef _V value_type;
//
//	// should have been able to just use buckets(...) in constructors below instead of buckets<char, _V>(...)
//	// this appears to be a bug in older versions of GCC
//	Glossary() : buckets<char, _V>() {};
//	Glossary(char low, char high) : buckets<char, _V>(toupper(low), toupper(high) + 1) {};
//
//	void add(value_type word) { this->spread(toupper(word[0]), toupper(word[0]) + 1, word); }
//};
//
//template<class _E>
//void printBucket(_E& bucket)
//{
//	typedef _E value_type;
//
//	cout << "----template<class _E> void printBucket(_E& bucket)" << endl;
//
//	for (typename value_type::iterator p = bucket.begin();
//		p != bucket.end();
//		++p)
//	{
//		typename value_type::triplet_type& triplet = *p;
//		cout << "start: " << triplet.first
//			<< ", end: " << triplet.second
//			<< ", value: (";
//
//		for (typename value_type::value_container::iterator b = triplet.third.begin();
//			b != triplet.third.end();
//			++b)
//		{
//			typename value_type::value_type val = (*b);
//			cout << val << ", ";
//		}
//
//		cout << ")" << endl;
//	}
//}
//
//#ifndef _GENERIC_PRINTING
//template <class _V>
//void printBucket(Glossary<_V>& bucket)
//{
//	typedef Glossary<_V> value_type;
//
//	cout << "----void printBucket(Glossary& bucket)" << endl;
//
//	for (typename value_type::iterator p = bucket.begin();
//		p != bucket.end();
//		++p)
//	{
//		typename value_type::triplet_type& triplet = *p;
//		cout << "starting with: '" << triplet.first << "'"
//			<< ", value: (";
//
//		for (typename value_type::value_container::iterator b = triplet.third.begin();
//			b != triplet.third.end();
//			++b)
//		{
//			typename value_type::value_type val = (*b);
//			cout << val << ", ";
//		}
//
//		cout << ")" << endl;
//	}
//}
//#endif // _GENERIC_PRINTING
//
//
//#ifndef _GENERIC_PRINTING
//void printBucket(WorkBucket& bucket)
//{
//	typedef WorkBucket value_type;
//
//	cout << "----void printBucket(WorkBucket& bucket)" << endl;
//
//	for (value_type::iterator p = bucket.begin();
//		p != bucket.end();
//		++p)
//	{
//		value_type::triplet_type& triplet = *p;
//
//		// ctime has thread-safety and/or re-entrancy issues.  Created the masxtra::ctime function
//		// to hopefully address those issues.
//		cout << "start:  " << masxtra::ctime(triplet.first) << endl;
//		cout << "end:    " << masxtra::ctime(triplet.second) << endl;
//
//		cout << "values: ";
//
//		for (value_type::value_container::iterator b = triplet.third.begin();
//			b != triplet.third.end();
//			++b)
//		{
//			char* val = (*b);
//			cout << val << endl;
//			cout << "        ";
//		}
//
//		cout << endl;
//	}
//}
//#endif // _GENERIC_PRINTING
//
//
//inline time_t make_time(int year, int month, int day, int hour, int minute, int second)
//{
//	struct tm date = { second, minute, hour, day, month - 1, year - 1900 };
//	return mktime(&date);
//}
//
//// Initialize words list
////   I would have liked to use list initialization but that is a C++0x
////   feature.  This works with VC++ 9.0+ (VS 2008) or GCC 4.4+.  For example
////   in VS 2013 I could have used:
////
////   const std::vector<std::string> words = { "str1", "str2", .... }
////
////   The endN template is a trick so that I don't have to calculate the
////   size of the words_init array and modify the declaration of words
////   below.
//
//template<typename _T, size_t _N>  _T* endN(_T(&ra)[_N]) { return ra + _N; }
//
//const char* words_init[] = {
//	"January",
//	"February",
//	"March",
//	"April",
//	"May",
//	"June",
//	"July",
//	"August",
//	"September",
//	"October",
//	"November",
//	"December",
//	"orange",
//	"pineapple",
//	"mango",
//	"apple",
//	"apricot",
//	"banana"
//};
//
//std::vector<std::string> words(words_init, endN(words_init));
//std::vector<const char*> words_charstar(words_init, endN(words_init));
//
//// ======================= begin tests ================================
//
//void test1() {
//#ifdef _TEST1
//	CharBucket bucket(18, 72); // constrained bucket
//
//	cout << "============ test1 ============" << endl;
//
//	bucket.spread(9, 10, "9-10");
//	printBucket(bucket);
//	bucket.spread(10, 20, "10-20");
//	printBucket(bucket);
//	bucket.spread(30, 40, "30-40");
//	printBucket(bucket);
//	bucket.spread(30, 31, "30-31");
//	printBucket(bucket);
//	bucket.spread(50, 60, "50-60");
//	printBucket(bucket);
//	bucket.spread(59, 60, "59-60");
//	printBucket(bucket);
//	bucket.spread(70, 80, "70-80");
//	printBucket(bucket);
//	bucket.spread(80, 81, "80-81");
//	printBucket(bucket);
//	bucket.spread(15, 75, "15-75");
//	printBucket(bucket);
//#endif // #ifdef _TEST1
//}
//
//void test2() {
//#ifdef _TEST2
//	MostRecentBucket bucket; // unconstrained bucket
//
//	cout << "============ test2 ============" << endl;
//
//	bucket.spread(9, 10, "9-10");
//	//		printBucket(bucket);
//	bucket.spread(10, 20, "10-20");
//	//		printBucket(bucket);
//	bucket.spread(30, 40, "30-40");
//	//		printBucket(bucket);
//	bucket.spread(30, 31, "30-31");
//	//		printBucket(bucket);
//	bucket.spread(50, 60, "50-60");
//	//		printBucket(bucket);
//	bucket.spread(59, 60, "59-60");
//	//		printBucket(bucket);
//	bucket.spread(70, 80, "70-80");
//	//		printBucket(bucket);
//	bucket.spread(80, 81, "80-81");
//	//		printBucket(bucket);
//	bucket.spread(15, 75, "15-75");
//	printBucket(bucket);
//#endif // #ifdef _TEST2
//
//}
//
//void test3() {
//#ifdef _TEST3
//	DescendCharBucket bucket(72, 18); // constrained bucket
//
//	cout << "============ test3 ============" << endl;
//
//	bucket.spread(10, 9, "9-10");
//	//		printBucket(bucket);
//	bucket.spread(20, 10, "10-20");
//	//		printBucket(bucket);
//	bucket.spread(40, 30, "30-40");
//	//		printBucket(bucket);
//	bucket.spread(31, 30, "30-31");
//	//		printBucket(bucket);
//	bucket.spread(60, 50, "50-60");
//	//		printBucket(bucket);
//	bucket.spread(60, 59, "59-60");
//	//		printBucket(bucket);
//	bucket.spread(80, 70, "70-80");
//	//		printBucket(bucket);
//	bucket.spread(81, 80, "80-81");
//	//		printBucket(bucket);
//	bucket.spread(75, 15, "15-75");
//	printBucket(bucket);
//#endif // #ifdef _TEST3
//}
//
//void test4() {
//#ifdef _TEST4
//	AddIntBucket bucket(0, 100);
//
//	cout << "============ test4 ============" << endl;
//
//	bucket.spread(10, 100, 10);
//
//	//		printBucket(bucket);
//	bucket.spread(20, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(30, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(40, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(50, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(60, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(70, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(80, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(90, 100, 10);
//	printBucket(bucket);
//#endif // #ifdef _TEST4
//}
//
//void test5() {
//#ifdef _TEST5
//	AddDoubleBucket bucket(0, 100);
//
//	cout << "============ test5 ============" << endl;
//
//	bucket.spread(10, 100, 10.25);
//
//	//		printBucket(bucket);
//	bucket.spread(20, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(30, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(40, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(50, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(60, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(70, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(80, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(90, 100, 10);
//	//		printBucket(bucket);
//	bucket.spread(15, 95, 5);
//	printBucket(bucket);
//#endif // #ifdef _TEST5
//}
//
//void test6() {
//#ifdef _TEST6
//	WorkBucket bucket; // unconstrained bucket
//
//	cout << "============ test6 ============" << endl;
//
//	for (int day = 13; day <= 17; day++)
//	{
//		bucket.spread(
//			make_time(2003, 11, day, 9, 0, 0),
//			make_time(2003, 11, day, 17, 0, 0),
//			"Open"
//		);
//	}
//	bucket.spread(
//		make_time(2003, 11, 17, 12, 0, 0),
//		make_time(2003, 11, 17, 13, 0, 0),
//		"Lunch"
//	);
//
//	printBucket(bucket);
//#endif // #ifdef _TEST6
//}
//
//void test7() {
//#ifdef _TEST7
//	WorkBucket bucket; // unconstrained bucket
//
//	cout << "============ test7 ============" << endl;
//
//	for (int day = 13; day <= 17; day++)
//	{
//		bucket.spread(
//			make_time(2003, 11, day, 9, 0, 0),
//			make_time(2003, 11, day, 17, 0, 0),
//			"Open"
//		);
//	}
//	bucket.cover(
//		make_time(2003, 11, 17, 12, 0, 0),
//		make_time(2003, 11, 17, 13, 0, 0),
//		"Lunch"
//	);
//
//	printBucket(bucket);
//#endif // #ifdef _TEST7
//}
//
//void test8() {
//#ifdef _TEST8
//	WorkBucket bucket; // unconstrained bucket
//
//	cout << "============ test8 ============" << endl;
//
//	for (int day = 13; day <= 17; day++)
//	{
//		bucket.spread(
//			make_time(2003, 11, day, 9, 0, 0),
//			make_time(2003, 11, day, 17, 0, 0),
//			"Open"
//		);
//	}
//	bucket.cover(
//		make_time(2003, 11, 17, 12, 0, 0),
//		make_time(2003, 11, 17, 13, 0, 0),
//		"Lunch"
//	);
//
//	bucket.cover(
//		make_time(2003, 11, 17, 12, 30, 0),
//		make_time(2003, 11, 17, 13, 30, 0),
//		"Lunch2"
//	);
//
//	bucket.cover(
//		make_time(2003, 11, 17, 12, 15, 0),
//		make_time(2003, 11, 17, 13, 15, 0),
//		"Lunch3"
//	);
//
//	printBucket(bucket);
//#endif // #ifdef _TEST8
//}
//
//void test9() {
//#ifdef _TEST9
//	Glossary<std::string> bucket; // unconstrained bucket
//
//	cout << "============ test9 ============" << endl;
//
//	for (std::vector<std::string>::iterator it = words.begin(); it != words.end(); ++it) {
//		bucket.add(*it);
//	}
//
//	printBucket(bucket);
//#endif // #ifdef _TEST9
//}
//
//void test10() {
//#ifdef _TEST10
//	Glossary<std::string> bucket('M', 'P'); // constrained bucket
//
//	cout << "============ test10 ============" << endl;
//
//	for (std::vector<std::string>::iterator it = words.begin(); it != words.end(); ++it) {
//		bucket.add(*it);
//	}
//
//	printBucket(bucket);
//#endif // #ifdef _TEST10
//}
//
//void test11() {
//#ifdef _TEST11
//	Glossary<const char*> bucket; // unconstrained bucket
//
//	cout << "============ test11 ============" << endl;
//
//	for (std::vector<const char*>::iterator it = words_charstar.begin(); it != words_charstar.end(); ++it) {
//		bucket.add(*it);
//	}
//
//	printBucket(bucket);
//#endif // #ifdef _TEST11
//}
//
//int main()
//{
//	test1();
//	test2();
//	test3();
//	test4();
//	test5();
//	test6();
//	test7();
//	test8();
//	test9();
//	test10();
//	test11();
//
//	return 0;
//}
