#ifndef MASTEST_SUPPORT_H_
#define MASTEST_SUPPORT_H_

#ifndef MASUTILS_BUCKETS_H_
#error Must include buckets.h first
#endif // !MASUTILS_BUCKETS_H_

#ifndef MASUTILS_BUCKETS_SUPP_H_
#error Must include buckets_supp.h first
#endif // !MASUTILS_BUCKETS_SUPP_H_

#ifndef MAIN_SUPPORT_H_
#error Must include main_support.h first
#endif // !MAIN_SUPPORT_H_

#ifndef UTILITY_H_
#include <utility>
#endif // !UTILITY_H_

#ifndef IOSFWD_H_
#include <iosfwd>
#endif // !IOSFWD_H_

#ifndef CSTRING_H_
#include <cstring>
#endif // !CSTRING_H_

using std::ostream;

namespace mastest {

template<typename T>
struct bucket_bucket_compare_traits {
public:
	using compare_type = T;

	static bool eq(const T& x, const T& y) { return (x == y); }
protected:
	~bucket_bucket_compare_traits() {}
};

template<>
struct bucket_bucket_compare_traits<char*> {
public:
	using compare_type = char*;

	static bool eq(const char* x, const char* y) { return !strcmp(x, y); }
};

template<typename E>
struct bucket_compare_case_insensitive_traits : public bucket_bucket_compare_traits<std::basic_string<E>> {
public:
	using compare_type = std::basic_string<E>;

	static bool eq(const compare_type& x, const compare_type& y) {
		const compare_type& x1 = x;
		const compare_type& y1 = y;
		return std::equal(x1.begin(), x1.end(), y1.begin(), y1.end(),
			[](const E& x2, const E& y2) {
				return std::tolower(x2) == std::tolower(y2);
			}
		);
	}
};

template <class BucketType,
	      class ValueCompare = bucket_bucket_compare_traits<typename BucketType::value_type>,
		  class IndexCompare = bucket_bucket_compare_traits<typename BucketType::index_type> >
class bucket_compare
{
public:
	using mytype        = bucket_compare<BucketType, ValueCompare, IndexCompare>;

	using index_type    = typename BucketType::index_type;
	using value_type    = typename BucketType::value_type;

	using bucket_type  = typename masutils::triplet<index_type,
				                                     index_type,
				                                     std::list<value_type> >;
	using bucket_type_list  = typename std::list<bucket_type>;

	using instance_type = typename masutils::triplet<index_type,
						                             index_type,
				                                     masutils::optional<value_type> >;

	struct instance_pair {
		bool mismatch         = false;
		bool missing_actual   = false;
		bool missing_expected = false;
		instance_type actual;
		instance_type expected;
	};

	using instance_list = typename std::list<instance_pair>;


	static bool equal(const BucketType& actual, const bucket_type_list& expected, instance_list& difference_list, const bool include_matches = false)
	{
		bool b_equal = true;

		auto it_actual   = actual.begin();
		auto it_expected = expected.begin();

		for (; it_actual != actual.end() && it_expected != expected.end();
			++it_actual, ++it_expected)
		{
			if (!IndexCompare::eq(it_actual->first, it_expected->first) ||
				!IndexCompare::eq(it_actual->second, it_expected->second)) {
				b_equal = false;
				difference_list.push_back({ true, false, false,
					{ it_actual->first,   it_actual->second,   masutils::optional<value_type>() },
					{ it_expected->first, it_expected->second, masutils::optional<value_type>() }
				});
			}
			else {
				auto it_values_actual   = it_actual->third.begin();
				auto it_values_expected = it_expected->third.begin();

				for (; it_values_actual != it_actual->third.end() && it_values_expected != it_expected->third.end();
					++it_values_actual, ++it_values_expected)
				{
					if (!ValueCompare::eq(*it_values_actual, *it_values_expected))
					{
						b_equal = false;
						difference_list.push_back({ true, false, false,
							{ it_actual->first,   it_actual->second,   masutils::optional<value_type>(*it_values_actual) },
							{ it_expected->first, it_expected->second, masutils::optional<value_type>(*it_values_expected) }
						});
					}
					else {
						if (include_matches) {
							difference_list.push_back({ false, false, false,
								{ it_actual->first,   it_actual->second,   masutils::optional<value_type>(*it_values_actual) },
								{ it_expected->first, it_expected->second, masutils::optional<value_type>(*it_values_expected) }
							});
						}
					}
				}

				for (; it_values_actual != it_actual->third.end(); ++it_values_actual)
				{
					b_equal = false;
					difference_list.push_back({ true, false, true,
						{ it_actual->first, it_actual->second, masutils::optional<value_type>(*it_values_actual) },
						{ it_actual->first, it_actual->second, masutils::optional<value_type>() }
					});
				}

				for (; it_values_expected != it_expected->third.end(); ++it_values_expected)
				{
					b_equal = false;
					difference_list.push_back({ true, true, false,
						{ it_expected->first, it_expected->second, masutils::optional<value_type>() },
						{ it_expected->first, it_expected->second, masutils::optional<value_type>(*it_values_expected) }
					});
				}
			}
		}

		for (; it_actual != actual.end(); ++it_actual)
		{
			b_equal = false;
			difference_list.push_back({ true, false, true,
				{ it_actual->first, it_actual->second, masutils::optional<value_type>() },
				{ it_actual->first, it_actual->second, masutils::optional<value_type>() }
			});
		}

		for (; it_expected != expected.end(); ++it_expected)
		{
			b_equal = false;
			difference_list.push_back({ true, true, false,
				{ it_expected->first, it_expected->second, masutils::optional<value_type>() },
				{ it_expected->first, it_expected->second, masutils::optional<value_type>() }
			});
		}

		return b_equal;
	}

	friend std::ostream& operator<<(std::ostream& os, const instance_list& list) {
		for (auto it = list.begin(); it != list.end(); ++it) {
			if (it->mismatch) {
				os << "Mismatch: ";
				if (it->missing_actual) {
					os << "Missing actual: ";
					os << "expected: " << it->expected.first << ", " << it->expected.second << ", " << it->expected.third << std::endl;
				}
				else if (it->missing_expected) {
					os << "Missing expected: ";
					os << "actual: "   << it->actual.first   << ", " << it->actual.second   << ", " << it->actual.third   << std::endl;
				}
				else {
					os << "Index mismatch: ";
					os << "actual: "   << it->actual.first   << ", " << it->actual.second   << ", " << it->actual.third   << " ";
 					os << "expected: " << it->expected.first << ", " << it->expected.second << ", " << it->expected.third << std::endl;
				}
			}
			else {
				os << "Match: ";
				os << "actual: "       << it->actual.first   << ", " << it->actual.second   << ", " << it->actual.third   << " ";
				os << "expected: "     << it->expected.first << ", " << it->expected.second << ", " << it->expected.third << std::endl;
			}
		}
		return os;
	}
};

} // namespace mastest

#endif // !MASTEST_SUPPORT_H_