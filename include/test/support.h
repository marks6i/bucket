#ifndef MASTEST_SUPPORT_H_
#define MASTEST_SUPPORT_H_

#ifndef MASUTILS_BUCKETS_H_
#error Must include buckets.h first
#endif

#ifndef MASUTILS_BUCKETS_SUPP_H_
#error Must include buckets_supp.h first
#endif

#ifndef CTIME_H_
#include <ctime>
#endif // !CTIME_H_

#ifndef MUTEX_H_
#include <mutex>
#endif // !MUTEX_H_

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

// Glossary only works because I've done a "plus 1" to the char value to create the range. This would
// work if the char whose bucket we were trying to create was equal to maximum char value.
template <typename _T, typename _V = basic_string<_T>, typename _P = std::less<_V>>
struct Glossary : public masutils::buckets<
	_T,
	_V,
	masutils::bucket_traits<_T>,
	masutils::unique_bucket_value_traits<_V, std::set<_V, _P> > > {

	Glossary() {};

	Glossary(_T low, _T high) : masutils::buckets<
		_T,
		_V,
		masutils::bucket_traits<_T>,
		masutils::unique_bucket_value_traits<_V, std::set<_V, _P> > > (
				std::toupper(low),
			static_cast<_T>(std::toupper(high) + 1)
		) {};

	void add(_V word) {
		this->spread(
			std::toupper(word[0]),
			static_cast<_T>(std::toupper(word[0]) + 1), word);
	}
};

template <class _T>
struct caseInsensitiveLess {
	bool operator()(const _T& lhs, const _T& rhs) const
	{
		_T::size_type xs(lhs.size());
		_T::size_type ys(rhs.size());
		_T::size_type bound(0);

		if (xs < ys)
			bound = xs;
		else
			bound = ys;

		{
			_T::size_type i = 0;
			for (auto it1 = lhs.begin(), it2 = rhs.begin(); i < bound; ++i, ++it1, ++it2)
			{
				if (std::tolower(*it1) < std::tolower(*it2))
					return true;

				if (std::tolower(*it2) < std::tolower(*it1))
					return false;
			}
		}
		return false;
	}
};

} // namespace mastest

#endif // MASTEST_SUPPORT_H_