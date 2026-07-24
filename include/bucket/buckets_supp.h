/**
 * @file  bucket_supp.h
 * @copyright
 * Copyright 2024 Mark Solinski
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @brief Supplemental functions for the buckets class.
 */

#ifndef MASUTILS_BUCKETS_SUPP_H_
#define MASUTILS_BUCKETS_SUPP_H_

#if !defined(MASUTILS_BUCKET_LIST_H_) && !defined(MASUTILS_BUCKET_MAP_H_)
#error Must include bucket_list.h or bucket_map.h first
#endif

#ifndef VECTOR_H_
#include <vector>
#endif // VECTOR_H_

#ifndef SET_H_
#include <set>
#endif // SET_H_

#ifndef STRING_H_
#include <string>
#endif // STRING_H_

#ifndef ALGORITHM_H_
#include <algorithm>
#endif // ALGORITHM_H_

#ifndef CCTYPE_H_
#include <cctype>
#endif // CCTYPE_H_

#ifndef IOSTREAM_H_
#include <iostream>
#endif // IOSTREAM_H_

namespace masutils {
/**
 * @brief Value traits functions for a LIFO value collection.
 * @tparam E the type of the values.
 * @tparam C the class of the value container.
 */
template<class E, class C = std::vector<E> >
struct most_recent_bucket_value_traits {

	using value_type = E;
	using value_container = C;

	/**
	 * @brief 
	 * @param x 
	 * @param y 
	 */
	static void add(value_container& x, const value_type& y)
	{
		if (x.size() > 0) {
			x.front() = y;
		}
		else {
			x.push_back(y);
		}
	}

	/**
	 * @brief 
	 * @tparam OtherValueContainer 
	 * @param x 
	 * @param y 
	 */
	template<typename OtherValueContainer>
	static void append(value_container& x, const OtherValueContainer& y)
	{
		add(x, y.back());
	}

	/**
	 * @brief 
	 * @param x 
	 */
	static void remove(value_container& x)
	{
		if (x.size() > 0) {
			x.pop_back();
		}
	}

protected:
	~most_recent_bucket_value_traits() = default;
};

/**
 * @brief 
 * @tparam E 
 * @tparam C 
 */
template<class E, class C = std::vector<E> >
struct bucket_value_add_traits {

	using value_type = E;
	using value_container = C;

	using const_iterator = typename value_container::const_iterator;

	/**
	 * @brief 
	 * @param _X 
	 * @param _Y 
	 */
	static void add(value_container& _X, const value_type& _Y)
	{
		if (_X.size() > 0) {
			_X.front() += _Y;
		}
		else {
			_X.push_back(_Y);
		}
	}

	/**
	 * @brief 
	 * @tparam other_value_container 
	 * @param _X 
	 * @param _Y 
	 */
	template<typename other_value_container>
	static void append(value_container& _X, const other_value_container& _Y)
	{
		for (const auto& p : _Y) {
			add(_X, p);
		}
	}

	/**
	 * @brief 
	 * @param _X 
	 */
	static void remove(value_container& _X)
	{
		if (_X.size() > 0) {
			_X.pop_back();
		}
	}

protected:
	~bucket_value_add_traits() = default;
};

/**
 * @brief 
 * @tparam E 
 * @tparam C 
 */
template<class E, class C = std::set<E> >
struct unique_bucket_value_traits {

	using value_type = E;
	using value_container = C;

	using const_iterator = typename value_container::const_iterator;

	/**
	 * @brief 
	 * @param x 
	 * @param y 
	 */
	static void add(value_container& x, const value_type& y)
	{
		x.insert(y);
	}

	/**
	 * @brief 
	 * @tparam other_value_container 
	 * @param x 
	 * @param y 
	 */
	template<typename other_value_container>
	static void append(value_container& x, const other_value_container& y)
	{
		for (const auto& p : y) {
			add(x, p);
		}
	}

	/**
	 * @brief 
	 * @param x 
	 */
	static void remove(value_container& x)
	{
		if (x.size() > 0) {
			x.erase(x.begin());
		}
	}

protected:
	~unique_bucket_value_traits() = default;
};

/**
 * @brief 
 * @tparam T 
 */
template <class T>
struct caseInsensitiveLess {
	/**
	 * @brief Compares two strings case-insensitively.
	 * @param lhs First string.
	 * @param rhs Second string.
	 * @return True if lhs is lexicographically smaller than rhs (case-insensitively).
	 */
	bool operator()(const T& lhs, const T& rhs) const noexcept
	{
		const typename T::size_type xs(lhs.size());
		const typename T::size_type ys(rhs.size());
		const typename T::size_type bound = std::min(xs, ys);

		{
			typename T::size_type i = 0;
			for (auto it1 = lhs.begin(), it2 = rhs.begin(); i < bound; ++i, ++it1, ++it2)
			{
				if (std::tolower(*it1) < std::tolower(*it2))
					return true;

				if (std::tolower(*it2) < std::tolower(*it1))
					return false;
			}
		}

		// If all characters up to the length of the shorter string are equal,
		// the shorter string is considered lexicographically smaller.
		return xs < ys;
	}
};

/**
 * @brief 
 * @tparam T 
 */
template<typename T>
class bucket_value_wrapper {
public:
	/**
	 * @brief 
	 * @param value_ 
	 */
	explicit bucket_value_wrapper(const T& value_) noexcept : value(value_) {}

