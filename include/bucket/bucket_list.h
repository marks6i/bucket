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
#include <ranges>
#include <span>
#include <concepts>

#include "bucket_compare_traits.h"
#include "bucket_value_traits.h"
#include "bucket_range.h"
#include "bucket_object.h"

namespace masutils
{
	/**
	 * @brief A bucket_list is a collection of buckets, where each bucket
	 * represents a non-overlapping range on an ordered axis.
	 *
	 * @tparam Indices The type of the indices used to define the ranges.
	 * @tparam Values The type of the values stored in the buckets.
	 * @tparam CompareTraits The traits class used to define the comparison operations.
	 * @tparam ValueTraits The traits class used to define the value type and its
	 * operations.
	 */
	template <typename Indices, typename Values, 
						typename CompareTraits = bucket_compare_traits<Indices>,
						typename ValueTraits = bucket_value_traits<Values>>
	class bucket_list
	{
		static_assert(std::is_arithmetic_v<Indices> || 
						(std::is_class_v<Indices> && 
						 std::is_convertible_v<decltype(CompareTraits::lt(std::declval<Indices>(), std::declval<Indices>())), bool> &&
						 std::is_convertible_v<decltype(CompareTraits::eq(std::declval<Indices>(), std::declval<Indices>())), bool>),
						"Indices must be either an arithmetic type or a class type that supports CompareTraits operations");

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

		[[nodiscard]] static constexpr bucket_type make_bucket(index_type low, index_type high, const value_container &values)
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

	public:
		[[nodiscard]] constexpr std::size_t size() const noexcept { return buckets_.size(); }
		[[nodiscard]] constexpr bool empty() const noexcept { return buckets_.empty(); }
		[[nodiscard]] constexpr index_type low() const noexcept { return low_; }
		[[nodiscard]] constexpr index_type high() const noexcept { return high_; }
		[[nodiscard]] constexpr bool constrained() const noexcept { return constrained_; }

	private:
		bucket_list(const mytype &) = default;
		mytype &operator=(const mytype &) = default;

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
		[[nodiscard]] index_type lower_bound() const
		{
			if (!constrained_)
			{
				throw std::runtime_error("Bounds are not constrained.");
			}
			return low_;
		}

