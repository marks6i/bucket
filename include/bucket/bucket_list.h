/**
 * @file  bucket_list.h
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
 * @brief The bucket_list class.
 *
 * A bucket is a collection much like a Dictionary (key-value store),
 * where the "keys" represent a non-overlapping range on an ordered axis.
 * Gaps are allowed if there are not values in the given range. The "values"
 * are stored in a collection as well, and can be accessed by the key.
 */

#ifndef MASUTILS_BUCKET_LIST_H_
#define MASUTILS_BUCKET_LIST_H_

#include <functional>
#include <list>
#include <stdexcept>
#include <type_traits>
#include <concepts>
#include <ranges>
#include <span>

#include "bucket_compare_traits.h"
#include "bucket_value_traits.h"

namespace masutils
{ 
	/**
	 * @brief The bucket_list class.
	 * @tparam Indices The type of the keys in the bucket.
	 * @tparam Values The type of the values in the bucket.
	 * @tparam Traits The operations that can be performed on the keys.
	 * @tparam ContainerTraits The operations that can be performed on the value container.
	 */
	template <class Indices,
	          class Values,
	          class Traits = bucket_compare_traits<Indices>,
	          class ContainerTraits = bucket_value_traits<Values>>
	requires std::totally_ordered<Indices> && std::equality_comparable<Indices>
	class bucket_list
	{
	public:
		using mytype = bucket_list<Indices,
		                          Values,
		                          Traits,
		                          ContainerTraits>;

		using index_type = Indices;
		using value_type = Values;

		using value_container = typename ContainerTraits::value_container;
		using const_value_container = const typename ContainerTraits::value_container;

		using bucket_type = std::pair<index_type,
		                             std::pair<index_type,
		                                      value_container>>;
		using bucket_type_list = std::list<bucket_type>;

		struct accessor {
			accessor() = delete;

			// Getters
			[[nodiscard]] static constexpr index_type& low(bucket_type& t) noexcept                     { return t.first;         }
			[[nodiscard]] static constexpr const index_type& low(const bucket_type& t) noexcept         { return t.first;         }
			[[nodiscard]] static constexpr index_type& high(bucket_type& t) noexcept                    { return t.second.first;  }
			[[nodiscard]] static constexpr const index_type& high(const bucket_type& t) noexcept        { return t.second.first;  }
			[[nodiscard]] static constexpr value_container& values(bucket_type& t) noexcept             { return t.second.second; }
			[[nodiscard]] static constexpr const_value_container& values(const bucket_type& t) noexcept { return t.second.second; }
		};

		[[nodiscard]] static constexpr bucket_type make_bucket(index_type low, index_type high, const value_container& values)
		{
			return std::make_pair(low, std::make_pair(high, values));
		}

		using iterator = typename bucket_type_list::iterator;
		using const_iterator = typename bucket_type_list::const_iterator;
		using reverse_iterator = typename bucket_type_list::reverse_iterator;
		using const_reverse_iterator = typename bucket_type_list::const_reverse_iterator;

		/**
		 * @brief Returns an iterator to the first element of the bucket collection.
		 * @return Iterator to the first element.
		 */
		[[nodiscard]] iterator begin() noexcept { return buckets_.begin(); }

		/**
		 * @brief Returns an iterator to the element following the last element of the bucket collection.
		 * @return Iterator to the element following the last element.
		 */
		[[nodiscard]] iterator end() noexcept { return buckets_.end(); }

		/**
		 * @brief Returns a const iterator to the first element of the bucket collection.
		 * @return Const iterator to the first element.
		 */
		[[nodiscard]] const_iterator begin() const noexcept { return const_iterator(buckets_.begin()); }

		/**
		 * @brief Returns a const iterator to the element following the last element of the bucket collection.
		 * @return Const iterator to the element following the last element.
		 */
		[[nodiscard]] const_iterator end() const noexcept { return const_iterator(buckets_.end()); }

		/**
		 * @brief Returns a reverse iterator to the first element of the reversed bucket collection. It corresponds to the last element of the non-reversed bucket collection.
		 * @return Reverse iterator to the first element.
		 */
		[[nodiscard]] reverse_iterator rbegin() noexcept { return buckets_.rbegin(); }

