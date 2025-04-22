/**
 * @file  bucket_range.h
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
 * @brief The bucket_range class.
 *
 * A bucket_range provides iterators for iterating over a range of buckets
 * in a bucket collection (bucket_list or bucket_map).
 */

#ifndef MASUTILS_BUCKET_RANGE_H_
#define MASUTILS_BUCKET_RANGE_H_

#include <iterator>
#include <type_traits>

namespace masutils
{
	/**
	 * @brief The bucket_range class.
	 * @tparam Container The type of the bucket container (bucket_list or bucket_map).
	 * @tparam IsConst Boolean value indicating if the iterator is const.
	 */
	template <class Container, bool IsConst>
	class bucket_range
	{
	public:
		using container_type = Container;
		using index_type = typename container_type::index_type;
		using bucket_type = typename container_type::bucket_type;
		using iterator_type = typename std::conditional_t<IsConst, 
			typename container_type::const_iterator, 
			typename container_type::iterator>;
		
		/**
		 * @brief Iterator for iterating over a range of buckets.
		 */
		class iterator
		{
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = typename std::conditional<IsConst, const bucket_type, bucket_type>::type;
			using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
			using pointer = value_type*;
			using reference = typename std::iterator_traits<iterator_type>::reference;

			/**
			 * @brief Constructor.
			 * @param container The container to iterate over.
			 * @param start_range The start of the range.
			 * @param end_range The end of the range.
			 * @param direction The direction to iterate (forward or reverse).
			 */
			constexpr iterator(container_type& container, 
				index_type start_range, 
				index_type end_range, 
				bool forward = true)
				: container_(container), 
				start_range_(start_range), 
				end_range_(end_range), 
				forward_(forward)
			{
				if (forward_)
				{
					current_ = container_.begin();
					while (current_ != container_.end() && !overlaps(*current_, start_range_, end_range_))
					{
						++current_;
					}
				}
				else
				{
					current_ = container_.begin();
					// Find the first overlapping bucket
					while (current_ != container_.end() && !overlaps(*current_, start_range_, end_range_))
					{
						++current_;
					}
					// If no overlapping bucket found, set to end
					if (current_ == container_.end())
					{
						return;
					}
					// Find the last overlapping bucket
					auto next = current_;
					++next;
					while (next != container_.end() && overlaps(*next, start_range_, end_range_))
					{
						++next;
					}
					current_ = next;
				}
			}

			/**
			 * @brief Pre-increment operator.
			 * @return Reference to this iterator.
			 */
			constexpr iterator& operator++()
			{
				if (forward_)
				{
					++current_;
					while (current_ != container_.end() && !overlaps(*current_, start_range_, end_range_))
					{
						++current_;
					}
				}
				else
				{
					if (current_ != container_.begin())
					{
						--current_;
						while (current_ != container_.begin() && !overlaps(*current_, start_range_, end_range_))
						{
							--current_;
						}
					}
				}
				return *this;
			}

			/**
			 * @brief Pre-decrement operator.
			 * @return Reference to this iterator.
			 */
			constexpr iterator& operator--()
			{
				if (forward_)
				{
					if (current_ != container_.begin())
					{
						--current_;
						while (current_ != container_.begin() && !overlaps(*current_, start_range_, end_range_))
						{
							--current_;
						}
					}
				}
				else
				{
					--current_;
					while (current_ != container_.begin() && !overlaps(*current_, start_range_, end_range_))
					{
						--current_;
					}
				}
				return *this;
			}

			/**
			 * @brief Equality operator.
			 * @param other The other iterator to compare with.
			 * @return True if the iterators are equal, false otherwise.
			 */
			[[nodiscard]] constexpr bool operator==(const iterator& other) const noexcept 
			{ 
				return current_ == other.current_; 
			}

			/**
			 * @brief Inequality operator.
			 * @param other The other iterator to compare with.
			 * @return True if the iterators are not equal, false otherwise.
			 */
			[[nodiscard]] constexpr bool operator!=(const iterator& other) const noexcept 
			{ 
				return !(*this == other); 
			}

			/**
			 * @brief Dereference operator.
			 * @return Reference to the current bucket.
			 */
			[[nodiscard]] constexpr reference operator*() noexcept 
			{ 
				return *current_; 
			}

			/**
			 * @brief Arrow operator.
			 * @return Pointer to the current bucket.
			 */
			[[nodiscard]] constexpr pointer operator->() noexcept 
			{ 
				return &(*current_); 
			}

		private:
			/**
			 * @brief Check if a bucket overlaps with a range.
			 * @param bucket The bucket to check.
			 * @param start_range The start of the range.
			 * @param end_range The end of the range.
			 * @return True if the bucket overlaps with the range, false otherwise.
			 */
			template<typename T>
			[[nodiscard]] static constexpr bool overlaps(const T& bucket, 
				index_type start_range, 
				index_type end_range)
			{
				return container_type::accessor::low(bucket) < end_range && 
					container_type::accessor::high(bucket) > start_range;
			}

			container_type& container_;
			iterator_type current_;
			index_type start_range_;
			index_type end_range_;
			bool forward_;
		};

		/**
		 * @brief Constructor.
		 * @param container The container to iterate over.
		 * @param start_range The start of the range.
		 * @param end_range The end of the range.
		 */
		constexpr bucket_range(container_type& container, 
			index_type start_range, 
			index_type end_range)
			: container_(container), 
			start_range_(start_range), 
			end_range_(end_range)
		{
		}

		/**
		 * @brief Get the beginning iterator.
		 * @return Iterator to the beginning of the range.
		 */
		[[nodiscard]] constexpr iterator begin() const noexcept 
		{ 
			return iterator(container_, start_range_, end_range_, true); 
		}

		/**
		 * @brief Get the end iterator.
		 * @return Iterator to the end of the range.
		 */
		[[nodiscard]] constexpr iterator end() const noexcept 
		{ 
			return iterator(container_, start_range_, end_range_, false); 
		}

		/**
		 * @brief Get the reverse beginning iterator.
		 * @return Iterator to the beginning of the reversed range.
		 */
		[[nodiscard]] constexpr iterator rbegin() const noexcept 
		{ 
			return iterator(container_, start_range_, end_range_, true); 
		}

		/**
		 * @brief Get the reverse end iterator.
		 * @return Iterator to the end of the reversed range.
		 */
		[[nodiscard]] constexpr iterator rend() const noexcept 
		{ 
			return iterator(container_, start_range_, end_range_, false); 
		}

	private:
		container_type& container_;
		index_type start_range_;
		index_type end_range_;
	};
}

#endif // MASUTILS_BUCKET_RANGE_H_ 