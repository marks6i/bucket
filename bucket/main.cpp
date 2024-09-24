#include "pch.h"

#include "../include/buckets.h"
#include "../include/bucket_list.h"
#include "../include/bucket_map.h"
#include "../include/buckets_supp.h"
#include "../include/app/main_support.h"

using namespace masutils;
using namespace masxtra;

using char_bucket = buckets<int,    const char*>;
using char_bucket_list = bucket_list<int, const char*>;
using char_bucket_map = bucket_map<int, const char*>;

using work_bucket = buckets<time_t, const char*>;
using work_bucket_list = bucket_list<time_t, const char*>;
using work_bucket_map = bucket_map<time_t, const char*>;

using descend_char_bucket = buckets<int, const char*,  bucket_compare_traits_descending<int>>;
using descend_char_bucket_list = bucket_list<int, const char*, bucket_compare_traits_descending<int>>;
using descend_char_bucket_map = bucket_map<int, const char*, bucket_compare_traits_descending<int>>;
using most_recent_bucket = buckets<int, const char*, bucket_compare_traits<int>, most_recent_bucket_value_traits<const char*>>;
using most_recent_bucket_list = bucket_list<int, const char*, bucket_compare_traits<int>, most_recent_bucket_value_traits<const char*>>;
using most_recent_bucket_map = bucket_map<int, const char*, bucket_compare_traits<int>, most_recent_bucket_value_traits<const char*>>;
using add_int_bucket = buckets<int, int, bucket_compare_traits<int>, bucket_value_add_traits<int>>;
using add_int_bucket_list = bucket_list<int, int, bucket_compare_traits<int>, bucket_value_add_traits<int>>;
using add_int_bucket_map = bucket_map<int, int, bucket_compare_traits<int>, bucket_value_add_traits<int>>;
using add_double_bucket = buckets<int, double, bucket_compare_traits<int>, bucket_value_add_traits<double>>;
using add_double_bucket_list = bucket_list<int, double, bucket_compare_traits<int>, bucket_value_add_traits<double>>;
using add_double_bucket_map = bucket_map<int, double, bucket_compare_traits<int>, bucket_value_add_traits<double>>;

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

