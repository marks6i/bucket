/**
 * @file  bucket_map.h
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
 * @brief The bucket_map class.
 *
 * A bucket is a collection much like a Dictionary (key-value store),
 * where the "keys" represent a non-overlapping range on an ordered axis.
 * Gaps are allowed if there are not values in the given range. The "values"
 * are stored in a collection as well, and can be accessed by the key.
 */

#pragma once

#include <functional>
#include <map>
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
	 * @brief The bucket_map class.
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
	class bucket_map
	{
	public:
		using mytype = bucket_map<Indices,
		                Values,
		                Traits,
		                         ContainerTraits>;

		using index_type = Indices;
		using value_type = Values;

		// Public container type that represents the actual values
		using value_container = typename ContainerTraits::value_container;
		using const_value_container = const typename ContainerTraits::value_container;

		// Define the bucket type using the new bucket_object class
		using bucket_type = bucket_object<index_type, value_container>;
		
		// Use a map with index_type as key and bucket_type as value
		using bucket_type_map = std::map<index_type, bucket_type>;

		struct accessor {
			accessor() = delete;

			// Getters for direct bucket objects
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

			// Getters for map pairs
			template<typename T>
			[[nodiscard]] static constexpr auto& low(std::pair<T, bucket_type>& t) noexcept                     { return t.second.low(); }
			template<typename T>
			[[nodiscard]] static constexpr const auto& low(const std::pair<T, bucket_type>& t) noexcept         { return t.second.low(); }
			template<typename T>
			[[nodiscard]] static constexpr auto& high(std::pair<T, bucket_type>& t) noexcept                    { return t.second.high(); }
			template<typename T>
			[[nodiscard]] static constexpr const auto& high(const std::pair<T, bucket_type>& t) noexcept        { return t.second.high(); }
			template<typename T>
			[[nodiscard]] static constexpr auto& values(std::pair<T, bucket_type>& t) noexcept                  { return t.second.values(); }
			template<typename T>
			[[nodiscard]] static constexpr const auto& values(const std::pair<T, bucket_type>& t) noexcept      { return t.second.values(); }
		};

		// Helper to create a bucket
		[[nodiscard]] static constexpr bucket_type make_bucket(index_type low, index_type high, const value_container& values)
		{
			return bucket_type(low, high, values);
		}

		using iterator = typename bucket_type_map::iterator;
		using const_iterator = typename bucket_type_map::const_iterator;
		using reverse_iterator = typename bucket_type_map::reverse_iterator;
		using const_reverse_iterator = typename bucket_type_map::const_reverse_iterator;

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

		/**
		 * @brief Returns the number of buckets in the map.
		 * @return The number of buckets.
		 */
		[[nodiscard]] size_t size() const noexcept {
			return buckets_.size();
		}

	private:
		bucket_map(const mytype&) = default;
		mytype& operator=(const mytype&) = default;

		bucket_type_map buckets_;
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
		explicit bucket_map(index_type low, index_type high) : low_(low), high_(high), constrained_(true)
		{
			if (Traits::lt(high_, low_))
				throw std::invalid_argument("Arguments not in correct order.");
		}

		/**
		 * @brief Constructor of an unconstrained buckets collection.
		 */
		explicit bucket_map() noexcept(
			std::is_nothrow_default_constructible<bucket_type>::value &&
			std::is_nothrow_default_constructible<index_type>::value &&
			noexcept(false)
			) : low_(), high_(), constrained_(false)
		{
		}

		/**
		 * @brief Default destructor.
		 */
		~bucket_map() = default;

		/**
		 * @brief Default move constructor.
		 * @param  Original buckets collection.
		 * @return New buckets collection.
		 */
		bucket_map& operator=(bucket_map&&) noexcept = default;

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

			// First, handle any existing buckets that overlap with our range
			for (iterator p = buckets_.begin(); p != buckets_.end(); ++p)
				{
					if (Traits::lt(l, h) != true)
					break;

				bucket_type& current_bucket = p->second;

				// If we have a gap before the current bucket
					if (Traits::lt(l, accessor::low(current_bucket)))
					{
						value_container container_;
					if (Traits::lt(accessor::low(current_bucket), h))
						{
						// Create a new bucket for the gap
							bucket_type new_bucket = make_bucket(l, accessor::low(current_bucket), container_);
						buckets_.insert(p, std::make_pair(l, new_bucket));
							Traits::assign(l, accessor::low(current_bucket));
						}
					else
						{
						// Create a new bucket that covers the entire range
							bucket_type new_bucket = make_bucket(l, h, container_);
						buckets_.insert(p, std::make_pair(l, new_bucket));
							Traits::assign(l, accessor::low(current_bucket));
						continue;
						}
					}

				// If we're at the start of a bucket
					if (Traits::eq(l, accessor::low(current_bucket)))
					{
						if (Traits::lt(h, accessor::high(current_bucket)))
						{
						// Split the bucket at h
							bucket_type split_bucket(current_bucket);
						Traits::assign(accessor::high(split_bucket), h);
						// Copy values from original bucket
						ContainerTraits::append(accessor::values(split_bucket), accessor::values(current_bucket));
						buckets_.insert(p, std::make_pair(l, split_bucket));
						Traits::assign(accessor::low(current_bucket), h);
							Traits::assign(l, h);
							continue;
						}
						else
						{
							Traits::assign(l, accessor::high(current_bucket));
						}
					}

				// If we're in the middle of a bucket
					if (Traits::lt(l, accessor::high(current_bucket)))
					{
					// Split the bucket at l
							bucket_type split_bucket(current_bucket);
					Traits::assign(accessor::high(split_bucket), l);
					// Copy values from original bucket
					ContainerTraits::append(accessor::values(split_bucket), accessor::values(current_bucket));
					buckets_.insert(p, std::make_pair(accessor::low(current_bucket), split_bucket));
					Traits::assign(accessor::low(current_bucket), l);

						if (Traits::lt(h, accessor::high(current_bucket)))
						{
						// Split the bucket at h
							bucket_type split_bucket2(current_bucket);
							Traits::assign(accessor::high(split_bucket2), h);
							// Copy values from original bucket
							ContainerTraits::append(accessor::values(split_bucket2), accessor::values(current_bucket));
							buckets_.insert(p, std::make_pair(l, split_bucket2));
							Traits::assign(accessor::low(current_bucket), h);
						}

					Traits::assign(l, accessor::high(current_bucket));
				}
			}

			// Create a new bucket for any remaining range
			if (Traits::lt(l, h))
			{
				value_container container_;
				bucket_type _bucket = make_bucket(l, h, container_);
				buckets_.insert(buckets_.end(), std::make_pair(l, _bucket));
			}

			// Find the begin and end iterators
			bool b_begin = false, b_end = false;

			for (iterator p = buckets_.begin(); p != buckets_.end(); ++p)
			{
				const bucket_type& bucket = p->second;
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
				bucket_type& bucket = p->second;
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

			buckets_.insert(next, std::make_pair(l, bucket2_));

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
		[[nodiscard]] int spread(const bucket_map<Indices, Values, Traits, OtherContainerTraits>& bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const bucket_type& bucket = p->second;
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
		[[nodiscard]] int cover(const bucket_map<Indices, Values, Traits, OtherContainerTraits>& bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const bucket_type& bucket = p->second;
				added_to_bucket += cover(bucket);
			}

			return added_to_bucket;
		}

		// Add a method to create a bucket_range
		bucket_range<bucket_map<Indices, Values, Traits, ContainerTraits>, false> range(Indices start, Indices end) {
			return bucket_range<bucket_map<Indices, Values, Traits, ContainerTraits>, false>(*this, start, end);
		}

		// Add a const method to create a bucket_range
		bucket_range<bucket_map<Indices, Values, Traits, ContainerTraits>, true> range(Indices start, Indices end) const {
			return bucket_range<bucket_map<Indices, Values, Traits, ContainerTraits>, true>(*this, start, end);
		}
	};
}
