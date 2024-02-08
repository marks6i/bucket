#ifndef MASTEST_SUPP_H_
#define MASTEST_SUPP_H_

#include <string>
#include <ctime>
#include <mutex>

namespace mastest {

inline std::tm localtime_xp(std::time_t timer)
{
    std::tm bt{};
#if defined(__unix__)
    localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
    localtime_s(&bt, &timer);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    bt = *std::localtime(&timer);
#endif
    return bt;
}

inline std::string time_to_string(const std::time_t bt, const std::string& fmt = "%c")
{
    auto timer = localtime_xp(bt);
    char buf[64];
	return { buf, std::strftime(buf, sizeof(buf), fmt.c_str(), &timer) };
}

inline std::string time_stamp(const std::string& fmt = "%F %T")
{
    return time_to_string(std::time(0), "%F %T");
}

inline std::time_t make_time(int year, int month, int day, int hour, int minute, int second)
{
    struct std::tm date = { second, minute, hour, day, month - 1, year - 1900 };

    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    return std::mktime(&date);
}

} // namespace mastest

#endif // MASTEST_SUPP_H_