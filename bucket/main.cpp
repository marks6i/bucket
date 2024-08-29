#include "pch.h"

#include "../include/buckets.h"
#include "../include/bucket_list.h"
#include "../include/buckets_supp.h"
#include "../include/app/main_support.h"

using namespace masutils;
using namespace masxtra;

using char_bucket = buckets<int,    const char*>;
using char_bucket_list = bucket_list<int, const char*>;
using work_bucket = buckets<time_t, const char*>;

using descend_char_bucket = buckets<int, const char*,  bucket_compare_traits_descending<int>>;
using most_recent_bucket  = buckets<int, const char*,  bucket_compare_traits<int>, most_recent_bucket_value_traits<const char*>>;
using add_int_bucket      = buckets<int, int,    bucket_compare_traits<int>, bucket_value_add_traits<int>>;
using add_double_bucket   = buckets<int, double, bucket_compare_traits<int>, bucket_value_add_traits<double>>;

const std::vector<const char *> words = {
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

void example1() {
	char_bucket bucket(18, 72); // constrained bucket

	std::cout << "============ example1 ============" << std::endl;

	bucket.spread( 9, 10,  "9-10");
	bucket.spread(10, 20, "10-20");
	bucket.spread(30, 40, "30-40");
	bucket.spread(30, 31, "30-31");
	bucket.spread(50, 60, "50-60");
	bucket.spread(59, 60, "59-60");
	bucket.spread(70, 80, "70-80");
	bucket.spread(80, 81, "80-81");
	bucket.spread(15, 75, "15-75");


	std::cout << "bucket is:\n" << bucket_wrapper<char_bucket>(bucket) << std::endl;
}

void example2() {
	most_recent_bucket bucket; // unconstrained bucket

	std::cout << "============ example2 ============" << std::endl;

	bucket.spread(9, 10, "9-10");
	bucket.spread(10, 20, "10-20");
	bucket.spread(30, 40, "30-40");
	bucket.spread(30, 31, "30-31");
	bucket.spread(50, 60, "50-60");
	bucket.spread(59, 60, "59-60");
	bucket.spread(70, 80, "70-80");
	bucket.spread(80, 81, "80-81");
	bucket.spread(15, 75, "15-75");

	std::cout << "bucket is:\n" << bucket_wrapper<most_recent_bucket>(bucket) << std::endl;
}

void example3() {
	descend_char_bucket bucket(72, 18); // constrained bucket

	std::cout << "============ example3 ============" << std::endl;

	bucket.spread(10, 9, "9-10");
	bucket.spread(20, 10, "10-20");
	bucket.spread(40, 30, "30-40");
	bucket.spread(31, 30, "30-31");
	bucket.spread(60, 50, "50-60");
	bucket.spread(60, 59, "59-60");
	bucket.spread(80, 70, "70-80");
	bucket.spread(81, 80, "80-81");
	bucket.spread(75, 15, "15-75");

	std::cout << "bucket is:\n" << bucket_wrapper<descend_char_bucket>(bucket) << std::endl;
}

void example4() {
	add_int_bucket bucket(0, 100);

	std::cout << "============ example4 ============" << std::endl;

	bucket.spread(10, 100, 10);

	bucket.spread(20, 100, 10);
	bucket.spread(30, 100, 10);
	bucket.spread(40, 100, 10);
	bucket.spread(50, 100, 10);
	bucket.spread(60, 100, 10);
	bucket.spread(70, 100, 10);
	bucket.spread(80, 100, 10);
	bucket.spread(90, 100, 10);

	std::cout << "bucket is:\n" << bucket_wrapper<add_int_bucket>(bucket) << std::endl;
}

void example5() {
	add_double_bucket bucket(0, 100);

	std::cout << "============ example5 ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<add_double_bucket>(bucket) << std::endl;
}

void example6() {
	work_bucket bucket; // unconstrained bucket

	std::cout << "============ example6 ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket>(bucket) << std::endl;
}

void example7() {
	work_bucket bucket; // unconstrained bucket

	std::cout << "============ example7 ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket>(bucket) << std::endl;
}

void example8() {
	work_bucket bucket; // unconstrained bucket

	std::cout << "============ example8 ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket>(bucket) << std::endl;
}

void example9() {
	glossary<char> bucket; // unconstrained bucket

	std::cout << "============ example9 ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary<char>>(bucket) << std::endl;
}

void example10() {
	glossary<char> bucket('M', 'P'); // constrained bucket

	std::cout << "============ example10 ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary<char>>(bucket) << std::endl;
}

void example11() {
	glossary<char, std::basic_string<char>, caseInsensitiveLess<std::basic_string<char>>> bucket; // unconstrained bucket

	std::cout << "============ example11 ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary<char, std::basic_string<char>, caseInsensitiveLess<std::basic_string<char>>>>(bucket) << std::endl;
}

void example12() {
	glossary<char, const char*, caseInsensitiveLess<std::basic_string<char>>> bucket; // unconstrained bucket

	std::cout << "============ example12 ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary<char, const char*, caseInsensitiveLess<std::basic_string<char>>>>(bucket) << std::endl;
}

void example13() {
	char_bucket_list bucket(18, 72); // constrained bucket

	std::cout << "============ example1 ============" << std::endl;

	bucket.spread(9, 10, "9-10");
	bucket.spread(10, 20, "10-20");
	bucket.spread(30, 40, "30-40");
	bucket.spread(30, 31, "30-31");
	bucket.spread(50, 60, "50-60");
	bucket.spread(59, 60, "59-60");
	bucket.spread(70, 80, "70-80");
	bucket.spread(80, 81, "80-81");
	bucket.spread(15, 75, "15-75");


	std::cout << "bucket is:\n" << bucket_wrapper<char_bucket_list>(bucket) << std::endl;
}

int main()
{
	std::cout << "start timestamp: " << time_stamp() << std::endl << std::endl;

	example1();
	example2();
	example3();
	example4();
	example5();
	example6();
	example7();
	example8();
	example9();
	example10();
	example11();
	example12();
	example13();

	std::cout << "\nend timestamp: " << time_stamp() << std::endl;

	return 0;
}
