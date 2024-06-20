#include "pch.h"

#include "../include/buckets.h"
#include "../include/buckets_supp.h"
#include "../include/app/main_support.h"
#include "../include/test/support.h"

using namespace masutils;
using namespace mastest;
using namespace masxtra;

TEST(BucketTest, InvalidRange) {
	using TestBucket = buckets<int, int>;

	{
		TestBucket constrained_bucket(0, 10);
		EXPECT_EQ(constrained_bucket.size(), 0) << "initially the bucket size should be 0";

		EXPECT_FALSE(constrained_bucket.spread(5, 4, 1)) << "range is invalid, should return false";
	}

	{
		TestBucket unconstrained_bucket;
		EXPECT_EQ(unconstrained_bucket.size(), 0) << "initially the bucket size should be 0";

		EXPECT_FALSE(unconstrained_bucket.spread(5, 4, 1)) << "range is invalid, should return false";
	}
}

TEST(BucketTest, ConstrainedWithSpread) {
	using TestBucket = buckets<int, int>;
	
	TestBucket bucket(26, 74);

	EXPECT_EQ(bucket.size(), 0) << "initially the bucket size should be 0";

	EXPECT_FALSE(bucket.spread(9, 10, 1)) << "spread is before the constrained area; should return false";
	EXPECT_EQ(bucket.size(), 0) << "the range is before the constrained area; size should still be 0";

	EXPECT_FALSE(bucket.spread(10, 25, 2)) << "spread is still before the constrained area, upper bound of bucket equals lower constraint; should return false";
	EXPECT_EQ(bucket.size(), 0) << "the range is still before the constrained area; size should still be 0";

	EXPECT_TRUE(bucket.spread(30, 40, 3)) << "spread is within the constrained area; should return true";
	EXPECT_EQ(bucket.size(), 1) << "bucket in constrained range; size should be 1";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 30, 40, { 3 } } 
			}, difference_list)
		) << "bucket has 1 bucket with the correct value" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(30, 31, 4)) << "spread is within the constrained area; should return true";
	EXPECT_EQ(bucket.size(), 2) << "bucket in constrained range; size should be 2, original bucket sliced into two, from start of original bucket to end of new bucket";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 30, 31, { 3, 4 } },
				{ 31, 40, { 3 } } 
			}, difference_list)
		) << "bucket has 2 buckets with the correct value" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(50, 60, 5)) << "spread is within the constrained area; should return true";
	EXPECT_EQ(bucket.size(), 3) << "bucket in constrained range; size should be 3, one bucket not contiguous";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 30, 31, { 3, 4 } },
				{ 31, 40, { 3 } },
				{ 50, 60, { 5 } }
			}, difference_list)
		) << "bucket has 3 buckets with the correct value" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(59, 60, 6)) << "spread is within the constrained area; should return true";
	EXPECT_EQ(bucket.size(), 4) << "bucket in constrained range; size should be 4, previous bucket sliced into two from start of new bucket to end of previous";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 30, 31, { 3, 4 } },
				{ 31, 40, { 3 } },
				{ 50, 59, { 5 } },
				{ 59, 60, { 5, 6 } } 
			}, difference_list)
		) << "bucket has 4 buckets with the correct value" << std::endl << "failed: " << difference_list;
	}

	{
		bucket_compare<TestBucket>::triplet_list expected_buckets = { // using a local var here to show that the second test below does not change values
			{ 30, 31, { 3, 4 } },
			{ 31, 40, { 3 } },
			{ 50, 59, { 5 } },
			{ 59, 60, { 5, 6 } },
			{ 70, 74, { 7 } }
		};

		EXPECT_TRUE(bucket.spread(70, 80, 7)) << "spread overlaps the constrained area; should return true";
		EXPECT_EQ(bucket.size(), 5) << "bucket sliced to constrained range; size should be 5, new bucket created from start of bucket to constrained upper value";
		{
			mastest::bucket_compare<TestBucket>::instance_list difference_list;
			EXPECT_TRUE(
				bucket_compare<TestBucket>::equal(bucket, expected_buckets, difference_list)
			) << "bucket has 5 buckets with the correct value" << std::endl << "failed: " << difference_list;
		}

		EXPECT_FALSE(bucket.spread(80, 81, 8)) << "spread is after the constrained area; should return false";
		EXPECT_EQ(bucket.size(), 5) << "bucket not created, size still 5, bucket start value above upper constrained limit";
		{
			mastest::bucket_compare<TestBucket>::instance_list difference_list;
			EXPECT_TRUE(
				bucket_compare<TestBucket>::equal(bucket, expected_buckets, difference_list)
			) << "bucket still has 5 buckets with no change" << std::endl << "failed: " << difference_list;
		}
	}

	EXPECT_TRUE(bucket.spread(15, 75, 9)) << "spread overlaps the constrained area; should return true";
	EXPECT_EQ(bucket.size(), 8) << "multiple buckets created (total 8), buckets constrained and there should be no gaps within the constraints";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 26, 30, { 9 } },
				{ 30, 31, { 3, 4, 9 } },
				{ 31, 40, { 3, 9 } },
				{ 40, 50, { 9 } },
				{ 50, 59, { 5, 9 } },
				{ 59, 60, { 5, 6, 9 } },
				{ 60, 70, { 9 } },
				{ 70, 74, { 7, 9 } }
			}, difference_list)
		) << "bucket still has 5 buckets with no change" << std::endl << "failed: " << difference_list;
	}
}