		/**
		 * @brief Returns a reverse iterator to the element following the last element of the reversed bucket collection. It corresponds to the element preceding the first element of the non-reversed bucket collection.
		 * @return Reverse iterator to the element following the last element.
		 */
		[[nodiscard]] reverse_iterator rend() noexcept { return buckets_.rend(); }

		/**
		 * @brief Returns a const reverse iterator to the first element of the reversed bucket collection. It corresponds to the last element of the non-reversed bucket collection.
		 * @return Const reverse iterator to the first element.
		 */
		[[nodiscard]] const_reverse_iterator rbegin() const noexcept
		{
			return const_reverse_iterator(buckets_.rbegin());
		}

		/**
		 * @brief Returns a const reverse iterator to the element following the last element of the reversed bucket collection. It corresponds to the element preceding the first element of the non-reversed bucket collection.
		 * @return Const reverse iterator to the element following the last element.
		 */
		[[nodiscard]] const_reverse_iterator rend() const noexcept
		{
			return const_reverse_iterator(buckets_.rend());
		}

	private:
		enum class iteration_direction { forward, reverse };

		// Define the range_iterator
		template <bool IsConst>
		class range_iterator
		{
			using parent_list = typename std::conditional_t<IsConst, const bucket_type_list, bucket_type_list>;
			using iterator_range = typename std::conditional_t<IsConst, const_iterator, iterator>;

			iterator_range current_;
			iterator_range begin_;
			iterator_range end_;
			index_type start_range_;
			index_type end_range_;
			iteration_direction direction_;

		private:
			[[nodiscard]] static constexpr bool overlaps(const bucket_type& bucket, index_type start_range, index_type end_range)
			{
				return !(accessor::low(bucket) >= end_range || accessor::high(bucket) < start_range);
			}

		public:
			constexpr range_iterator(parent_list& list, index_type start_range, index_type end_range, iteration_direction direction)
				: begin_(list.begin()), end_(list.end()), start_range_(start_range), end_range_(end_range), direction_(direction)
			{
				if (direction_ == iteration_direction::forward)
				{
					current_ = begin_;
					while (current_ != end_ && !overlaps(*current_, start_range_, end_range_))
					{
						++current_;
					}
				}
				else
				{
					current_ = end_;
					if (current_ != begin_)
					{
						do
						{
							--current_;
							if (overlaps(*current_, start_range_, end_range_))
							{
								++current_;
								break;
							}
						} while (current_ != begin_);
					}
				}
			}

			constexpr range_iterator& operator++()
			{
				if (direction_ == iteration_direction::forward)
				{
					do
					{
						++current_;
					}
					while (current_ != end_ && !overlaps(*current_, start_range_, end_range_));
				}
				return *this;
			}

			constexpr range_iterator& operator--()
			{
				if (direction_ == iteration_direction::reverse)
				{
					if (current_ != begin_)
					{
						do
						{
							--current_;
						}
						while (current_ != begin_ && !overlaps(*current_, start_range_, end_range_));
					}
				}
				return *this;
			}

			[[nodiscard]] constexpr bool operator==(const range_iterator& other) const noexcept { return current_ == other.current_; }
			[[nodiscard]] constexpr bool operator!=(const range_iterator& other) const noexcept { return !(*this == other); }

			[[nodiscard]] constexpr bucket_type& operator*() noexcept { return *current_; }
			[[nodiscard]] constexpr bucket_type* operator->() noexcept { return &(*current_); }
		};

	public:
		// Forward iterators

		/**
		 * @brief Forward iterator over a range of buckets.
		 * @tparam IsConst Boolean value indicating if the iterator is const.
		 * @param start_range First bucket after or containing this index.
		 * @param end_range First bucket after this index.
		 * @return Iterator starting at first bucket in the range.
		 */
		template <bool IsConst>
		[[nodiscard]] range_iterator<IsConst> beginRange(index_type start_range, index_type end_range)
		{
			return range_iterator<IsConst>(buckets_, start_range, end_range, iteration_direction::forward);
		}

