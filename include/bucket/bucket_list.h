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
#include "bucket_range.h"
#include "bucket_object.h"

namespace masutils
{ 
	/**
	 * @brief The bucket_list class.
	 * @tparam Indices The type of the keys in the bucket.
	 * @tparam Values The type of the values in the bucket.
	 * @tparam CompareTraits The operations that can be performed on the keys.
	 * @tparam ValueTraits The operations that can be performed on the value container.
	 */
	template <class Indices,
	          class Values,
	          class CompareTraits = bucket_compare_traits<Indices>,
	          class ValueTraits = bucket_value_traits<Values>>
	requires std::totally_ordered<Indices> && std::equality_comparable<Indices>
	class bucket_list
	{
	public:
		using mytype = bucket_list<Indices,
		                Values,
		                CompareTraits,
		                          ValueTraits>;

		using index_type = Indices;
		using value_type = Values;

		// Public container type that represents the actual values
		using value_container = typename ValueTraits::value_container;
		using const_value_container = const typename ValueTraits::value_container;

		// Define the bucket type using the new bucket_object class
		using bucket_type = bucket_object<index_type, value_container>;
		
		// Use a list to store the buckets
		using bucket_type_list = std::list<bucket_type>;

		struct accessor {
			accessor() = delete;

			// Getters
			template<typename T>
			[[nodiscard]] static constexpr auto& low(T& t) noexcept                     { return t.low(); }
			template<typename T>
			[[nodiscard]] static constexpr const auto& low(const T& t) noexcept         { return t.low(); }
			template<typename T>
			[[nodiscard]] static constexpr auto& high(T& t) noexcept                    { return t.high(); }
			template<typename T>
			[[nodiscard]] static constexpr const auto& high(const T& t) noexcept        { return t.high(); }
			template<typename T>
			[[nodiscard]] static constexpr auto& values(T& t) noexcept                  { return t.values(); }
			template<typename T>
			[[nodiscard]] static constexpr const auto& values(const T& t) noexcept      { return t.values(); }
		};

		[[nodiscard]] static constexpr bucket_type make_bucket(index_type low, index_type high, const value_container& values)
		{
			return bucket_type(low, high, values);
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

		public:
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = typename std::conditional<IsConst, const bucket_type, bucket_type>::type;
			using difference_type = typename std::iterator_traits<iterator>::difference_type;
			using pointer = value_type*;
			using reference = value_type&;

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
					while (current_ != begin_)
					{
						--current_;
						if (overlaps(*current_, start_range_, end_range_))
						{
							break;
						}
					}
				}
			}

			constexpr range_iterator& operator++()
			{
				if (direction_ == iteration_direction::forward)
				{
					++current_;
					while (current_ != end_ && !overlaps(*current_, start_range_, end_range_))
					{
						++current_;
					}
				}
				else
				{
					if (current_ != begin_)
					{
						--current_;
						while (current_ != begin_ && !overlaps(*current_, start_range_, end_range_))
						{
							--current_;
						}
					}
				}
				return *this;
			}

			constexpr range_iterator& operator--()
			{
				if (direction_ == iteration_direction::forward)
				{
					if (current_ != begin_)
					{
						--current_;
						while (current_ != begin_ && !overlaps(*current_, start_range_, end_range_))
						{
							--current_;
						}
					}
				}
				else
				{
					--current_;
					while (current_ != begin_ && !overlaps(*current_, start_range_, end_range_))
					{
						--current_;
					}
				}
				return *this;
			}

			[[nodiscard]] constexpr bool operator==(const range_iterator& other) const noexcept { return current_ == other.current_; }
			[[nodiscard]] constexpr bool operator!=(const range_iterator& other) const noexcept { return !(*this == other); }

			// Add comparison operators for the underlying iterator type
			[[nodiscard]] constexpr bool operator==(const iterator_range& other) const noexcept { return current_ == other; }
			[[nodiscard]] constexpr bool operator!=(const iterator_range& other) const noexcept { return !(*this == other); }

