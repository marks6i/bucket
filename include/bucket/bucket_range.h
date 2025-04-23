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
 * in a bucket collection (bucket_list or bucket_map). The iterators support
 * both forward and reverse iteration, with proper handling of end conditions
 * and range boundaries. When iterating, only buckets that overlap with the
 * specified range are included in the iteration.
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
		 * 
		 * The iterator supports both forward and reverse iteration over buckets
		 * that overlap with the specified range. When iterating forward, buckets
		 * are visited in ascending order of their low values. When iterating in
		 * reverse, buckets are visited in descending order of their high values.
		 * 
		 * End conditions are handled consistently: an iterator is considered at
		 * the end when either:
		 * 1. It has reached the end of the container
		 * 2. No more buckets overlap with the specified range
		 * 3. For reverse iteration, it has reached the beginning of the container
		 */
		class iterator
		{
		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = typename std::conditional<IsConst, const bucket_type, bucket_type>::type;
			using difference_type = typename std::iterator_traits<iterator_type>::difference_type;
			using pointer = value_type*;
			using reference = typename std::iterator_traits<iterator_type>::reference;

			friend class bucket_range;

			/**
			 * @brief Constructor.
			 * @param container The container to iterate over.
			 * @param start_range The start of the range.
			 * @param end_range The end of the range.
			 * @param forward If true, iterate forward (ascending order);
			 *              if false, iterate in reverse (descending order).
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
					current_ = container_.end();
					if (current_ != container_.begin())
					{
						--current_;
						while (current_ != container_.begin() && !overlaps(*current_, start_range_, end_range_))
						{
							--current_;
						}
						if (!overlaps(*current_, start_range_, end_range_))
						{
							current_ = container_.end();
						}
					}
				}
			}

			/**
			 * @brief Pre-increment operator.
			 * @return Reference to this iterator.
			 * 
			 * For forward iteration, moves to the next overlapping bucket.
			 * For reverse iteration, moves to the previous overlapping bucket.
			 * Sets the iterator to the end if no more overlapping buckets are found.
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
						if (!overlaps(*current_, start_range_, end_range_))
						{
							current_ = container_.end();
						}
					}
					else
					{
						current_ = container_.end();
					}
				}
				return *this;
			}

			/**
			 * @brief Pre-decrement operator.
			 * @return Reference to this iterator.
			 * 
			 * For forward iteration, moves to the previous overlapping bucket.
			 * For reverse iteration, moves to the next overlapping bucket.
			 * Sets the iterator to the end if no more overlapping buckets are found.
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
						if (!overlaps(*current_, start_range_, end_range_))
						{
							current_ = container_.end();
						}
					}
				}
				else
				{
					++current_;
					while (current_ != container_.end() && !overlaps(*current_, start_range_, end_range_))
					{
						++current_;
					}
				}
				return *this;
			}

			/**
			 * @brief Equality operator.
			 * @param other The other iterator to compare with.
			 * @return True if the iterators are equal, false otherwise.
			 * 
			 * Two iterators are considered equal if:
			 * 1. Both are at the end (current_ == container_.end())
			 * 2. Both point to the same bucket and have the same iteration direction
			 */
			[[nodiscard]] constexpr bool operator==(const iterator& other) const noexcept 
			{ 
				if (current_ == container_.end() && other.current_ == container_.end())
				{
					return true;
				}
				if (current_ == container_.end() || other.current_ == container_.end())
				{
					return false;
				}
				return current_ == other.current_ && forward_ == other.forward_;
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

		protected:
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
		 * @return Iterator to the first overlapping bucket in ascending order.
		 */
		[[nodiscard]] constexpr iterator begin() const noexcept 
		{ 
			return iterator(container_, start_range_, end_range_, true); 
		}

		/**
		 * @brief Get the end iterator.
		 * @return Iterator marking the end of forward iteration.
		 */
		[[nodiscard]] constexpr iterator end() const noexcept 
		{ 
			iterator it(container_, start_range_, end_range_, true);
			it.current_ = container_.end();
			return it;
		}

		/**
		 * @brief Get the reverse beginning iterator.
		 * @return Iterator to the first overlapping bucket in descending order.
		 */
		[[nodiscard]] constexpr iterator rbegin() const noexcept 
		{ 
			return iterator(container_, start_range_, end_range_, false); 
		}

		/**
		 * @brief Get the reverse end iterator.
		 * @return Iterator marking the end of reverse iteration.
		 */
		[[nodiscard]] constexpr iterator rend() const noexcept 
		{ 
			iterator it(container_, start_range_, end_range_, false);
			it.current_ = container_.end();
			return it;
		}

	private:
		container_type& container_;
		index_type start_range_;
		index_type end_range_;
	};
}

#endif // MASUTILS_BUCKET_RANGE_H_ 