		/**
		 * @brief Iterator of first bucket past the range.
		 * @tparam IsConst Boolean value indicating if the iterator is const.
		 * @param start_range Start index of buckets in the range.
		 * @param end_range End index of buckets in the range.
		 * @return Iterator to first bucket past the range.
		 */
		template <bool IsConst>
		[[nodiscard]] range_iterator<IsConst> endRange(index_type start_range, index_type end_range) {
			range_iterator<IsConst> iter(buckets_, start_range, end_range, iteration_direction::forward);
			while (iter.current_ != iter.end_ && overlaps(*iter.current_, start_range, end_range)) {
				++iter.current_;
			}
			return iter;
		}

		// Reverse iterators

		/**
		 * @brief Forward iterator over a range of buckets.
		 * @tparam IsConst Boolean value indicating if the iterator is const.
		 * @param start_range First bucket in reverse order after or containing this index.
		 * @param end_range First bucket after this index.
		 * @return Iterator starting at first bucket in reverse order in the range.
		 */
		template <bool IsConst>
		[[nodiscard]] range_iterator<IsConst> rbeginRange(index_type start_range, index_type end_range)
		{
			return range_iterator<IsConst>(buckets_, start_range, end_range, iteration_direction::reverse);
		}

		/**
		 * @brief Iterator of first bucket in reverse order past the range.
		 * @tparam IsConst Boolean value indicating if the iterator is const.
		 * @param start_range Start index of buckets in the range.
		 * @param end_range End index of buckets in the range.
		 * @return Iterator to first bucket in reverse order past the range.
		 */
		template <bool IsConst>
		[[nodiscard]] range_iterator<IsConst> rendRange(index_type start_range, index_type end_range) {
			range_iterator<IsConst> iter(buckets_, start_range, end_range, iteration_direction::reverse);
			while (iter.current_ != iter.begin_ && overlaps(*iter.current_, start_range, end_range)) {
				--iter.current_;
			}
			return iter;
		}

		[[nodiscard]] constexpr std::size_t size() const noexcept { return buckets_.size(); }
		[[nodiscard]] constexpr bool empty() const noexcept { return buckets_.empty(); }
		[[nodiscard]] constexpr index_type low() const noexcept { return low_; }
		[[nodiscard]] constexpr index_type high() const noexcept { return high_; }
		[[nodiscard]] constexpr bool constrained() const noexcept { return constrained_; }

	private:
		bucket_list(const mytype&) = default;
		mytype& operator=(const mytype&) = default;

		bucket_type_list buckets_;
		const index_type low_;
		const index_type high_;
		const bool constrained_;

	public:
		/**
		 * @brief Returns true if buckets is constrained.
		 */
		[[nodiscard]] constexpr bool is_constrained() const noexcept { return constrained_; }

		/**
		 * @brief Returns the lower bound of a constrained buckets
		 * or a run-time exception if not constrained.
		 */
		[[nodiscard]] index_type lower_bound() const {
			if (!constrained_) {
				throw std::runtime_error("Bounds are not constrained.");
			}
			return low_;
		}

		/**
		 * @brief Returns the upper bound of a constrained buckets
		 * or a run-time exception if not constrained.
		 */
		[[nodiscard]] index_type upper_bound() const {
			if (!constrained_) {
				throw std::runtime_error("Bounds are not constrained.");
			}
			return high_;
		}

		/**
		 * @brief Constructor of a constrained buckets collection.
		 * @param low Lower bounds of the buckets collection.
		 * @param high Upper bounds of the buckets collection.
		 */
		explicit bucket_list(index_type low, index_type high) : low_(low), high_(high), constrained_(true)
		{
			if (Traits::lt(high_, low_))
				throw std::invalid_argument("Arguments not in correct order.");
		}

		/**
		 * @brief Constructor of an unconstrained buckets collection.
		 */
		explicit bucket_list() noexcept(
			std::is_nothrow_default_constructible<bucket_type>::value &&
			std::is_nothrow_default_constructible<index_type>::value &&
			noexcept(false)
			) : low_(), high_(), constrained_(false)
		{
		}

		/**
		 * @brief Default destructor.
		 */
		~bucket_list() = default;

