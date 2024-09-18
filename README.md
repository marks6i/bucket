# bucket

A bucket is a collection much like a Dictionary (key-value store), where the "keys" represent a non-overlapping range on an ordered axis.
Gaps are allowed if there are not values in the given range. The "values" are stored in a collection as well, and can be accessed by the key.

## ***New in his Release***
- An erase function to remove entirely or in part the buckets that overlap with a given range.
- bucket_value_traits now has a remove function that removes a value from the bucket.
- Two new public methods: *lower_bound* and *upper_bound*. *lower_bound* returns the lower bound of a constrained bucket; *upper_bound* returns
the upper bound of a constrained bucket. These methods will throw a runtime error if the bucket is
not constrained.
- Every bucket class contains a nested class, *range_iterator*, with four methods: *beginRange*, *endRange*,
*rbeginRange* and *rendRange*. These methods, take two arguments - the lower bound of the bucket range to be
iterated and the upper bound of the range to be iterated. These functions will start with the first bucket that
overlaps (or starts at) with the given range and end with the first bucket past the given range.
- In the bucket_supp.h file, there is now a bucket_wrapper class that can be used to output the contents of a bucket
to a standard stream. This class is useful for debugging purposes.


## Description

One of the primary use cases for a bucket would be to provide a timeline or work schedule, where different people would be scheduled to work. In this scenario, a new bucket would
be created any time a (or several) new person is added to the schedule, and the bucket would be terminated when any person is removed from the schedule.

Let's look at an example of a typical day in a bakery:

Open from 7:00am until 2:00pm, the bakery has five people working at different times.
Rush period: 8:00am until 12:00pm.

Sarah, the salesperson, starts work at 7:00am when the bakery opens, and works until 2:00pm.
John, the baker, starts at 4:00am, and leaves when he is done baking at 10:00am.
Ruby works part-time to handle the rush period and starts at 8:00am, and leaves at 12:00pm.
Phil, the cleanup guy, starts at 2:00pm (when the bakery closes) and works until 6:00pm.
Mark, the owner, is there from 6:00am until 6:00pm.

So, what does the schedule look like?

```
 ​4:00am -  6:00am: John  
​ 6:00am -  7:00am: John, Mark  
​ 7:00am -  8:00am: Sarah, John, Mark  
​ 8:00am - 10:00am: Sarah, John, Ruby, Mark  
​10:00am - 12:00pm: Sarah, Ruby, Mark  
​12:00pm -  2:00pm: Sarah, Mark  
 2:00pm -  6:00pm: Phil, Mark
```  

From this we can see that there are four people working the rush period from 8:00am until 10:00am, and that 3 people are working from 10:00am until 12:00pm.

So, how would we represent this in a bucket?

```C++
#include <ctime>

#include "../include/buckets.h"

using namespace masutils;

using WorkBucket = buckets<time_t, char*>;

WorkBucket bakery_schedule;

int main() {
	const int HOUR_OF_DAY = 60 * 60;

	bakery_schedule.spread( 7 * HOUR_OF_DAY, 14 * HOUR_OF_DAY, "Sarah");
	bakery_schedule.spread( 4 * HOUR_OF_DAY, 10 * HOUR_OF_DAY, "John");
	bakery_schedule.spread( 8 * HOUR_OF_DAY, 12 * HOUR_OF_DAY, "Ruby");
	bakery_schedule.spread(14 * HOUR_OF_DAY, 18 * HOUR_OF_DAY, "Phil");
	bakery_schedule.spread( 6 * HOUR_OF_DAY, 18 * HOUR_OF_DAY, "Mark");

	return 0;
}
```

There are two basic operations for a bucket - spread and cover.  We can see from above how the spread operation might work. When we spread Sarah's time from 7:00am until 2:00pm, we are looking for existing buckets that partially or completely overlap with the given range. Since there are no existing buckets, a new bucket is created and the value "Sarah" is stored in the bucket.

Now, when we spread John's time from 4:00am until 10:00am, we see that there is already an existing bucket which partially overlaps with John's time - 7am to 2pm. In this case, the existing bucket from 7am to 2pm is split into two buckets - 7am to 10am and 10am to 2pm. Sarah is still in both of these buckets, and John is added to the bucket from 7am to 10am. A new bucket is created from 4am to 7am, and John is added to this bucket. After these two spread operations, there are now three buckets in the bakery_schedule and it looks
like this:

```
​ 4:00am -  7:00am: John  
​ 7:00am - 10:00am: John, Sarah  
​10:00am -  2:00pm: Sarah
```  

The cover operation does not add the value to existing buckets, rather it removes either wholly or in part any bucket that overlaps with the cover range and adds a new bucket with the given value.

I like to think of the spread operation as a "fill" operation, and the cover operation as a "paint" operation.  With spread, you are filling contiguous buckets in the range, creating new buckets as needed. With cover, you are painting over existing buckets, creating a new bucket.
