/**
 * @file  bucket_traits.h
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
 * @brief Traits for bucket containers.
 */

#ifndef MASUTILS_BUCKET_TRAITS_H_
#define MASUTILS_BUCKET_TRAITS_H_

#include <iterator>
#include <type_traits>
#include <algorithm>

namespace masutils
{
	/**
	 * @brief Traits class for bucket containers.
	 * @tparam Container The type of the bucket container.
	 */
	template <class Container>
	struct bucket_traits
	{
		/**
		 * @brief Indicates whether the container supports reverse iterators.
		 */
		static constexpr bool supports_reverse_iterators = std::is_base_of_v<std::bidirectional_iterator_tag, typename std::iterator_traits<typename Container::iterator>::iterator_category>;

		/**
		 * @brief Indicates whether the container supports binary search via std::binary_search.
		 * This requires random access iterators.
		 */
		static constexpr bool supports_binary_search = std::is_base_of_v<std::random_access_iterator_tag, typename std::iterator_traits<typename Container::iterator>::iterator_category>;

		/**
		 * @brief Indicates whether the container supports lower_bound and upper_bound operations.
		 * This is typically true for std::map and std::multimap.
		 */
		template <typename T = Container>
		static constexpr bool supports_bound_operations = 
			std::is_same_v<decltype(std::declval<T>().lower_bound(std::declval<typename T::index_type>())), 
				typename T::iterator> &&
			std::is_same_v<decltype(std::declval<T>().upper_bound(std::declval<typename T::index_type>())), 
				typename T::iterator>;
	};
}

#endif // MASUTILS_BUCKET_TRAITS_H_ 