		/**
		 * @brief Default move constructor.
		 * @param  Original buckets collection.
		 * @return New buckets collection.
		 */
		bucket_list& operator=(bucket_list&&) noexcept = default;

	protected:
		[[nodiscard]] bool splice(index_type low, index_type high, iterator& begin, iterator& end)
		{
			index_type l, h;
			Traits::assign(l, low);
			Traits::assign(h, high);

			if (constrained_)
			{
				if (Traits::lt(h, low_) || Traits::lt(high_, l))
					return false;

				if (Traits::lt(l, low_)) Traits::assign(l, low_);
				if (Traits::lt(high_, h)) Traits::assign(h, high_);
			}

			index_type lowest_, highest_;
			Traits::assign(lowest_, l);
			Traits::assign(highest_, h);

			for (iterator p = buckets_.begin(); p != buckets_.end(); ++p)
			{
				if (Traits::lt(l, h) != true)
					break;

				bucket_type& bucket = *p;

				if (Traits::lt(l, accessor::low(bucket)))
				{
					value_container container_;
					if (Traits::lt(accessor::low(bucket), h))
					{
						bucket_type _bucket = make_bucket(l, accessor::low(bucket), container_);
						buckets_.insert(p, _bucket);
						Traits::assign(l, accessor::low(bucket));
					}
					else
					{
						bucket_type _bucket = make_bucket(l, h, container_);
						buckets_.insert(p, _bucket);
						Traits::assign(l, accessor::low(bucket));
						continue;
					}
				}

				if (Traits::eq(l, accessor::low(bucket)))
				{
					if (Traits::lt(h, accessor::high(bucket)))
					{
						bucket_type bucket_(bucket);
						Traits::assign(accessor::high(bucket_), h);
						buckets_.insert(p, bucket_);
						Traits::assign(accessor::low(bucket), h);
						Traits::assign(l, h);
						continue;
					}
					else
					{
						Traits::assign(l, accessor::high(bucket));
					}
				}

				if (Traits::lt(l, accessor::high(bucket)))
				{
					{
						bucket_type bucket_(bucket);
						Traits::assign(accessor::high(bucket_), l);
						buckets_.insert(p, bucket_);
						Traits::assign(accessor::low(bucket), l);
					}

					if (Traits::lt(h, accessor::high(bucket)))
					{
						bucket_type bucket_(bucket);
						Traits::assign(accessor::high(bucket_), h);
						buckets_.insert(p, bucket_);
						Traits::assign(accessor::low(bucket), h);
					}

					Traits::assign(l, accessor::high(bucket));
				}
			}

			if (Traits::lt(l, h))
			{
				value_container container_;
				bucket_type _bucket = make_bucket(l, h, container_);
				buckets_.push_back(_bucket);
			}

			bool b_begin = false, b_end = false;

			{
				for (iterator p = buckets_.begin(); p != buckets_.end(); ++p)
				{
					const bucket_type& bucket = *p;
					if (Traits::eq(lowest_, accessor::low(bucket)))
					{
						begin = p;
						b_begin = true;
					}
					if (Traits::eq(highest_, accessor::high(bucket)))
					{
						end = p;
						++end;
						b_end = true;
						break;
					}
				}
			}

			return (b_begin && b_end);
		}

		[[nodiscard]] int spread(const bucket_type& bucket_)
		{
			int added_to_bucket = 0;

			iterator begin, end;
			const bool b_spliced = splice(accessor::low(bucket_), accessor::high(bucket_), begin, end);

			if (!b_spliced)
				return added_to_bucket;

			index_type l, h;
			Traits::assign(l, accessor::low(bucket_));
			Traits::assign(h, accessor::high(bucket_));

			if (constrained_)
			{
				if (Traits::lt(l, low_)) Traits::assign(l, low_);
				if (Traits::lt(high_, h)) Traits::assign(h, high_);
			}

			for (iterator p = begin; p != end; ++p)
			{
				bucket_type& bucket = *p;
				if (Traits::lt(accessor::high(bucket), l)) continue;
				if (Traits::lt(h, accessor::low(bucket))) break;
				value_container& ocontainer_ = accessor::values(bucket);
				const value_container& icontainer_ = accessor::values(bucket_);
				ContainerTraits::append(ocontainer_, icontainer_);
				added_to_bucket++;
			}

			return added_to_bucket;
		}

