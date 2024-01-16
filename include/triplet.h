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

template <typename _T1, typename _T2, typename _T3>
struct triplet
{
	typedef _T1 first_type;
	typedef _T2 second_type;
    typedef _T3 third_type;
	triplet()
	: first (_T1()), 
      second(_T2()),
      third (_T3())
    {}
	triplet( const _T1& _V1, const _T2& _V2, const _T3& _V3)
	: first (_V1), 
      second(_V2),
      third (_V3)
    {}

	~triplet() {}

    _T1  first;
    _T2  second;
    _T3  third;
};

template <typename _T1, typename _T2, typename _T3>
triplet<_T1, _T2, _T3> make_triplet( const _T1& _V1, const _T2& _V2, const _T3& _V3 )
{ return triplet<_T1, _T2, _T3>( _V1, _V2, _V3 ); }

}

#endif // MASUTILS_TRIPLET_H_

