#ifndef MAIN_SUPPORT_H_
#define MAIN_SUPPORT_H_

#ifndef MASUTILS_BUCKETS_H_
#error Must include buckets.h first
#endif // !MASUTILS_BUCKETS_H_

#ifndef MASUTILS_BUCKETS_SUPP_H_
#error Must include buckets_supp.h first
#endif // !MASUTILS_BUCKETS_SUPP_H_

#ifndef STRING_H_
#include <string>
#endif // !STRING_H_

#ifndef CTIME_H_
#include <ctime>
#endif // !CTIME_H_

#ifndef SET_H_
#include <set>
#endif // !SET_H_

#ifndef MUTEX_H_
#include <mutex>
#endif // !MUTEX_H_

#ifndef LIMITS_H_
#include <limits>
#endif // !LIMITS_H_

#ifndef CHRONO_H_
#include <chrono>
#endif // !CHRONO_H_

namespace masxtra {

inline std::tm localtime_xp(std::time_t timer)
{
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);

	// Convert std::time_t to std::tm as a local time
	std::tm bt{};

#if defined(__unix__) || defined(_MSC_VER)

	// Both GCC (Linux) and MSVC (Windows) support thread-safe "localtime_s".
	localtime_s(&bt, &timer);

#else

	// For other compilers/platforms, use std::localtime.
	// The mutex above ensures thread safety.
	auto* localTime = std::localtime(&timer);
	if (localTime) {
		bt = *localTime;
	}

#endif

	return bt;
}

inline std::string time_to_string(const std::time_t bt, const std::string& fmt = "%c")
{
	const auto timer = localtime_xp(bt);
	char buf[64];
	const auto len = std::strftime(buf, sizeof(buf), fmt.c_str(), &timer);
	return { buf, len };
}

inline std::string time_stamp(const std::string& fmt = "%F %T")
{
	return time_to_string(std::time(nullptr), "%F %T");
}

inline std::time_t make_time(const int year, const int month, const int day, const int hour, const int minute, const int second)
{
	struct std::tm date = { second, minute, hour, day, month - 1, year - 1900 };

	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	return std::mktime(&date);
}

// glossary only works because I've done a "plus 1" to the char value to create the range. This would
// work if the char whose bucket we were trying to create was equal to maximum char value.
template <typename T, typename V = std::basic_string<T>, typename P = std::less<V>>
struct glossary : public masutils::buckets<
	T,
	V,
	masutils::compare_traits<T>,
	masutils::unique_bucket_value_traits<V, std::set<V, P> > > {

	glossary() noexcept {}

	glossary(T low, T high) : masutils::buckets<
		T,
		V,
		masutils::compare_traits<T>,
		masutils::unique_bucket_value_traits<V, std::set<V, P> > >(
			std::toupper(low),
			{ T(std::toupper(high) + 1) }
		) {};

	void add(V word) {
		const auto first_char = std::toupper(word[0]);
		// Check if firstChar is at the maximum value for T
		if (first_char == std::numeric_limits<T>::max()) {
			throw std::runtime_error("Attempted to use the maximum char value. This is not production code, only for demo purposes.");
		}
		this->spread(
			std::toupper(word[0]),
			{ T(std::toupper(word[0]) + 1) }, word);
	}
};

} // namespace masxtra

namespace masutils {

	template <>
	class bucket_value_wrapper<std::time_t> {
	public:
		explicit bucket_value_wrapper(const std::time_t& _value) noexcept : value(_value) {}

		friend std::ostream& operator<<(std::ostream& os, const bucket_value_wrapper<std::time_t>& wrapper) {
			os << "\"" << masxtra::time_to_string(wrapper.get()) << "\"";
			return os;
		}

		const std::time_t& get() const noexcept {
			return value;
		}

	private:
		const std::time_t& value;
	};
}

#endif // MAIN_SUPPORT_H_