		[[nodiscard]] int cover(const bucket_type& bucket_)
		{
			int added_to_bucket = 0;

			iterator begin, end;
			const bool b_spliced = splice(accessor::low(bucket_), accessor::high(bucket_), begin, end);

			if (!b_spliced)
				return added_to_bucket;

			index_type l, h;
			Traits::assign(l, accessor::low(bucket_));
			Traits::assign(h, accessor::high(bucket_));

			if (constrained_)
			{
				if (Traits::lt(l, low_)) Traits::assign(l, low_);
				if (Traits::lt(high_, h)) Traits::assign(h, high_);
			}

			iterator next = buckets_.erase(begin, end);

			bucket_type bucket2_ = make_bucket(l, h, accessor::values(bucket_));

			buckets_.insert(next, bucket2_);

			added_to_bucket++;

			return added_to_bucket;
		}

	public:
		/**
		 * @brief Spread \bvalue into all buckets (creating new ones as needed) in the range \blow to \bhigh.
		 * @param low Lower bounds of the range.
		 * @param high Upper bounds of the range.
		 * @param value Value to be added to the value container in each bucket.
		 * @return Number of buckets that the value was added to.
		 */
		[[nodiscard]] int spread(index_type low, index_type high, value_type value)
		{
			value_container container_;
			ContainerTraits::add(container_, value);
			bucket_type bucket_ = make_bucket(low, high, container_);

			return spread(bucket_);
		}

		/**
		 * @brief Create a new bucket with the range \blow to \bhigh (replacing all previous buckets in the range) and add \bvalue to the value container.
		 * @param low Lower bounds of the range.
		 * @param high Upper bounds of the range.
		 * @param value Value to be added to the value container in the bucket.
		 * @return Number of buckets that the value was added to.
		 */
		[[nodiscard]] int cover(index_type low, index_type high, value_type value)
		{
			value_container container_;
			ContainerTraits::add(container_, value);
			bucket_type bucket_ = make_bucket(low, high, container_);

			return cover(bucket_);
		}

		/**
		 * @brief Remove (or partially remove) all buckets in the range \blow to \bhigh.
		 * @param low Lower bounds of the range.
		 * @param high Upper bounds of the range.
		 * @return Boolean indicating if any buckets were removed.
		 */
		[[nodiscard]] bool erase(index_type low, index_type high)
		{
			iterator begin, end;
			const bool b_spliced = splice(low, high, begin, end);

			if (!b_spliced)
				return false;

			index_type l, h;
			Traits::assign(l, low);
			Traits::assign(h, high);

			if (constrained_)
			{
				if (Traits::lt(l, low_)) Traits::assign(l, low_);
				if (Traits::lt(high_, h)) Traits::assign(h, high_);
			}

			iterator next = buckets_.erase(begin, end);

			return true;
		}

		/**
		 * @brief Repeated spread each element of a bucket into another bucket.
		 * @tparam OtherContainerTraits The container traits of the passed bucket.
		 * @param bucket_ The bucket to spread.
		 * @return Number of buckets that all the values were added to.
		 */
		template <class OtherContainerTraits>
		[[nodiscard]] int spread(const bucket_list<Indices, Values, Traits, OtherContainerTraits>& bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const bucket_type& bucket = *p;
				added_to_bucket += spread(bucket);
			}

			return added_to_bucket;
		}

		/**
		 * @brief Cover a bucket with the buckets in another bucket collection.
		 * @tparam OtherContainerTraits the container traits of the passed bucket.
		 * @param bucket_ the bucket used to cover.
		 * @return Th number of buckets that all the values were added to.
		 */
		template <class OtherContainerTraits>
		[[nodiscard]] int cover(const bucket_list<Indices, Values, Traits, OtherContainerTraits>& bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const bucket_type& bucket = *p;
				added_to_bucket += cover(bucket);
			}

			return added_to_bucket;
		}
	};
}

#endif // MASUTILS_BUCKET_LIST_H_
