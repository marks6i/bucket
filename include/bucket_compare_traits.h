/**
 * @file  bucket_compare_traits.h
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
 * @brief Compare keys elements of a bucket.
 *
 * The \bbucket_compare_traits struct is used to compare the key
 * elements of a bucket to maintain the order of the elements.
 * If it needs to be overridden, only the \beq, \blt, and \bassign
 * methods need to be defined; the other methods are derived from
 * these.
 */

#ifndef MASUTILS_BUCKET_COMPARE_TRAITS_H_
#define MASUTILS_BUCKET_COMPARE_TRAITS_H_

namespace masutils {

/**
 * @brief The bucket_compare_traits struct has all static functions
 *        that are used to compare the key elements of a bucket to
 *        maintain the order of the elements.
 * @tparam E the type of the keys in the bucket
 * 
 * The struct nly declares static functions, so it is not necessary to
 * create an instance of this struct. All constructors are deleted to
 * prevent instantiation.
 */
template<class E>
struct bucket_compare_traits {

    typedef E value_type;

    // These are the only methods that need to be defined for a bucket_compare_traits

    /**
     * @brief Compare for equality.
     * @param x_ 
     * @param y_ 
	 * @return Boolean value indicating if the two keys are equal.
     */
    static bool eq(const E& x_, const E& y_) noexcept { return (x_ == y_); }

    /**
	 * @brief Less than comparison.
     * @param x_ 
     * @param y_ 
	 * @return Boolean value indicating if x_ is less than y_.
     */
    static bool lt(const E& x_, const E& y_) noexcept { return (x_ <  y_); }

    /**
	 * @brief Assignment operator.
     * @param x_ 
     * @param y_ 
     */
    static void assign(E& x_, const E& y_)   noexcept { x_ = y_; }

    // These are methods derived from the above methods and should not (need to) be overridden

    /**
	 * @brief Not equal comparison.
     * @param x_ 
     * @param y_ 
	 * @return Boolean value indicating if the two keys are not equal.
     */
    static bool ne(const E& x_, const E& y_) noexcept { return !eq(x_, y_); }

    /**
	 * @brief Less than or equal comparison.
     * @param x_ 
     * @param y_ 
	 * @return Boolean value indicating if x_ is less than or equal to y_.
     */
    static bool le(const E& x_, const E& y_) noexcept { return !lt(y_, x_); }

    /**
	 * @brief Greater than comparison.
     * @param x_ 
     * @param y_ 
	 * @return Boolean value indicating if x_ is greater than y_.
     */
    static bool gt(const E& x_, const E& y_) noexcept { return  lt(y_, x_); }

    /**
	 * @brief Greater than or equal comparison.
     * @param x_ 
     * @param y_ 
	 * @return Boolean value indicating if x_ is greater than or equal to y_.
     */
    static bool ge(const E& x_, const E& y_) noexcept { return !lt(x_, y_); }

private:
    bucket_compare_traits() = delete;
};

template<class E>
struct bucket_compare_traits_descending : public bucket_compare_traits<E> {

    // This might seem counterintuitive, but redefining the lt method to
    // return the opposite of the base class method will says that higher
    // values are less than lower values
    static bool lt(const E& x_, const E& y_) noexcept { return (y_ < x_); }
};

} // namespace masutils


#endif // MASUTILS_BUCKET_COMPARE_TRAITS_H_