TEST(BucketTest, UnconstrainedWithSpread) {
	using TestBucket = buckets<int, int>;

	TestBucket bucket;

	EXPECT_EQ(bucket.size(), 0) << "initially the bucket size should be 0";

	EXPECT_TRUE(bucket.spread(9, 10, 1)) << "new bucket created";
	EXPECT_EQ(bucket.size(), 1) << "Only one bucket created";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } }
				}, difference_list)
		) << "bucket has 1 bucket with the correct value" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(10, 25, 2)) << "new bucket created";
	EXPECT_EQ(bucket.size(), 2) << "only one new bucket created, size is 2";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } },
				{ 10, 25, { 2 } }
			}, difference_list)
		) << "bucket has 2 buckets with correct values" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(30, 40, 3)) << "new bucket created";
	EXPECT_EQ(bucket.size(), 3) << "only one new bucket created, size is 3";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } },
				{ 10, 25, { 2 } },
				{ 30, 40, { 3 } }
			}, difference_list)
		) << "bucket has 3 buckets with the correct values" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(30, 31, 4)) << "new bucket created, slicing an existing bucket";
	EXPECT_EQ(bucket.size(), 4) << "buckets overlap; size should be 4, original bucket sliced into two, from start of original bucket to end of new bucket";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } },
				{ 10, 25, { 2 } },
				{ 30, 31, { 3, 4 } },
				{ 31, 40, { 3 } }
			}, difference_list)
		) << "bucket has 4 buckets with the correct values" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(50, 60, 5)) << "new bucket created";
	EXPECT_EQ(bucket.size(), 5) << "non-contiguous bucket created; size should be 5";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } },
				{ 10, 25, { 2 } },
				{ 30, 31, { 3, 4 } },
				{ 31, 40, { 3 } },
				{ 50, 60, { 5 } }
			}, difference_list)
		) << "bucket has 5 buckets with the correct values" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(59, 60, 6)) << "new bucket created; slicing an existing bucket";
	EXPECT_EQ(bucket.size(), 6) << "new bucket created, size is 6; previous bucket sliced into two from start of new bucket to end of previous";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } },
				{ 10, 25, { 2 } },
				{ 30, 31, { 3, 4 } },
				{ 31, 40, { 3 } },
				{ 50, 59, { 5 } },
				{ 59, 60, { 5, 6 } }
			}, difference_list)
		) << "bucket has 6 buckets with the correct value" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(70, 80, 7)) << "new bucket created";
	EXPECT_EQ(bucket.size(), 7) << "new bucket created, size is 7";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } },
				{ 10, 25, { 2 } },
				{ 30, 31, { 3, 4 } },
				{ 31, 40, { 3 } },
				{ 50, 59, { 5 } },
				{ 59, 60, { 5, 6 } },
				{ 70, 80, { 7 } }
			}, difference_list)
		) << "bucket has 7 buckets with the correct values" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(80, 81, 8)) << "new bucket created";
	EXPECT_EQ(bucket.size(), 8) << "new bucket created, size is 8";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } },
				{ 10, 25, { 2 } },
				{ 30, 31, { 3, 4 } },
				{ 31, 40, { 3 } },
				{ 50, 59, { 5 } },
				{ 59, 60, { 5, 6 } },
				{ 70, 80, { 7 } },
				{ 80, 81, { 8 } }
				}, difference_list)
		) << "bucket has 8 buckets with the correct values" << std::endl << "failed: " << difference_list;
	}

	EXPECT_TRUE(bucket.spread(15, 75, 9)) << "spread adds values to all existing buckets between 15 to 75 and creates all new buckets in the gaps";
	EXPECT_EQ(bucket.size(), 13) << "multiple buckets created (total 13), there should be no gaps between 15 and 75";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		EXPECT_TRUE(
			bucket_compare<TestBucket>::equal(bucket, {
				{ 9, 10, { 1 } },
				{ 10, 15, { 2 } },
				{ 15, 25, { 2, 9 } },
				{ 25, 30, { 9 } },
				{ 30, 31, { 3, 4, 9 } },
				{ 31, 40, { 3, 9 } },
				{ 40, 50, { 9 } },
				{ 50, 59, { 5, 9 } },
				{ 59, 60, { 5, 6, 9 } },
				{ 60, 70, { 9 } },
				{ 70, 75, { 7, 9 } },
				{ 75, 80, { 7 } },
				{ 80, 81, { 8 } }
			}, difference_list)
		) << "bucket has 13 buckets with the correct values" << std::endl << "failed: " << difference_list;
	}
}