void example1_legacy() {
	char_bucket bucket(18, 72); // constrained bucket

	std::cout << "============ example1 legacy ============" << std::endl;

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

void example1_list() {
	char_bucket_list bucket(18, 72); // constrained bucket

	std::cout << "============ example1 list ============" << std::endl;

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

void example1_map() {
	char_bucket_map bucket(18, 72); // constrained bucket

	std::cout << "============ example1 map ============" << std::endl;

	bucket.spread(9, 10, "9-10");
	bucket.spread(10, 20, "10-20");
	bucket.spread(30, 40, "30-40");
	bucket.spread(30, 31, "30-31");
	bucket.spread(50, 60, "50-60");
	bucket.spread(59, 60, "59-60");
	bucket.spread(70, 80, "70-80");
	bucket.spread(80, 81, "80-81");
	bucket.spread(15, 75, "15-75");

	std::cout << "bucket is:\n" << bucket_wrapper<char_bucket_map>(bucket) << std::endl;
}

void example2_legacy() {
	most_recent_bucket bucket; // unconstrained bucket

	std::cout << "============ example2 legacy ============" << std::endl;

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

void example2_list() {
	most_recent_bucket_list bucket; // unconstrained bucket

	std::cout << "============ example2 list ============" << std::endl;

	bucket.spread(9, 10, "9-10");
	bucket.spread(10, 20, "10-20");
	bucket.spread(30, 40, "30-40");
	bucket.spread(30, 31, "30-31");
	bucket.spread(50, 60, "50-60");
	bucket.spread(59, 60, "59-60");
	bucket.spread(70, 80, "70-80");
	bucket.spread(80, 81, "80-81");
	bucket.spread(15, 75, "15-75");

	std::cout << "bucket is:\n" << bucket_wrapper<most_recent_bucket_list>(bucket) << std::endl;
}

void example2_map() {
	most_recent_bucket_map bucket; // unconstrained bucket

	std::cout << "============ example2 map ============" << std::endl;

	bucket.spread(9, 10, "9-10");
	bucket.spread(10, 20, "10-20");
	bucket.spread(30, 40, "30-40");
	bucket.spread(30, 31, "30-31");
	bucket.spread(50, 60, "50-60");
	bucket.spread(59, 60, "59-60");
	bucket.spread(70, 80, "70-80");
	bucket.spread(80, 81, "80-81");
	bucket.spread(15, 75, "15-75");

	std::cout << "bucket is:\n" << bucket_wrapper<most_recent_bucket_map>(bucket) << std::endl;
}

void example3_legacy() {
	descend_char_bucket bucket(72, 18); // constrained bucket

	std::cout << "============ example3 legacy ============" << std::endl;

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

void example3_list() {
	descend_char_bucket_list bucket(72, 18); // constrained bucket

	std::cout << "============ example3 list ============" << std::endl;

	bucket.spread(10, 9, "9-10");
	bucket.spread(20, 10, "10-20");
	bucket.spread(40, 30, "30-40");
	bucket.spread(31, 30, "30-31");
	bucket.spread(60, 50, "50-60");
	bucket.spread(60, 59, "59-60");
	bucket.spread(80, 70, "70-80");
	bucket.spread(81, 80, "80-81");
	bucket.spread(75, 15, "15-75");

	std::cout << "bucket is:\n" << bucket_wrapper<descend_char_bucket_list>(bucket) << std::endl;
}

void example3_map() {
	descend_char_bucket_map bucket(72, 18); // constrained bucket

	std::cout << "============ example3 map ============" << std::endl;

	bucket.spread(10, 9, "9-10");
	bucket.spread(20, 10, "10-20");
	bucket.spread(40, 30, "30-40");
	bucket.spread(31, 30, "30-31");
	bucket.spread(60, 50, "50-60");
	bucket.spread(60, 59, "59-60");
	bucket.spread(80, 70, "70-80");
	bucket.spread(81, 80, "80-81");
	bucket.spread(75, 15, "15-75");

	std::cout << "bucket is:\n" << bucket_wrapper<descend_char_bucket_map>(bucket) << std::endl;
}

void example4_legacy() {
	add_int_bucket bucket(0, 100);

	std::cout << "============ example4 legacy ============" << std::endl;

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

void example4_list() {
	add_int_bucket_list bucket(0, 100);

	std::cout << "============ example4 list ============" << std::endl;

	bucket.spread(10, 100, 10);

	bucket.spread(20, 100, 10);
	bucket.spread(30, 100, 10);
	bucket.spread(40, 100, 10);
	bucket.spread(50, 100, 10);
	bucket.spread(60, 100, 10);
	bucket.spread(70, 100, 10);
	bucket.spread(80, 100, 10);
	bucket.spread(90, 100, 10);

	std::cout << "bucket is:\n" << bucket_wrapper<add_int_bucket_list>(bucket) << std::endl;
}

void example4_map() {
	add_int_bucket_map bucket(0, 100);

	std::cout << "============ example4 map ============" << std::endl;

	bucket.spread(10, 100, 10);

	bucket.spread(20, 100, 10);
	bucket.spread(30, 100, 10);
	bucket.spread(40, 100, 10);
	bucket.spread(50, 100, 10);
	bucket.spread(60, 100, 10);
	bucket.spread(70, 100, 10);
	bucket.spread(80, 100, 10);
	bucket.spread(90, 100, 10);

	std::cout << "bucket is:\n" << bucket_wrapper<add_int_bucket_map>(bucket) << std::endl;
}

void example5_legacy() {
	add_double_bucket bucket(0, 100);

	std::cout << "============ example5 legacy ============" << std::endl;

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

void example5_list() {
	add_double_bucket_list bucket(0, 100);

	std::cout << "============ example5 list ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<add_double_bucket_list>(bucket) << std::endl;
}

void example5_map() {
	add_double_bucket_map bucket(0, 100);

	std::cout << "============ example5 map ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<add_double_bucket_map>(bucket) << std::endl;
}

void example6_legacy() {
	work_bucket bucket; // unconstrained bucket

	std::cout << "============ example6 legacy ============" << std::endl;

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

void example6_list() {
	work_bucket_list bucket; // unconstrained bucket

	std::cout << "============ example6 list ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket_list>(bucket) << std::endl;
}

void example6_map() {
	work_bucket_map bucket; // unconstrained bucket

	std::cout << "============ example6 map ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket_map>(bucket) << std::endl;
}

void example6a_legacy() {
	work_bucket bucket; // unconstrained bucket

	std::cout << "============ example6a legacy ============" << std::endl;

	bucket.spread(
		make_time(2003, 11, 17, 9, 0, 0),
		make_time(2003, 11, 17, 17, 0, 0),
		"Open"
	);

	bucket.spread(
		make_time(2003, 11, 17, 12, 0, 0),
		make_time(2003, 11, 17, 13, 0, 0),
		"Lunch"
	);

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket>(bucket) << std::endl;
}

void example6a_list() {
	work_bucket_list bucket; // unconstrained bucket

	std::cout << "============ example6a list ============" << std::endl;

	bucket.spread(
		make_time(2003, 11, 17, 9, 0, 0),
		make_time(2003, 11, 17, 17, 0, 0),
		"Open"
	);

	bucket.spread(
		make_time(2003, 11, 17, 12, 0, 0),
		make_time(2003, 11, 17, 13, 0, 0),
		"Lunch"
	);

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket_list>(bucket) << std::endl;
}

void example6a_map() {
	work_bucket_map bucket; // unconstrained bucket

	std::cout << "============ example6a map ============" << std::endl;

	bucket.spread(
		make_time(2003, 11, 17, 9, 0, 0),
		make_time(2003, 11, 17, 17, 0, 0),
		"Open"
	);

	bucket.spread(
		make_time(2003, 11, 17, 12, 0, 0),
		make_time(2003, 11, 17, 13, 0, 0),
		"Lunch"
	);

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket_map>(bucket) << std::endl;
}

void example6b_legacy() {
	string_key_bucket<char> bucket; // unconstrained bucket

	std::cout << "============ example6b legacy ============" << std::endl;

	bucket.spread(
		"2003-11-17 09:00:00",
		"2003-11-17 17:00:00",
		"Open"
	);

	bucket.spread(
		"2003-11-17 12:00:00",
		"2003-11-17 13:00:00",
		"Lunch"
	);

	std::cout << "bucket is:\n" << bucket_wrapper<string_key_bucket<char>>(bucket) << std::endl;
}

void example6b_list() {
	string_key_bucket_list<char> bucket; // unconstrained bucket

	std::cout << "============ example6b list ============" << std::endl;

	bucket.spread(
		"2003-11-17 09:00:00",
		"2003-11-17 17:00:00",
		"Open"
	);

	bucket.spread(
		"2003-11-17 12:00:00",
		"2003-11-17 13:00:00",
		"Lunch"
	);

	std::cout << "bucket is:\n" << bucket_wrapper<string_key_bucket_list<char>>(bucket) << std::endl;
}

void example6b_map() {
	string_key_bucket_map<char> bucket; // unconstrained bucket

	std::cout << "============ example6b map ============" << std::endl;

	bucket.spread(
		"2003-11-17 09:00:00",
		"2003-11-17 17:00:00",
		"Open"
	);

	bucket.spread(
		"2003-11-17 12:00:00",
		"2003-11-17 13:00:00",
		"Lunch"
	);

	std::cout << "bucket is:\n" << bucket_wrapper<string_key_bucket_map<char>>(bucket) << std::endl;
}

void example7_legacy() {
	work_bucket bucket; // unconstrained bucket

	std::cout << "============ example7 legacy ============" << std::endl;

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

void example7_list() {
	work_bucket_list bucket; // unconstrained bucket

	std::cout << "============ example7 list ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket_list>(bucket) << std::endl;
}

void example7_map() {
	work_bucket_map bucket; // unconstrained bucket

	std::cout << "============ example7 map ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket_map>(bucket) << std::endl;
}

void example8_legacy() {
	work_bucket bucket; // unconstrained bucket

	std::cout << "============ example8 legacy ============" << std::endl;

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

void example8_list() {
	work_bucket_list bucket; // unconstrained bucket

	std::cout << "============ example8 list ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket_list>(bucket) << std::endl;
}

void example8_map() {
	work_bucket_map bucket; // unconstrained bucket

	std::cout << "============ example8 map ============" << std::endl;

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

	std::cout << "bucket is:\n" << bucket_wrapper<work_bucket_map>(bucket) << std::endl;
}

void example9_legacy() {
	glossary<char> bucket; // unconstrained bucket

	std::cout << "============ example9 legacy ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary<char>>(bucket) << std::endl;
}

void example9_list() {
	glossary_list<char> bucket; // unconstrained bucket

	std::cout << "============ example9 list ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary_list<char>>(bucket) << std::endl;
}

void example9_map() {
	glossary_map<char> bucket; // unconstrained bucket

	std::cout << "============ example9 map ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary_map<char>>(bucket) << std::endl;
}

void example10_legacy() {
	glossary<char> bucket('M', 'P'); // constrained bucket

	std::cout << "============ example10 legacy ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary<char>>(bucket) << std::endl;
}

void example10_list() {
	glossary_list<char> bucket('M', 'P'); // constrained bucket

	std::cout << "============ example10 list ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary_list<char>>(bucket) << std::endl;
}

void example10_map() {
	glossary_map<char> bucket('M', 'P'); // constrained bucket

	std::cout << "============ example10 map ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary_map<char>>(bucket) << std::endl;
}

void example11_legacy() {
	glossary<char, std::basic_string<char>, caseInsensitiveLess<std::basic_string<char>>> bucket; // unconstrained bucket

	std::cout << "============ example11 legacy ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary<char, std::basic_string<char>, caseInsensitiveLess<std::basic_string<char>>>>(bucket) << std::endl;
}

void example11_list() {
	glossary_list<char, std::basic_string<char>, caseInsensitiveLess<std::basic_string<char>>> bucket; // unconstrained bucket

	std::cout << "============ example11 list ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary_list<char, std::basic_string<char>, caseInsensitiveLess<std::basic_string<char>>>>(bucket) << std::endl;
}

void example11_map() {
	glossary_map<char, std::basic_string<char>, caseInsensitiveLess<std::basic_string<char>>> bucket; // unconstrained bucket

	std::cout << "============ example11 map ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary_map<char, std::basic_string<char>, caseInsensitiveLess<std::basic_string<char>>>>(bucket) << std::endl;
}

void example12_legacy() {
	glossary<char, const char*, caseInsensitiveLess<std::basic_string<char>>> bucket; // unconstrained bucket

	std::cout << "============ example12 legacy ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary<char, const char*, caseInsensitiveLess<std::basic_string<char>>>>(bucket) << std::endl;
}

void example12_list() {
	glossary_list<char, const char*, caseInsensitiveLess<std::basic_string<char>>> bucket; // unconstrained bucket

	std::cout << "============ example12 list ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary_list<char, const char*, caseInsensitiveLess<std::basic_string<char>>>>(bucket) << std::endl;
}

void example12_map() {
	glossary_map<char, const char*, caseInsensitiveLess<std::basic_string<char>>> bucket; // unconstrained bucket

	std::cout << "============ example12 map ============" << std::endl;

	for (auto it = words.begin(); it != words.end(); ++it) {
		bucket.add(*it);
	}

	std::cout << "bucket is:\n" << bucket_wrapper<glossary_map<char, const char*, caseInsensitiveLess<std::basic_string<char>>>>(bucket) << std::endl;
}

int main()
{
	std::cout << "start timestamp: " << time_stamp() << std::endl;

	std::cout << std::endl;
	example1_legacy();
	example1_list();
	example1_map();
	
	std::cout << std::endl;
	example2_legacy();
	example2_list();
	example2_map();
	
	std::cout << std::endl;
	example3_legacy();
	example3_list();
	example3_map();
	
	std::cout << std::endl;
	example4_legacy();
	example4_list();
	example4_map();
	
	std::cout << std::endl;
	example5_legacy();
	example5_list();
	example5_map();
	
	std::cout << std::endl;
	example6_legacy();
	example6_list();
	example6_map();
	
	std::cout << std::endl;
	example6a_legacy();
	example6a_list();
	example6a_map();
	
	std::cout << std::endl;
	example6b_legacy();
	example6b_list();
	example6b_map();
	
	std::cout << std::endl;
	example7_legacy();
	example7_list();
	example7_map();
	
	std::cout << std::endl;
	example8_legacy();
	example8_list();
	example8_map();
	
	std::cout << std::endl;
	example9_legacy();
	example9_list();
	example9_map();
	
	std::cout << std::endl;
	example10_legacy();
	example10_list();
	example10_map();
	
	std::cout << std::endl;
	example11_legacy();
	example11_list();
	example11_map();
	
	std::cout << std::endl;
	example12_legacy();
	example12_list();
	example12_map();

	std::cout << "\nend timestamp: " << time_stamp() << std::endl;

	return 0;
}
