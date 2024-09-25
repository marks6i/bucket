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

#ifndef MASUTILS_BUCKET_MAP_H_
#define MASUTILS_BUCKET_MAP_H_

#include <functional>
#include <map>
#include <stdexcept>
#include <utility>

#include "bucket_compare_traits.h"
#include "bucket_value_traits.h"

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
	class bucket_map
	{
	public:
		typedef bucket_map<Indices,
		                Values,
		                Traits,
		                ContainerTraits> mytype;

		typedef Indices index_type;
		typedef Values value_type;

		typedef typename ContainerTraits::value_container value_container;
		typedef const typename ContainerTraits::value_container const_value_container;

		typedef std::pair<const index_type,
					std::pair< index_type,
							   value_container> > bucket_type;

		struct bucket_compare_functor
		{
			bool operator()(const index_type& x, const index_type& y) const noexcept
			{
				return Traits::lt(x, y);
			}
		};

		typedef std::map<const index_type, std::pair<index_type, value_container>, bucket_compare_functor> bucket_type_map;

		struct accessor {
			accessor() = delete;

			// Getters
			inline static const index_type& low(const bucket_type& t) noexcept         { return t.first;         }
			inline static index_type& high(bucket_type& t) noexcept                    { return t.second.first;  }
			inline static const index_type& high(const bucket_type& t) noexcept        { return t.second.first;  }
			inline static value_container& values(bucket_type& t) noexcept             { return t.second.second; }
			inline static const_value_container& values(const bucket_type& t) noexcept { return t.second.second; }

			inline static const std::pair< index_type, value_container>& data(const bucket_type& t) noexcept { return t.second; }
		};

		static inline bucket_type make_bucket(index_type low, index_type high, const value_container& values)
		{
			return std::make_pair(low, std::make_pair(high, values)); // Assuming compiler is using RVO/NRVO to avoid copies
		}

		typedef typename bucket_type_map::iterator iterator;
		typedef typename bucket_type_map::const_iterator const_iterator;
		typedef typename bucket_type_map::reverse_iterator reverse_iterator;
		typedef typename bucket_type_map::const_reverse_iterator const_reverse_iterator;

		/**
		 * @brief Returns an iterator to the first element of the bucket collection.
		 * @return Iterator to the first element.
		 */
		iterator begin() { return buckets_.begin(); }

		/**
		 * @brief Returns an iterator to the element following the last element of the bucket collection.
		 * @return Iterator to the element following the last element.
		 */
		iterator end() { return buckets_.end(); }

		/**
		 * @brief Returns a const iterator to the first element of the bucket collection.
		 * @return Const iterator to the first element.
		 */
		const_iterator begin() const noexcept { return const_iterator(buckets_.begin()); }

		/**
		 * @brief Returns a const iterator to the element following the last element of the bucket collection.
		 * @return Const iterator to the element following the last element.
		 */
		const_iterator end() const noexcept { return const_iterator(buckets_.end()); }

		/**
		 * @brief Returns a reverse iterator to the first element of the reversed bucket collection. It corresponds to the last element of the non-reversed bucket collection.
		 * @return Reverse iterator to the first element.
		 */
		reverse_iterator rbegin() { return buckets_.rbegin(); }

		/**
		 * @brief Returns a reverse iterator to the element following the last element of the reversed bucket collection. It corresponds to the element preceding the first element of the non-reversed bucket collection.
		 * @return Reverse iterator to the element following the last element.
		 */
		reverse_iterator rend() { return buckets_.rend(); }

		/**
		 * @brief Returns a const reverse iterator to the first element of the reversed bucket collection. It corresponds to the last element of the non-reversed bucket collection.
		 * @return Const reverse iterator to the first element.
		 */
		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator(buckets_.rbegin());
		}

		/**
		 * @brief Returns a const reverse iterator to the element following the last element of the reversed bucket collection. It corresponds to the element preceding the first element of the non-reversed bucket collection.
		 * @return Const reverse iterator to the element following the last element.
		 */
		const_reverse_iterator rend() const
		{
			return const_reverse_iterator(buckets_.rend());
		}

	private:
		enum class iteration_direction { forward, reverse };

		// Define the range_iterator
		template <bool IsConst>
		class range_iterator
		{
			typedef typename std::conditional_t<IsConst, const bucket_type_map, bucket_type_map> parent_map;
			typedef typename std::conditional_t<IsConst, const_iterator, iterator> iterator_range;

			iterator_range current_;
			iterator_range begin_;
			iterator_range end_;
			index_type start_range_;
			index_type end_range_;
			iteration_direction direction_;

		private:
			static bool overlaps(const bucket_type& bucket, index_type start_range, index_type end_range)
			{
				return !(accessor::low(bucket) >= end_range || accessor::high(bucket) < start_range);
			}

		public:
			range_iterator(parent_map& map, index_type start_range, index_type end_range, iteration_direction direction)
				: begin_(map.begin()), end_(map.end()), start_range_(start_range), end_range_(end_range), direction_(direction)
			{
				begin_ = map.lower_bound(start_range);
				end_ = map.upper_bound(end_range);

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

			range_iterator& operator++()
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

			range_iterator& operator--()
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

			bool operator==(const range_iterator& other) const { return current_ == other.current_; }
			bool operator!=(const range_iterator& other) const { return !(*this == other); }

			bucket_type& operator*() { return *current_; }
			bucket_type* operator->() { return &(*current_); }
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
		range_iterator<IsConst> beginRange(index_type start_range, index_type end_range)
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
		range_iterator<IsConst> endRange(index_type start_range, index_type end_range) {
			// Create an iterator starting from the end of the map
			range_iterator<IsConst> iter(buckets_, start_range, end_range, iteration_direction::forward);
			// Move the iterator to one-past-the-last valid element within the range
			while (iter.current_ != iter.end_ && overlaps(*iter.current_, start_range, end_range)) {
				++iter.current_;
			}
			return iter; // This will be one-past-the-last valid element
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
		range_iterator<IsConst> rbeginRange(index_type start_range, index_type end_range)
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
		range_iterator<IsConst> rendRange(index_type start_range, index_type end_range) {
			// Create an iterator starting from the end of the map (in reverse)
			range_iterator<IsConst> iter(buckets_, start_range, end_range, iteration_direction::reverse);
			// Move the iterator to one-past-the-first valid element within the range
			while (iter.current_ != iter.begin_ && overlaps(*iter.current_, start_range, end_range)) {
				--iter.current_;
			}
			return iter; // This will be one-past-the-first valid element
		}

		std::size_t size() const { return buckets_.size(); } /**< Number of buckets in bucket_map collection */
		bool empty() const { return buckets_.empty(); } /**< Boolean indicating if the bucket_map collection is empty */

	private:
		bucket_map(const mytype&) = default;
		mytype& operator=(const mytype&) = default;

		bucket_type_map buckets_;
		const index_type low_;
		const index_type high_;
		const bool constrained_;

	private:
		class modification_iterators {
		public:
			explicit modification_iterators(index_type lowest, index_type highest)
				noexcept(noexcept(index_type(lowest)) && noexcept(index_type(highest)))
				: lowest_(lowest), highest_(highest) {}

			~modification_iterators() = default;

			void check_range(iterator p) noexcept {
				if (b_begin_ && b_end_)
					return;

				const bucket_type& bucket = *p;

				if (!b_begin_ && Traits::eq(lowest_, accessor::low(bucket))) {
					begin_ = p;
					b_begin_ = true;
				}
				if (!b_end_ && Traits::eq(highest_, accessor::high(bucket))) {
					end_ = p;
					b_end_ = true;
				}
			}

			bool range(iterator& begin, iterator& end)
			{
				if (b_begin_ && b_end_)
				{
					begin = begin_;
					end = std::next(end_);
					return true;
				}
				return false;
			}

			inline const index_type& lowest() const noexcept { return lowest_; }
			inline const index_type& highest() const noexcept { return highest_; }

		private:
			const index_type lowest_;
			const index_type highest_;

			iterator begin_;
			iterator end_;
			bool b_begin_ = false;
			bool b_end_ = false;
		};

	public:
		/**
		 * @brief Returns true if bucket_map is constrained.
		 */
		bool is_constrained() const noexcept { return constrained_; }

		/**
		 * @brief Returns the lower bound of a constrained bucket_list
		 * or a run-time exception if not constrained.
		 */
		index_type lower_bound() const {
			if (!constrained_) {
				throw std::runtime_error("Bounds are not constrained.");
			}
			return low_;
		}

		/**
		 * @brief Returns the upper bound of a constrained bucket_list
		 * or a run-time exception if not constrained.
		 */
		index_type upper_bound() const {
			if (!constrained_) {
				throw std::runtime_error("Bounds are not constrained.");
			}
			return high_;
		}

		/**
		 * @brief Constructor of a constrained bucket_map collection.
		 * @param low Lower bounds of the bucket_map collection.
		 * @param high Upper bounds of the bucket_map collection.
		 */
		explicit bucket_map(index_type low, index_type high) : low_(low), high_(high), constrained_(true)
		{
			if (Traits::lt(high_, low_))
				throw std::invalid_argument("Arguments not in correct order.");
		}

		/**
		 * @brief Constructor of an unconstrained bucket_map collection.
		 */
		explicit bucket_map() noexcept(
			std::is_nothrow_default_constructible<bucket_type_map>::value &&
			std::is_nothrow_default_constructible<index_type>::value &&
			noexcept(false)
			) : low_(), high_(), constrained_(false)
		{
		}

		/**
		 * @brief Default destructor.
		 */
		~bucket_map() = default; // Destructor

		/**
		 * @brief Default move constructor.
		 * @param  Original bucket_map collection.
		 * @return New bucket_map collection.
		 */
		bucket_map& operator=(bucket_map&&) noexcept = default;

	protected:
		bool splice(index_type low, index_type high, iterator& begin, iterator& end)
		{
			index_type l, h;
			Traits::assign(l, low);
			Traits::assign(h, high);

			if (constrained_)
			{
				if (Traits::lt(h, low_) || Traits::lt(high_, l))
					return false;

				// They now must overlap somewhere and their range can now be
				// constricted to the bounds
				if (Traits::lt(l, low_)) Traits::assign(l, low_);
				if (Traits::lt(high_, h)) Traits::assign(h, high_);
			}

			// The modification_iterators keep track of the range of buckets that
			// are between the low and high values.  This is used by the functions
			// that call splice to determine which buckets are affected and are
			// candidates to be further processed.
			//
			// The key function of modification_iterators is check_range which
			// saves the first and last iterators of the range of buckets.
			// There are two important patterns in its use. First, since we
			// iterate over the buckets in order, we need to check every bucket
			// we add (at the point we add it). Second, we should only check the
			// "current" bucket at the end of the loop, since its values might
			// have changed.
			modification_iterators iterators(l, h);

			{

				iterator p = buckets_.lower_bound(iterators.lowest());

				if (p != buckets_.begin()) {
					if (p == buckets_.end() || accessor::low(*p) != iterators.lowest()) {
						--p;
					}
				}

				// check for overlaps to slice buckets
				for (; p != buckets_.end(); ++p)
				{
					if (Traits::lt(l, h) != true)
						break; //  // all done since range is null, always called since l and h change below

					bucket_type& bucket = *p;

					// step 1: first isolate the part of the new range which occurs
					// before the current bucket
					if (Traits::lt(l, accessor::low(bucket)))
					{
						value_container container_;
						if (Traits::lt(accessor::low(bucket), h)) // overlap
						{
							bucket_type _bucket = make_bucket(l, accessor::low(bucket), container_);
							buckets_[accessor::low(_bucket)] = accessor::data(_bucket);
							iterators.check_range(std::prev(p));
							Traits::assign(l, accessor::low(bucket));
						}
						else // no overlap
						{
							bucket_type _bucket = make_bucket(l, h, container_);
							buckets_[accessor::low(_bucket)] = accessor::data(_bucket);
							iterators.check_range(std::prev(p));
							Traits::assign(l, accessor::low(bucket));
							continue; // it all ends before the current bucket
							// so we're done
						}
					}

					// step 2: now isolate the part remaining which starts at
					// the current bucket but ends before the end of the
					// current bucket
					if (Traits::eq(l, accessor::low(bucket)))
					{
						if (Traits::lt(h, accessor::high(bucket)))
						{
							bucket_type bucket_(bucket);
							Traits::assign(accessor::high(bucket), h);
							iterators.check_range(p);
							buckets_[h] = std::make_pair(accessor::high(bucket_), accessor::values(bucket));
							++p;
							Traits::assign(l, h);
							continue;
						}
						else
						{
							// already have a bucket from l to accessor::high(bucket)
							// (the current bucket) so adjust l
							Traits::assign(l, accessor::high(bucket));
						}
					}

					// step 3: now isolate the part remaining which starts after
					// the current bucket and create the two buckets which split
					// the current bucket with l

					if (Traits::lt(l, accessor::high(bucket)))
					{
						{
							// first create the "start of current bucket to
							// l" bucket
							bucket_type bucket_(bucket);
							Traits::assign(accessor::high(bucket), l);
							iterators.check_range(p);

							// next change current bucket to be "l to end of
							// current bucket
							buckets_[l] = std::make_pair(accessor::high(bucket_), accessor::values(bucket_));
							++p;
						}

						// step 4: now isolate the part remaining which starts at
						// _l, the new start of the current bucket and check if _h
						// is less than the end of the current bucket
						if (Traits::lt(h, accessor::high(bucket)))
						{
							bucket_type bucket_(bucket);
							Traits::assign(accessor::high(bucket), h);
							iterators.check_range(p);
							buckets_[h] = std::make_pair(accessor::high(bucket_), accessor::values(bucket_));
							++p;
						}

						// already have a bucket from l to accessor::high(bucket), adjust
						// l to after the current bucket -- accessor::high(bucket)
						Traits::assign(l, accessor::high(bucket));
					}

					// at most, only l (which should be after current bucket) to
					// h is remaining, so loop
					iterators.check_range(p);
				}
			}

			if (Traits::lt(l, h)) // either first bucket or after last bucket
			{
				value_container container_;
				buckets_[l] = std::make_pair(h, container_);
				iterators.check_range(std::prev(buckets_.end()));
			}

			return (iterators.range(begin, end));
		}

		int spread(const bucket_type& bucket_)
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
				// The call to splice above catches the condition below so it
				// is not needed here

				//if (traits::lt(h, low_) || traits::lt(high_, l))
				//  return false;

				// They now must overlap somewhere and their range can now be
				// constricted to the bounds
				if (Traits::lt(l, low_)) Traits::assign(l, low_);
				if (Traits::lt(high_, h)) Traits::assign(h, high_);
			}

			for (iterator p = begin; p != end; ++p)
			{
				bucket_type& bucket(*p);
				if (Traits::lt(accessor::high(bucket), l)) continue; // not yet...
				if (Traits::lt(h, accessor::low(bucket))) break; // already done...
				value_container& ocontainer_ = accessor::values(bucket);
				const value_container& icontainer_ = accessor::values(bucket_); // Ensure icontainer_ is const
				ContainerTraits::append(ocontainer_, icontainer_);
				added_to_bucket++;
			}

			return added_to_bucket;
		}

		int cover(const bucket_type& bucket_)
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
				// The call to splice above catches the condition below so it
				// is not needed here

				//if (traits::lt(h, low_) || traits::lt(high_, l))
				//  return false;

				// They now must overlap somewhere and their range can now be
				// constricted to the bounds
				if (Traits::lt(l, low_)) Traits::assign(l, low_);
				if (Traits::lt(high_, h)) Traits::assign(h, high_);
			}

			iterator next = buckets_.erase(begin, end);

			bucket_type bucket2_ = make_bucket(l, h, accessor::values(bucket_));

			buckets_[accessor::low(bucket_)] = accessor::data(bucket_);

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
		int spread(index_type low, index_type high, value_type value)
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
		int cover(index_type low, index_type high, value_type value)
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
		bool erase(index_type low, index_type high)
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
				// The call to splice above catches the condition below so it
				// is not needed here

				//if (traits::lt(h, low_) || traits::lt(high_, l))
				//  return false;

				// They now must overlap somewhere and their range can now be
				// constricted to the bounds
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
		int spread(const bucket_map<Indices, Values, Traits, OtherContainerTraits>& bucket_)
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
		int cover(const bucket_map<Indices, Values, Traits, OtherContainerTraits>& bucket_)
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

#endif // MASUTILS_BUCKET_MAP_H_