			// Add friend operators for reverse comparison
			[[nodiscard]] friend constexpr bool operator==(const iterator_range& lhs, const range_iterator& rhs) noexcept { return rhs == lhs; }
			[[nodiscard]] friend constexpr bool operator!=(const iterator_range& lhs, const range_iterator& rhs) noexcept { return rhs != lhs; }

			// Add getter for current position
			[[nodiscard]] constexpr iterator_range current() const noexcept { return current_; }

			[[nodiscard]] constexpr bucket_type& operator*() noexcept requires (!IsConst) { return *current_; }
			[[nodiscard]] constexpr bucket_type* operator->() noexcept requires (!IsConst) { return &(*current_); }
			[[nodiscard]] constexpr const bucket_type& operator*() const noexcept requires IsConst { return *std::as_const(current_); }
			[[nodiscard]] constexpr const bucket_type* operator->() const noexcept requires IsConst { return &(*std::as_const(current_)); }

			[[nodiscard]] static constexpr bool overlaps(const bucket_type& bucket, index_type start_range, index_type end_range)
			{
				return !(accessor::low(bucket) >= end_range || accessor::high(bucket) < start_range);
			}

		private:
			iterator_range current_;
			iterator_range begin_;
			iterator_range end_;
			index_type start_range_;
			index_type end_range_;
			iteration_direction direction_;
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
			while (iter != buckets_.end() && range_iterator<IsConst>::overlaps(*iter, start_range, end_range)) {
				++iter;
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
			while (iter.current() != buckets_.begin() && range_iterator<IsConst>::overlaps(*iter, start_range, end_range)) {
				--iter;
			}
			return iter;
		}

		// Add comparison operators for range_iterator
		template <bool IsConst>
		[[nodiscard]] friend constexpr bool operator==(const range_iterator<IsConst>& lhs, const range_iterator<IsConst>& rhs) noexcept {
			return lhs.current_ == rhs.current_;
		}