	/**
	 * @brief 
	 * @param os 
	 * @param wrapper 
	 * @return 
	 */
	friend std::ostream& operator<<(std::ostream& os, const bucket_value_wrapper& wrapper) {
		os << wrapper.get();
		return os;
	}

	/**
	 * @brief 
	 * @return 
	 */
	const T& get() const noexcept {
		return value;
	}

private:
	const T& value;
};

/**
 * @brief 
 * @tparam CharT 
 * @tparam Traits 
 * @tparam Alloc 
 */
template <typename CharT, typename Traits, typename Alloc>
class bucket_value_wrapper<std::basic_string<CharT, Traits, Alloc>> {
public:
	/**
	 * @brief 
	 * @param value_ 
	 */
	explicit bucket_value_wrapper(const std::basic_string<CharT, Traits, Alloc>& value_) noexcept : value(value_) {}

	/**
	 * @brief 
	 * @param os 
	 * @param wrapper 
	 * @return 
	 */
	friend std::ostream& operator<<(std::ostream& os, const bucket_value_wrapper<std::basic_string<CharT, Traits, Alloc>>& wrapper) {
		os << "\"" << wrapper.get() << "\"";
		return os;
	}

	/**
	 * @brief 
	 * @return 
	 */
	std::basic_string<CharT, Traits, Alloc> get() const {
		return value;
	}

private:
	const std::basic_string<CharT, Traits, Alloc>& value;
};

/**
 * @brief 
 */
template <>
class bucket_value_wrapper<char*> {
public:
	/**
	 * @brief 
	 * @param value_ 
	 */
	explicit bucket_value_wrapper(const char* value_) : value(value_) {}

	/**
	 * @brief 
	 * @param os 
	 * @param wrapper 
	 * @return 
	 */
	friend std::ostream& operator<<(std::ostream& os, const bucket_value_wrapper<char*>& wrapper) {
		os << "\"" << wrapper.get() << "\"";
		return os;
	}

	/**
	 * @brief 
	 * @return 
	 */
	const std::basic_string<char>& get() const noexcept {
		return value;
	}

private:
	const std::basic_string<char> value;
};

/**
 * @brief 
 * @tparam bucket_type 
 * @tparam IndexWrapper 
 * @tparam ValueWrapper 
 */
template<typename bucket_type,
	typename IndexWrapper = bucket_value_wrapper<typename bucket_type::index_type>,
	typename ValueWrapper = bucket_value_wrapper<typename bucket_type::value_type> >
class bucket_wrapper {
public:
	/**
	 * @brief 
	 * @param bucket_ 
	 */
	explicit bucket_wrapper(const bucket_type& bucket_) noexcept : bucket(bucket_) {}

	/**
	 * @brief 
	 * @param os 
	 * @param wrapper 
	 * @return 
	 */
	friend std::ostream& operator<<(std::ostream& os, const bucket_wrapper& wrapper) {
		bool b_first = true;

		auto& bucket_ = wrapper.bucket;

		if (bucket_.is_constrained()) {
			os  << "constrained["
				<< bucket_.lower_bound() << ", "
				<< bucket_.upper_bound() << ")," << std::endl;
		}
		else {
			os << "unconstrained," << std::endl;
		}

		for (auto& item : bucket_) {
			if (!b_first) {
				os << "," << std::endl;
			}
			os << "{ " << IndexWrapper(bucket_type::accessor::low(item)) << ", " << IndexWrapper(bucket_type::accessor::high(item)) << ", ";
			os << "{ ";
			bool b_first2 = true;
			auto value_container = bucket_type::accessor::values(item);
			for (auto& value : value_container) {
				if (!b_first2) {
					os << ", ";
				}
				os << ValueWrapper(value);
				b_first2 = false;
			}
			os << " } }";
			b_first = false;
		}
		return os;
	}

private:
	const bucket_type& bucket;
};

} // namespace masutils

#endif // MASUTILS_BUCKETS_SUPP_H_