TEST(BucketTest, SpreadBucketIntoAnother) {
	using TestBucket = buckets<int, int>;

	auto initialize_bucket = [](TestBucket& bucket) {
		bucket.spread( 9, 10, 1);
		bucket.spread(10, 25, 2);
		bucket.spread(30, 40, 3);
		bucket.spread(30, 31, 4);
		bucket.spread(50, 60, 5);
		bucket.spread(59, 60, 6);
		bucket.spread(70, 80, 7);
		bucket.spread(80, 81, 8);
		bucket.spread(15, 75, 9);
	};

	TestBucket constrained_bucket(26, 74);
	initialize_bucket(constrained_bucket);

	TestBucket unconstrained_bucket;
	initialize_bucket(unconstrained_bucket);

	bucket_compare<TestBucket>::triplet_list expected_constrained_buckets = {
		{ 26, 30, { 9 } },
		{ 30, 31, { 3, 4, 9 } },
		{ 31, 40, { 3, 9 } },
		{ 40, 50, { 9 } },
		{ 50, 59, { 5, 9 } },
		{ 59, 60, { 5, 6, 9 } },
		{ 60, 70, { 9 } },
		{ 70, 74, { 7, 9 } }
	};

	bucket_compare<TestBucket>::triplet_list expected_reduced_constrained_buckets = { // 33 to 55
		{ 33, 40, { 3, 9 } },
		{ 40, 50, { 9 } },
		{ 50, 55, { 5, 9 } },
	};

	bucket_compare<TestBucket>::triplet_list expected_unconstrained_buckets = {
		{ 9, 10, { 1 } },
		{ 10, 15, { 2 } },
		{ 15, 25, { 2, 9 } },
		{ 25, 30, { 9 } },
		{ 30, 31, { 3, 4, 9 } },
		{ 31, 40, { 3, 9 } },
		{ 40, 50, { 9 } },
		{ 50, 59, { 5, 9 } },
		{ 59, 60, { 5, 6, 9 } },
		{ 60, 70, { 9 } },
		{ 70, 75, { 7, 9 } },
		{ 75, 80, { 7 } },
		{ 80, 81, { 8 } }
	};

	ASSERT_EQ(constrained_bucket.size(), 8) << "valid setup: multiple buckets created (total 8), buckets constrained and there should be no gaps within the constraints";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		ASSERT_TRUE(
			bucket_compare<TestBucket>::equal(constrained_bucket, expected_constrained_buckets, difference_list)
		) << "valid_setup: constrained bucket has 8 buckets" << std::endl << "failed: " << difference_list;
	}

	ASSERT_EQ(unconstrained_bucket.size(), 13) << "valid setup: multiple buckets created (total 13)";
	{
		mastest::bucket_compare<TestBucket>::instance_list difference_list;
		ASSERT_TRUE(
			bucket_compare<TestBucket>::equal(unconstrained_bucket, expected_unconstrained_buckets, difference_list)
		) << "valid setup: unconstrained bucket has 13 buckets" << std::endl << "failed: " << difference_list;
	}

	// ------------------------------------------------------------------------------------------------------------------------
	// Setup is finished, now let's test the assignment and copy constructors

	{
		TestBucket test_bucket(26, 74);
		test_bucket.spread(constrained_bucket); // same constraints as constrained_bucket		
		EXPECT_EQ(test_bucket.size(), 8) << "multiple buckets copied (total 8)";
		{
			mastest::bucket_compare<TestBucket>::instance_list difference_list;
			EXPECT_TRUE(
				bucket_compare<TestBucket>::equal(test_bucket, expected_constrained_buckets, difference_list)
			) << "constrained bucket assigned a constrained bucket with same constraints is an exact copy" << std::endl << "failed: " << difference_list;
		}
	}

	{
		TestBucket test_bucket(33, 55);
		test_bucket.spread(constrained_bucket); // constricted constraints of constrained_bucket		
		EXPECT_EQ(test_bucket.size(), 3) << "multiple buckets copied (total 3)";
		{
			mastest::bucket_compare<TestBucket>::instance_list difference_list;
			EXPECT_TRUE(
				bucket_compare<TestBucket>::equal(test_bucket, expected_reduced_constrained_buckets, difference_list)
			) << "constricted bucket only has buckets with union of the constraints" << std::endl << "failed: " << difference_list;
		}
	}

	{
		TestBucket test_bucket(26, 74);
		test_bucket.spread(unconstrained_bucket);
		EXPECT_EQ(test_bucket.size(), 8) << "multiple buckets copied but only those that overlap or are inside the constraints (total 8)";
		{
			mastest::bucket_compare<TestBucket>::instance_list difference_list;
			EXPECT_TRUE(
				bucket_compare<TestBucket>::equal(test_bucket, expected_constrained_buckets, difference_list)
			) << "constrained bucket assigned an unconstrained bucket applies the constraints" << std::endl << "failed: " << difference_list;
		}
	}

	{
		TestBucket test_bucket;
		test_bucket.spread(unconstrained_bucket);
		EXPECT_EQ(test_bucket.size(), 13) << "exact copy of buckets (total 13)";
		{
			mastest::bucket_compare<TestBucket>::instance_list difference_list;
			EXPECT_TRUE(
				bucket_compare<TestBucket>::equal(test_bucket, expected_unconstrained_buckets, difference_list)
			) << "bucket has 13 copied buckets" << std::endl << "failed: " << difference_list;
		}
	}
}