		template <bool IsConst>
		[[nodiscard]] friend constexpr bool operator!=(const range_iterator<IsConst>& lhs, const range_iterator<IsConst>& rhs) noexcept {
			return !(lhs == rhs);
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
			if (CompareTraits::lt(high_, low_))
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
			CompareTraits::assign(l, low);
			CompareTraits::assign(h, high);

			if (constrained_)
			{
				if (CompareTraits::lt(h, low_) || CompareTraits::lt(high_, l))
					return false;

				if (CompareTraits::lt(l, low_)) CompareTraits::assign(l, low_);
				if (CompareTraits::lt(high_, h)) CompareTraits::assign(h, high_);
			}

			index_type lowest_, highest_;
			CompareTraits::assign(lowest_, l);
			CompareTraits::assign(highest_, h);

			for (iterator p = buckets_.begin(); p != buckets_.end(); ++p)
			{
				if (CompareTraits::lt(l, h) != true)
					break;

				bucket_type& bucket = *p;

				if (CompareTraits::lt(l, accessor::low(bucket)))
				{
					value_container container_;
					if (CompareTraits::lt(accessor::low(bucket), h))
					{
						bucket_type _bucket = make_bucket(l, accessor::low(bucket), container_);
						buckets_.insert(p, _bucket);
						CompareTraits::assign(l, accessor::low(bucket));
					}
					else
					{
						bucket_type _bucket = make_bucket(l, h, container_);
						buckets_.insert(p, _bucket);
						CompareTraits::assign(l, accessor::low(bucket));
						continue;
					}
				}

				if (CompareTraits::eq(l, accessor::low(bucket)))
				{
					if (CompareTraits::lt(h, accessor::high(bucket)))
					{
						bucket_type bucket_(bucket);
						CompareTraits::assign(accessor::high(bucket_), h);
						buckets_.insert(p, bucket_);
						CompareTraits::assign(accessor::low(bucket), h);
						CompareTraits::assign(l, h);
						continue;
					}
					else
					{
						CompareTraits::assign(l, accessor::high(bucket));
					}
				}

				if (CompareTraits::lt(l, accessor::high(bucket)))
				{
					{
						bucket_type bucket_(bucket);
						CompareTraits::assign(accessor::high(bucket_), l);
						buckets_.insert(p, bucket_);
						CompareTraits::assign(accessor::low(bucket), l);
					}

					if (CompareTraits::lt(h, accessor::high(bucket)))
					{
						bucket_type bucket_(bucket);
						CompareTraits::assign(accessor::high(bucket_), h);
						buckets_.insert(p, bucket_);
						CompareTraits::assign(accessor::low(bucket), h);
					}

					CompareTraits::assign(l, accessor::high(bucket));
				}
			}

			if (CompareTraits::lt(l, h))
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
					if (CompareTraits::eq(lowest_, accessor::low(bucket)))
					{
						begin = p;
						b_begin = true;
					}
					if (CompareTraits::eq(highest_, accessor::high(bucket)))
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
			CompareTraits::assign(l, accessor::low(bucket_));
			CompareTraits::assign(h, accessor::high(bucket_));

			if (constrained_)
			{
				if (CompareTraits::lt(l, low_)) CompareTraits::assign(l, low_);
				if (CompareTraits::lt(high_, h)) CompareTraits::assign(h, high_);
			}

			for (iterator p = begin; p != end; ++p)
			{
				bucket_type& bucket = *p;
				if (CompareTraits::lt(accessor::high(bucket), l)) continue;
				if (CompareTraits::lt(h, accessor::low(bucket))) break;
				value_container& ocontainer_ = accessor::values(bucket);
				const value_container& icontainer_ = accessor::values(bucket_);
				ValueTraits::append(ocontainer_, icontainer_);
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
			CompareTraits::assign(l, accessor::low(bucket_));
			CompareTraits::assign(h, accessor::high(bucket_));

			if (constrained_)
			{
				if (CompareTraits::lt(l, low_)) CompareTraits::assign(l, low_);
				if (CompareTraits::lt(high_, h)) CompareTraits::assign(h, high_);
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
			ValueTraits::add(container_, value);
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
			ValueTraits::add(container_, value);
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
			CompareTraits::assign(l, low);
			CompareTraits::assign(h, high);

			if (constrained_)
			{
				if (CompareTraits::lt(l, low_)) CompareTraits::assign(l, low_);
				if (CompareTraits::lt(high_, h)) CompareTraits::assign(h, high_);
			}

			iterator next = buckets_.erase(begin, end);

			return true;
		}

		/**
		 * @brief Repeated spread each element of a bucket into another bucket.
		 * @tparam OtherValueTraits The container traits of the passed bucket.
		 * @param bucket_ The bucket to spread.
		 * @return Number of buckets that all the values were added to.
		 */
		template <class OtherValueTraits>
		[[nodiscard]] int spread(const bucket_list<Indices, Values, CompareTraits, OtherValueTraits>& bucket_)
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
		 * @tparam OtherValueTraits the container traits of the passed bucket.
		 * @param bucket_ the bucket used to cover.
		 * @return Th number of buckets that all the values were added to.
		 */
		template <class OtherValueTraits>
		[[nodiscard]] int cover(const bucket_list<Indices, Values, CompareTraits, OtherValueTraits>& bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const bucket_type& bucket = *p;
				added_to_bucket += cover(bucket);
			}

			return added_to_bucket;
		}

		// Add a method to create a bucket_range
		bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, false> range(Indices start, Indices end) {
			return bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, false>(*this, start, end);
		}

		// Add a const method to create a bucket_range
		bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, true> range(Indices start, Indices end) const {
			return bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, true>(*this, start, end);
		}
	};
}

#endif // MASUTILS_BUCKET_LIST_H_