		/**
		 * @brief Returns the upper bound of a constrained buckets
		 * or a run-time exception if not constrained.
		 */
		[[nodiscard]] index_type upper_bound() const
		{
			if (!constrained_)
			{
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
				noexcept(false)) : low_(), high_(), constrained_(false)
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
		bucket_list &operator=(bucket_list &&) noexcept = default;

	protected:
		[[nodiscard]] bool splice(index_type low, index_type high, iterator &begin, iterator &end)
		{
			index_type l, h;
			CompareTraits::assign(l, low);
			CompareTraits::assign(h, high);

			if (constrained_)
			{
				if (CompareTraits::lt(h, low_) || CompareTraits::lt(high_, l))
					return false;

				if (CompareTraits::lt(l, low_))
					CompareTraits::assign(l, low_);
				if (CompareTraits::lt(high_, h))
					CompareTraits::assign(h, high_);
			}

			index_type lowest_, highest_;
			CompareTraits::assign(lowest_, l);
			CompareTraits::assign(highest_, h);

			for (iterator p = buckets_.begin(); p != buckets_.end(); ++p)
			{
				if (CompareTraits::lt(l, h) != true)
					break;

				bucket_type &bucket = *p;

				if (CompareTraits::lt(l, bucket.low()))
				{
					value_container container_;
					if (CompareTraits::lt(bucket.low(), h))
					{
						bucket_type _bucket = make_bucket(l, bucket.low(), container_);
						buckets_.insert(p, _bucket);
						CompareTraits::assign(l, bucket.low());
					}
					else
					{
						bucket_type _bucket = make_bucket(l, h, container_);
						buckets_.insert(p, _bucket);
						CompareTraits::assign(l, bucket.low());
						continue;
					}
				}

				if (CompareTraits::lt(l, bucket.low()))
				{
					if (CompareTraits::lt(h, bucket.high()))
					{
						bucket_type bucket_(bucket);
						bucket_.set_high(h);
						buckets_.insert(p, bucket_);
						bucket.set_low(h);
						CompareTraits::assign(l, h);
						continue;
					}
					else
					{
						CompareTraits::assign(l, bucket.high());
					}
				}

				if (CompareTraits::lt(l, bucket.high()))
				{
					{
						bucket_type bucket_(bucket);
						bucket_.set_high(l);
						buckets_.insert(p, bucket_);
						bucket.set_low(l);
					}

					if (CompareTraits::lt(h, bucket.high()))
					{
						bucket_type bucket_(bucket);
						bucket_.set_high(h);
						buckets_.insert(p, bucket_);
						bucket.set_low(h);
					}

					CompareTraits::assign(l, bucket.high());
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
					const bucket_type &bucket = *p;
					if (CompareTraits::lt(lowest_, bucket.low()))
					{
						begin = p;
						b_begin = true;
					}
					if (CompareTraits::lt(highest_, bucket.high()))
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

		[[nodiscard]] int spread(const bucket_type &bucket_)
		{
			int added_to_bucket = 0;

			iterator begin, end;
			const bool b_spliced = splice(bucket_.low(), bucket_.high(), begin, end);

			if (!b_spliced)
				return added_to_bucket;

			index_type l, h;
			CompareTraits::assign(l, bucket_.low());
			CompareTraits::assign(h, bucket_.high());

			if (constrained_)
			{
				if (CompareTraits::lt(l, low_))
					CompareTraits::assign(l, low_);
				if (CompareTraits::lt(high_, h))
					CompareTraits::assign(h, high_);
			}

			for (iterator p = begin; p != end; ++p)
			{
				bucket_type &bucket = *p;
				if (CompareTraits::lt(bucket.high(), l))
					continue;
				if (CompareTraits::lt(h, bucket.low()))
					break;
				value_container &ocontainer_ = bucket.values();
				const value_container &icontainer_ = bucket_.values();
				ValueTraits::append(ocontainer_, icontainer_);
				added_to_bucket++;
			}

			return added_to_bucket;
		}

		[[nodiscard]] int cover(const bucket_type &bucket_)
		{
			int added_to_bucket = 0;

			iterator begin, end;
			const bool b_spliced = splice(bucket_.low(), bucket_.high(), begin, end);

			if (!b_spliced)
				return added_to_bucket;

			index_type l, h;
			CompareTraits::assign(l, bucket_.low());
			CompareTraits::assign(h, bucket_.high());

			if (constrained_)
			{
				if (CompareTraits::lt(l, low_))
					CompareTraits::assign(l, low_);
				if (CompareTraits::lt(high_, h))
					CompareTraits::assign(h, high_);
			}

			iterator next = buckets_.erase(begin, end);

			bucket_type bucket2_ = make_bucket(l, h, bucket_.values());

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
				if (CompareTraits::lt(l, low_))
					CompareTraits::assign(l, low_);
				if (CompareTraits::lt(high_, h))
					CompareTraits::assign(h, high_);
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
		[[nodiscard]] int spread(const bucket_list<Indices, Values, CompareTraits, OtherValueTraits> &bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const bucket_type &bucket = *p;
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
		[[nodiscard]] int cover(const bucket_list<Indices, Values, CompareTraits, OtherValueTraits> &bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const bucket_type &bucket = *p;
				added_to_bucket += cover(bucket);
			}

			return added_to_bucket;
		}

		// Add a method to create a bucket_range
		bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, false> range(Indices start, Indices end)
		{
			return bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, false>(*this, start, end);
		}

		// Add a const method to create a bucket_range
		bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, true> range(Indices start, Indices end) const
		{
			return bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, true>(*this, start, end);
		}
	};
}

#endif // MASUTILS_BUCKET_LIST_H_
