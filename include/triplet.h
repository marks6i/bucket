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

#ifndef MASUTILS_TRIPLET_H_
#define MASUTILS_TRIPLET_H_

namespace masutils {

// This is a simple triplet class that is used to hold three values.  It is used
// inplace of std::tuple when the number of elements is known and fixed.  It also
// allow for simple initialization of the elements from an initializer list.
// This class can go away when there is a fix to the std::tuple class (N4387).
// This is necessary for C++14 and before.

template <typename T1, typename T2, typename T3>
struct triplet
{
	typedef T1 first_type;
	typedef T2 second_type;
    typedef T3 third_type;
	triplet()
	: first (T1()), 
      second(T2()),
      third (T3())
    {}
	triplet( const T1& v1, const T2& v2, const T3& v3)
	: first (v1), 
      second(v2),
      third (v3)
    {}

    ~triplet() = default;
    triplet(const triplet&) = default;
    triplet(triplet&&) noexcept = default;
    triplet& operator=(const triplet&) = default;
    triplet& operator=(triplet&&) noexcept = default;

    T1  first;
    T2  second;
    T3  third;
};

template <typename T1, typename T2, typename T3>
triplet<T1, T2, T3> make_triplet( const T1& v1, const T2& v2, const T3& v3 )
{ return triplet<T1, T2, T3>( v1, v2, v3 ); }

}

#endif // MASUTILS_TRIPLET_H_

