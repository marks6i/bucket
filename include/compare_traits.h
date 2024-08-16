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

template<class E>
struct compare_traits {

    typedef E value_type;

    // These are the only methods that need to be defined for a compare_traits
    static bool eq(const E& x_, const E& y_) noexcept { return (x_ == y_); }
    static bool lt(const E& x_, const E& y_) noexcept { return (x_ <  y_); }
    static void assign(E& x_, const E& y_)   noexcept { x_ = y_; }

    // These are methods derived from the above methods and should not (need to) be overridden
    static bool ne(const E& x_, const E& y_) noexcept { return !eq(x_, y_); }
    static bool le(const E& x_, const E& y_) noexcept { return !lt(y_, x_); }
    static bool gt(const E& x_, const E& y_) noexcept { return  lt(y_, x_); }
    static bool ge(const E& x_, const E& y_) noexcept { return !lt(x_, y_); }

private:
    compare_traits() = delete;
};

template<class E>
struct compare_traits_descending : public compare_traits<E> {

    // This might seem counterintuitive, but redefining the lt method to
    // return the opposite of the base class method will says that higher
    // values are less than lower values
    static bool lt(const E& x_, const E& y_) noexcept { return (y_ < x_); }
};

} // namespace masutils


#endif // MASUTILS_COMPARE_TRAITS_H_
