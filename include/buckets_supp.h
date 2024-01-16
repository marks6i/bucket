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
	static void append(value_container& _X, const value_container& _Y)
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
	static void append(value_container& _X, const value_container& _Y)
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

}

#endif // MASUTILS_BUCKETS_SUPP_H_
