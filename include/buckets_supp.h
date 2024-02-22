// Copyright 2024 Mark Solinski
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// buckets_supp.h - Supplemental file to define some common support classes/templates

#ifndef MASUTILS_BUCKETS_SUPP_H_
#define MASUTILS_BUCKETS_SUPP_H_

#ifndef MASUTILS_BUCKETS_H_
#error Must include buckets.h first
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

#ifndef MASUTILS_OPTIONAL_H_
#include "optional.h"
#endif // MASUTILS_OPTIONAL_H_

namespace masutils {

template<class _E, class _C = std::vector<_E> >
struct most_recent_bucket_value_traits {

	typedef _E  value_type;
	typedef _C  value_container;

	static void add(value_container& _X, const value_type& _Y)
	{
		if (_X.size() > 0) {
			_X.front() = _Y;
		}
		else {
			_X.push_back(_Y);
		}
	}

	template<typename other_value_container>
	static void append(value_container& _X, const other_value_container& _Y)
	{
		add(_X, _Y.back());
	}
protected:
	~most_recent_bucket_value_traits() {}
};

template<class _E, class _C = std::vector<_E> >
struct bucket_value_add_traits {

	typedef _E  value_type;
	typedef _C  value_container;

	typedef typename value_container::const_iterator const_iterator;

	static void add(value_container& _X, const value_type& _Y)
	{
		if (_X.size() > 0) {
			_X.front() += _Y;
		}
		else {
			_X.push_back(_Y);
		}
	}

	template<typename other_value_container>
	static void append(value_container& _X, const other_value_container& _Y)
	{
		for (const_iterator p = _Y.begin();
			p != _Y.end();
			++p)
		{
			add(_X, *p);
		}
	}
protected:
	~bucket_value_add_traits() {}
};

template<class _E, class _C = std::set<_E> >
struct unique_bucket_value_traits {

	typedef _E  value_type;
	typedef _C  value_container;

	typedef typename value_container::const_iterator const_iterator;

	static void add(value_container& _X, const value_type& _Y)
	{
		_X.insert(_Y);
	}

	template<typename other_value_container>
	static void append(value_container& _X, const other_value_container& _Y)
	{
		for (const_iterator p = _Y.begin();
			p != _Y.end();
			++p)
		{
			add(_X, *p);
		}
	}
protected:
	~unique_bucket_value_traits() {}
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

template<typename _T>
class bucket_value_wrapper {
public:
	bucket_value_wrapper(const _T& _value) : value(_value) {}

	friend std::ostream& operator<<(std::ostream& os, const bucket_value_wrapper& wrapper) {
		os << wrapper.get();
		return os;
	}

	const _T& get() const {
		return value;
	}

private:
	const _T& value;
};

template <typename CharT, typename Traits, typename Alloc>
class bucket_value_wrapper<std::basic_string<CharT, Traits, Alloc>> {
public:
	bucket_value_wrapper(const std::basic_string<CharT, Traits, Alloc>& _value) : value(_value) {}

	friend std::ostream& operator<<(std::ostream& os, const bucket_value_wrapper<std::basic_string<CharT, Traits, Alloc>>& wrapper) {
		os << "\"" << wrapper.get() << "\"";
		return os;
	}

	std::basic_string<CharT, Traits, Alloc> get() const {
		return value;
	}

private:
	const std::basic_string<CharT, Traits, Alloc>& value;
};

template <>
class bucket_value_wrapper<char*> {
public:
	bucket_value_wrapper(const char* _value) : value(_value) {}

	friend std::ostream& operator<<(std::ostream& os, const bucket_value_wrapper<char*>& wrapper) {
		os << "\"" << wrapper.get() << "\"";
		return os;
	}

	const std::basic_string<char>& get() const {
		return value;
	}

private:
	const std::basic_string<char> value;
};

template<typename bucket_type,
	typename index_wrapper = bucket_value_wrapper<bucket_type::index_type>,
	typename value_wrapper = bucket_value_wrapper<bucket_type::value_type> >
class bucket_wrapper {
public:
	bucket_wrapper(const bucket_type& _bucket) : bucket(_bucket) {}

	friend std::ostream& operator<<(std::ostream& os, const bucket_wrapper& wrapper) {
		bool bFirst = true;
		for (auto it = wrapper.bucket.begin(); it != wrapper.bucket.end(); ++it) {
			if (!bFirst) {
				os << "," << std::endl;
			}
			os << "{ " << index_wrapper(it->first) << ", " << index_wrapper(it->second) << ", ";
			os << "{ ";
			bool bFirst2 = true;
			for (auto it2 = it->third.begin(); it2 != it->third.end(); ++it2) {
				if (!bFirst2) {
					os << ", ";
				}
				os << value_wrapper(*it2);
				bFirst2 = false;
			}
			os << " } }";
			bFirst = false;
		}
		return os;
	}

private:
	const bucket_type& bucket;
};

} // namespace masutils

#endif // MASUTILS_BUCKETS_SUPP_H_
