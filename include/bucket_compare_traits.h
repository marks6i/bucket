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
template<class ValueType>
struct bucket_compare_traits {

    typedef ValueType value_type;

    // These are the only methods that need to be defined for a bucket_compare_traits

    /**
     * @brief Compare for equality.
     * @param x 
     * @param y
	 * @return Boolean value indicating if the two keys are equal.
     */
    static bool eq(const ValueType& x, const ValueType& y) noexcept { return (x == y); }

    /**
	 * @brief Less than comparison.
     * @param x
     * @param y
	 * @return Boolean value indicating if x is less than y.
     */
    static bool lt(const ValueType& x, const ValueType& y) noexcept { return (x <  y); }

    /**
	 * @brief Assignment operator.
     * @param x 
     * @param y 
     */
    static void assign(ValueType& x, const ValueType& y)   noexcept { x = y; }

    // These are methods derived from the above methods and should not (need to) be overridden

    /**
	 * @brief Not equal comparison.
     * @param x 
     * @param y 
	 * @return Boolean value indicating if the two keys are not equal.
     */
    static bool ne(const ValueType& x, const ValueType& y) noexcept { return !eq(x, y); }

    /**
	 * @brief Less than or equal comparison.
     * @param x 
     * @param y 
	 * @return Boolean value indicating if x is less than or equal to y.
     */
    static bool le(const ValueType& x, const ValueType& y) noexcept { return !lt(y, x); }

    /**
	 * @brief Greater than comparison.
     * @param x 
     * @param y 
	 * @return Boolean value indicating if x is greater than y.
     */
    static bool gt(const ValueType& x, const ValueType& y) noexcept { return  lt(y, x); }

    /**
	 * @brief Greater than or equal comparison.
     * @param x 
     * @param y 
	 * @return Boolean value indicating if x is greater than or equal to y.
     */
    static bool ge(const ValueType& x, const ValueType& y) noexcept { return !lt(x, y); }

private:
    bucket_compare_traits() = delete;
};

template<class ValueType>
struct bucket_compare_traits_descending : public bucket_compare_traits<ValueType> {

    // This might seem counterintuitive, but redefining the lt method to
    // return the opposite of the base class method will says that higher
    // values are less than lower values
    static bool lt(const ValueType& x, const ValueType& y) noexcept { return (y < x); }
};

} // namespace masutils


#endif // MASUTILS_BUCKET_COMPARE_TRAITS_H_
