/**
 * @file  bucket_value_traits.h
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
 * @brief Operations supported on the bucket value container.
 *
 * The \bbucket_value_traits struct describes all the operations that can be
 * performed on the value collection of a bucket. Any collection type can be
 * used as the value container, but the operations must be defined for the
 * container type. Currently, the operations are \badd, \bappend, and \bremove.
 */

#ifndef MASUTILS_BUCKET_VALUE_TRAITS_H_
#define MASUTILS_BUCKET_VALUE_TRAITS_H_

namespace masutils {

/**
	* @brief The bucket_value_traits struct has all static functions
	*        that are the operations that can be performed on a
			 bucket collection.
	* @tparam E_ the type of the values in the bucket
	* @tparam C_ the type of the container used to store the values
	*
	* The struct only declares static functions, so it is not necessary to
	* create an instance of this struct. All constructors are deleted to
	* prevent instantiation.
	*/
template <class E_, class C_ = std::list<E_>>
struct bucket_value_traits
{
	typedef E_ value_type;
	typedef C_ value_container;

	/**
	 * @brief Add a value to a bucket.
	 * @param x_ the bucket's value container
	 * @param y_ the value to add to the bucket
	 */
	static void add(value_container& x_, const value_type& y_)
	{
		x_.push_back(y_);
	}

	/**
	 * @brief Append one value container into another.
	 * @tparam other_value_container 
	 * @param x_ The receiving value container
	 * @param y_ The value container to append
	 */
	template <class other_value_container>
	static void append(value_container& x_, const other_value_container& y_)
	{
		x_.insert(x_.end(), y_.begin(), y_.end());
	}

	/**
	 * @brief Remove a value from a bucket.
	 * @param x_ The value container
	 * @param y_ The value to remove
	 * @return Boolean value indicating if the value was removed
	 */
	static bool remove(value_container& x_, const value_type& y_)
	{
		for (auto p = x_.begin(); p != x_.end(); ++p)
		{
			if (*p == y_)
			{
				x_.erase(p);
				return true;
			}
		}
		return false;
	}

protected:
	~bucket_value_traits() = default;
};

} // namespace masutils


#endif // MASUTILS_BUCKET_COMPARE_TRAITS_H_
