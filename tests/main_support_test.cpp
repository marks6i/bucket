#include <gtest/gtest.h>

#include "../include/buckets.h"
#include "../include/buckets_supp.h"
#include "../include/app/main_support.h"

#include <cstdlib>
#include <ctime>
#include <limits>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#if defined(_MSC_VER)
#include <stdlib.h>
#endif

using namespace masutils;
using namespace masxtra;

namespace {

// All of the functions under test operate on local time, so pin the
// process time zone to a fixed, offset-less zone before any test runs.
// This makes the expected wall-clock values deterministic across CI
// machines regardless of their configured local time zone.
struct FixedUtcTimeZone {
    FixedUtcTimeZone() {
#if defined(_MSC_VER)
        _putenv_s("TZ", "UTC0");
        _tzset();
#else
        setenv("TZ", "UTC0", 1);
        tzset();
#endif
    }
};
const FixedUtcTimeZone force_utc_time_zone{};

} // namespace

TEST(LocaltimeXpTest, ConvertsEpochToUtcBrokenDownTime) {
    const std::tm bt = localtime_xp(0);
    EXPECT_EQ(bt.tm_year, 70); // years since 1900
    EXPECT_EQ(bt.tm_mon, 0);
    EXPECT_EQ(bt.tm_mday, 1);
    EXPECT_EQ(bt.tm_hour, 0);
    EXPECT_EQ(bt.tm_min, 0);
    EXPECT_EQ(bt.tm_sec, 0);
}

TEST(LocaltimeXpTest, ConvertsArbitraryTimestamp) {
    // 2000-01-01 00:00:00 UTC
    const std::tm bt = localtime_xp(946684800);
    EXPECT_EQ(bt.tm_year, 100);
    EXPECT_EQ(bt.tm_mon, 0);
    EXPECT_EQ(bt.tm_mday, 1);
}

TEST(TimeToStringTest, FormatsUsingProvidedFormat) {
    EXPECT_EQ(time_to_string(0, "%Y-%m-%d"), "1970-01-01");
    EXPECT_EQ(time_to_string(946684800, "%Y-%m-%d %H:%M:%S"), "2000-01-01 00:00:00");
}

TEST(TimeToStringTest, DefaultFormatProducesNonEmptyResult) {
    const auto result = time_to_string(0);
    EXPECT_FALSE(result.empty());
}

TEST(TimeStampTest, ProducesDateTimeInFixedFormatRegardlessOfArgument) {
    // time_stamp() currently ignores its `fmt` parameter and always uses
    // "%F %T"; verify the actual (documented-by-test) behavior.
    const std::regex expected_pattern(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})");
    EXPECT_TRUE(std::regex_match(time_stamp(), expected_pattern));
    EXPECT_TRUE(std::regex_match(time_stamp("%Y"), expected_pattern));
}

TEST(MakeTimeTest, ComputesEpochForKnownDate) {
    EXPECT_EQ(make_time(1970, 1, 1, 0, 0, 0), static_cast<std::time_t>(0));
    EXPECT_EQ(make_time(2000, 1, 1, 0, 0, 0), static_cast<std::time_t>(946684800));
}

TEST(GlossaryTest, UnconstrainedGroupsWordsByFirstLetterCaseSensitive) {
    glossary<char> g; // unconstrained

    g.add("apple");
    g.add("apricot");
    g.add("banana");
    g.add("Banana"); // different case, so a distinct value in a case-sensitive set

    // Bucket ranges are [toupper(c), toupper(c)+1), so 'A' -> ['A', 'B') etc.
    std::vector<std::pair<char, std::set<std::string>>> actual;
    for (auto it = g.begin(); it != g.end(); ++it) {
        actual.emplace_back(it->first, std::set<std::string>(it->third.begin(), it->third.end()));
    }

    ASSERT_EQ(actual.size(), 2u);
    EXPECT_EQ(actual[0].first, 'A');
    EXPECT_EQ(actual[0].second, (std::set<std::string>{"apple", "apricot"}));
    EXPECT_EQ(actual[1].first, 'B');
    EXPECT_EQ(actual[1].second, (std::set<std::string>{"banana", "Banana"}));
}

TEST(GlossaryTest, ConstrainedRangeDropsWordsOutsideRange) {
    glossary<char> g('M', 'P'); // constrained to letters M..P

    g.add("apple");  // outside range, dropped
    g.add("mango");  // inside range
    g.add("orange"); // inside range

    std::vector<char> firsts;
    for (auto it = g.begin(); it != g.end(); ++it) {
        firsts.push_back(it->first);
    }
    EXPECT_EQ(firsts, (std::vector<char>{'M', 'O'}));
}

TEST(GlossaryTest, CaseInsensitiveComparatorMergesDifferentCaseWords) {
    glossary<char, std::string, caseInsensitiveLess<std::string>> g;

    g.add("apricot");
    g.add("aPricot");

    ASSERT_EQ(g.size(), 1u);
    // With a case-insensitive comparator, only one of the two spellings is kept
    // (unique_bucket_value_traits uses set::insert, which is a no-op on a
    // "duplicate" as defined by the comparator).
    EXPECT_EQ(g.begin()->third.size(), 1u);
}

TEST(GlossaryTest, AddThrowsWhenFirstCharacterIsAtTypeMaximum) {
    glossary<char> g;
    const std::string word(1, static_cast<char>(std::numeric_limits<char>::max()));
    EXPECT_THROW(g.add(word), std::runtime_error);
}

TEST(BucketValueWrapperTimeTTest, StreamsQuotedFormattedTime) {
    // bucket_value_wrapper<std::time_t> stores a `const std::time_t&`
    // reference, so use a named, still-alive std::time_t rather than a
    // temporary (see the equivalent note in buckets_supp_test.cpp).
    std::time_t value = 0;
    bucket_value_wrapper<std::time_t> wrapper(value);
    std::ostringstream oss;
    oss << wrapper;
    EXPECT_EQ(oss.str(), "\"" + time_to_string(0) + "\"");
    EXPECT_EQ(wrapper.get(), static_cast<std::time_t>(0));
}
