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

template<typename _T>
struct bucket_compare_traits {
public:
	using compare_type = _T;

	static bool eq(const _T& _X, const _T& _Y) { return (_X == _Y); }
protected:
	~bucket_compare_traits() {}
};

template<>
struct bucket_compare_traits<char*> {
public:
	using compare_type = char*;

	static bool eq(const char* _X, const char* _Y) { return !strcmp(_X, _Y); }
};

template<typename _E>
struct bucket_compare_case_insensitive_traits : public bucket_compare_traits<std::basic_string<_E>> {
public:
	using compare_type = std::basic_string<_E>;

	static bool eq(const compare_type& _X, const compare_type& _Y) {
		const compare_type& x = _X;
		const compare_type& y = _Y;
		return std::equal(x.begin(), x.end(), y.begin(), y.end(),
			[](const _E& x, const _E& y) {
				return std::tolower(x) == std::tolower(y);
			}
		);
	}
};

template <class bucket_type,
	      class value_compare = bucket_compare_traits<bucket_type::value_type>,
		  class index_compare = bucket_compare_traits<bucket_type::index_type> >
class bucket_compare
{
public:
	using mytype        = bucket_compare<bucket_type, value_compare, index_compare>;

	using index_type    = typename bucket_type::index_type;
	using value_type    = typename bucket_type::value_type;

	using triplet_type  = typename masutils::triplet<index_type,
				                                     index_type,
				                                     std::list<value_type> >;
	using triplet_list  = typename std::list<triplet_type>;

	using instance_type = typename masutils::triplet<index_type,
						                             index_type,
				                                     masutils::optional<value_type> >;

	struct instance_pair {
		bool mismatch        = false;
		bool missingActual   = false;
		bool missingExpected = false;
		instance_type actual;
		instance_type expected;
	};

	using instance_list = typename std::list<instance_pair>;


	static bool equal(const bucket_type& actual, const triplet_list& expected, instance_list& difference_list, bool includeMatches = false)
	{
		bool bEqual = true;

		auto it_actual   = actual.begin();
		auto it_expected = expected.begin();

		for (; it_actual != actual.end() && it_expected != expected.end();
			++it_actual, ++it_expected)
		{
			if (!index_compare::eq(it_actual->first, it_expected->first) ||
				!index_compare::eq(it_actual->second, it_expected->second)) {
				bEqual = false;
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
					if (!value_compare::eq(*it_values_actual, *it_values_expected))
					{
						bEqual = false;
						difference_list.push_back({ true, false, false,
							{ it_actual->first,   it_actual->second,   masutils::optional<value_type>(*it_values_actual) },
							{ it_expected->first, it_expected->second, masutils::optional<value_type>(*it_values_expected) }
						});
					}
					else {
						if (includeMatches) {
							difference_list.push_back({ false, false, false,
								{ it_actual->first,   it_actual->second,   masutils::optional<value_type>(*it_values_actual) },
								{ it_expected->first, it_expected->second, masutils::optional<value_type>(*it_values_expected) }
							});
						}
					}
				}

				for (; it_values_actual != it_actual->third.end(); ++it_values_actual)
				{
					bEqual = false;
					difference_list.push_back({ true, false, true,
						{ it_actual->first, it_actual->second, masutils::optional<value_type>(*it_values_actual) },
						{ it_actual->first, it_actual->second, masutils::optional<value_type>() }
					});
				}

				for (; it_values_expected != it_expected->third.end(); ++it_values_expected)
				{
					bEqual = false;
					difference_list.push_back({ true, true, false,
						{ it_expected->first, it_expected->second, masutils::optional<value_type>() },
						{ it_expected->first, it_expected->second, masutils::optional<value_type>(*it_values_expected) }
					});
				}
			}
		}

		for (; it_actual != actual.end(); ++it_actual)
		{
			bEqual = false;
			difference_list.push_back({ true, false, true,
				{ it_actual->first, it_actual->second, masutils::optional<value_type>() },
				{ it_actual->first, it_actual->second, masutils::optional<value_type>() }
			});
		}

		for (; it_expected != expected.end(); ++it_expected)
		{
			bEqual = false;
			difference_list.push_back({ true, true, false,
				{ it_expected->first, it_expected->second, masutils::optional<value_type>() },
				{ it_expected->first, it_expected->second, masutils::optional<value_type>() }
			});
		}

		return bEqual;
	}

	friend std::ostream& operator<<(std::ostream& os, const instance_list& list) {
		for (auto it = list.begin(); it != list.end(); ++it) {
			if (it->mismatch) {
				os << "Mismatch: ";
				if (it->missingActual) {
					os << "Missing actual: ";
					os << "expected: " << it->expected.first << ", " << it->expected.second << ", " << it->expected.third << std::endl;
				}
				else if (it->missingExpected) {
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