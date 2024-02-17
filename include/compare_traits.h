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

// buckets.h : Define the bucket collection class.
//

#ifndef MASUTILS_COMPARE_TRAITS_H_
#define MASUTILS_COMPARE_TRAITS_H_

namespace masutils {

template<class _E>
struct compare_traits {

    typedef _E value_type;

    // These are the only methods that need to be defined for a compare_traits
    static bool eq(const _E& _X, const _E& _Y) { return (_X == _Y); }
    static bool lt(const _E& _X, const _E& _Y) { return (_X <  _Y); }
    static void assign(_E& _X, const _E& _Y)   { _X = _Y; }

    // These are methods derived from the above methods and should not be overridden
    static bool ne(const _E& _X, const _E& _Y) { return !eq(_X, _Y); }
    static bool le(const _E& _X, const _E& _Y) { return !lt(_Y, _X); }
    static bool gt(const _E& _X, const _E& _Y) { return  lt(_Y, _X); }
    static bool ge(const _E& _X, const _E& _Y) { return !lt(_X, _Y); }

private:
    compare_traits() = delete;
};

template<class _E>
struct compare_traits_descending : public compare_traits<_E> {

    // This might seem counterintuitive, but redefining the lt method to
    // return the opposite of the base class method will says that higher
    // values are less than lower values
    static bool lt(const _E& _X, const _E& _Y) { return (_Y < _X); }
};

} // namespace masutils


#endif // MASUTILS_COMPARE_TRAITS_H_
