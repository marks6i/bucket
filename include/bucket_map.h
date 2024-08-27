// Copyright 2024 Mark Solinski
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// bucket_map.h : Define the bucket_map collection class.
//

#ifndef MASUTILS_BUCKET_MAP_H_
#define MASUTILS_BUCKET_MAP_H_

#include <functional>
#include <map>
#include <list>
#include <stdexcept>

// #include "triplet.h"
#include "bucket_compare_traits.h"
#include "bucket_value_traits.h"

namespace masutils
{
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
											 ContainerTraits>
				mytype;

		typedef Indices index_type;
		typedef Values value_type;

		typedef typename ContainerTraits::value_container value_container;
		typedef const typename ContainerTraits::value_container const_value_container;
		typedef std::pair<std::pair<index_type, index_type>, value_container> triplet_type;
		typedef std::map<std::pair<index_type, index_type>, value_container> triplet_map;

		typedef typename triplet_map::iterator iterator;
		typedef typename triplet_map::const_iterator const_iterator;
		typedef typename triplet_map::reverse_iterator reverse_iterator;
		typedef typename triplet_map::const_reverse_iterator const_reverse_iterator;

		iterator begin() { return buckets_.begin(); }
		iterator end() { return buckets_.end(); }
		const_iterator begin() const noexcept { return const_iterator(buckets_.begin()); }
		const_iterator end() const noexcept { return const_iterator(buckets_.end()); }

		reverse_iterator rbegin() { return buckets_.rbegin(); }
		reverse_iterator rend() { return buckets_.rend(); }

		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator(buckets_.rbegin());
		}

		const_reverse_iterator rend() const
		{
			return const_reverse_iterator(buckets_.rend());
		}

	private:
		enum class iteration_direction
		{
			forward,
			reverse
		};

		// Define the range_iterator
		template <bool IsConst>
		class range_iterator
		{
			typedef typename std::conditional_t<IsConst, const triplet_map, triplet_map> parent_map;
			typedef typename std::conditional_t<IsConst, const_iterator, iterator> iterator_range;

			iterator_range current_;
			iterator_range begin_;
			iterator_range end_;
			index_type start_range_;
			index_type end_range_;
			iteration_direction direction_;

		private:
			static bool overlaps(const triplet_type &triplet, index_type start_range, index_type end_range)
			{
				return !(triplet.first.first >= end_range || triplet.first.second < start_range);
			}

		public:
			range_iterator(parent_map &map, index_type start_range, index_type end_range, iteration_direction direction)
					: begin_(map.begin()), end_(map.end()), start_range_(start_range), end_range_(end_range), direction_(direction)
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

			range_iterator &operator++()
			{
				if (direction_ == iteration_direction::forward)
				{
					do
					{
						++current_;
					} while (current_ != end_ && !overlaps(*current_, start_range_, end_range_));
				}
				return *this;
			}

			range_iterator &operator--()
			{
				if (direction_ == iteration_direction::reverse)
				{
					if (current_ != begin_)
					{
						do
						{
							--current_;
						} while (current_ != begin_ && !overlaps(*current_, start_range_, end_range_));
					}
				}
				return *this;
			}

			bool operator==(const range_iterator &other) const { return current_ == other.current_; }
			bool operator!=(const range_iterator &other) const { return !(*this == other); }

			triplet_type &operator*() { return *current_; }
			triplet_type *operator->() { return &(*current_); }
		};

	public:
		// Forward iterators
		template <bool IsConst>
		range_iterator<IsConst> beginRange(index_type start_range, index_type end_range)
		{
			return range_iterator<IsConst>(buckets_, start_range, end_range, iteration_direction::forward);
		}

		template <bool IsConst>
		range_iterator<IsConst> endRange(index_type start_range, index_type end_range)
		{
			return range_iterator<IsConst>(buckets_, end_range, end_range, iteration_direction::forward);
		}

		// Reverse iterators
		template <bool IsConst>
		range_iterator<IsConst> rbeginRange(index_type start_range, index_type end_range)
		{
			return range_iterator<IsConst>(buckets_, start_range, end_range, iteration_direction::reverse);
		}

		template <bool IsConst>
		range_iterator<IsConst> rendRange(index_type start_range, index_type end_range)
		{
			return range_iterator<IsConst>(buckets_, end_range, end_range, iteration_direction::reverse);
		}

		std::size_t size() const { return buckets_.size(); }
		bool empty() const { return buckets_.empty(); }
		index_type low() const { return low_; }
		index_type high() const { return high_; }
		bool constrained() const { return constrained_; }

	private:
		buckets(const mytype &) = default;
		mytype &operator=(const mytype &) = default;

		triplet_map buckets_;
		index_type low_;
		index_type high_;
		bool constrained_;

	public:
		explicit buckets(index_type low, index_type high) : low_(low), high_(high), constrained_(true)
		{
			if (Traits::lt(high_, low_))
				throw std::invalid_argument("Arguments not in correct order.");
		}

		explicit buckets() noexcept : low_(0), high_(0), constrained_(false)
		{
		}

		~buckets() = default; // Destructor
		buckets &operator=(buckets &&) noexcept = default;

	protected:
		bool splice(index_type low, index_type high, iterator &begin, iterator &end)
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
				if (Traits::lt(l, low_))
					Traits::assign(l, low_);
				if (Traits::lt(high_, h))
					Traits::assign(h, high_);
			}

			index_type lowest_, highest_;
			Traits::assign(lowest_, l);
			Traits::assign(highest_, h);

			// binary search to find the starting position for the splice
			auto it = buckets_.lower_bound(std::make_pair(l, l));

			// check for overlaps to slice buckets
			for (auto p = it; p != buckets_.end(); ++p)
			{
				if (Traits::lt(l, h) != true)
					break; // all done since range is null

				triplet_type &triplet = *p;

				if (Traits::lt(h, triplet.first.first))
					break; // all done since range is null

				// step 1: first isolate the part of the new range which occurs
				// before the current bucket
				if (Traits::lt(l, triplet.first.first))
				{
					value_container container_;
					if (Traits::lt(triplet.first.first, h)) // overlap
					{
						// triplet_type _triplet(l, triplet.first.first, container_); // **TODO**
						// buckets_.insert(p, _triplet);
						Traits::assign(l, triplet.first.first);
					}
					else // no overlap
					{
						// triplet_type _triplet(l, h, container_); // **TODO**
						// buckets_.insert(p, _triplet);
						Traits::assign(l, triplet.first.first);
						continue; // it all ends before the current bucket
											// so we're done
					}
				}

				// step 2: now isolate the part remaining which starts at
				// the current bucket but ends before the end of the
				// current bucket
				if (Traits::eq(l, triplet.first.first))
				{
					if (Traits::lt(h, triplet.first.second))
					{
						triplet_type triplet_(triplet);
						Traits::assign(triplet_.first.second, h);
						// buckets_.insert(p, triplet_);  // **TODO**
						Traits::assign(triplet.first.first, h);
						Traits::assign(l, h);
						continue;
					}
					else
					{
						// already have a bucket from l to triplet.second
						// (the current bucket) so adjust l
						Traits::assign(l, triplet.first.second);
					}
				}

				// step 3: now isolate the part remaining which starts after
				// the current bucket and create the two buckets which split
				// the current bucket with _l
				if (Traits::lt(l, triplet.first.second))
				{
					{
						// first create the "start of current bucket to
						// _l" bucket
						triplet_type triplet_(triplet);
						Traits::assign(triplet_.first.second, l);
						// buckets_.insert(p, triplet_); // **TODO**
						// next change current bucket to be "_l to end of
						// current bucket"
						Traits::assign(triplet.first.first, l);
					}

					// step 4: now isolate the part remaining which starts at
					// _l, the new start of the current bucket and check if _h
					// is less than the end of the current bucket
					if (Traits::lt(h, triplet.first.second))
					{
						triplet_type triplet_(triplet);
						Traits::assign(triplet_.first.second, h);
						// buckets_.insert(p, triplet_); // **TODO**
						Traits::assign(triplet.first.first, h);
					}

					// already have a bucket from l to triplet.second, adjust
					// l to after the current bucket -- triplet.second
					Traits::assign(l, triplet.first.second);
				}

				// at most, only l (which should be after current bucket) to
				// h is remaining, so loop
			}

			if (Traits::lt(l, h)) // either first bucket or after last bucket
			{
				value_container container_;
				// triplet_type _triplet(l, h, container_); // **TODO**
				// buckets_.push_back(_triplet);
			}

			bool b_begin = false, b_end = false;

			{
				for (iterator p = buckets_.begin(); p != buckets_.end(); ++p)
				{
					const triplet_type &triplet = *p;
					if (Traits::eq(lowest_, triplet.first.first))
					{
						begin = p;
						b_begin = true;
					}
					if (Traits::eq(highest_, triplet.first.second))
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

		int spread(const triplet_type &triplet_)
		{
			int added_to_bucket = 0;

			iterator begin, end;
			const bool b_spliced = splice(triplet_.first.first, triplet_.first.second, begin, end);

			if (!b_spliced)
				return added_to_bucket;

			index_type l, h;
			Traits::assign(l, triplet_.first.first);
			Traits::assign(h, triplet_.first.second);

			if (constrained_)
			{
				// The call to splice above catches the condition below so it
				// is not needed here

				// if (traits::lt(h, low_) || traits::lt(high_, l))
				//   return false;

				// They now must overlap somewhere and their range can now be
				// constricted to the bounds
				if (Traits::lt(l, low_))
					Traits::assign(l, low_);
				if (Traits::lt(high_, h))
					Traits::assign(h, high_);
			}

			for (iterator p = begin; p != end; ++p)
			{
				triplet_type &triplet = *p;
				if (Traits::lt(triplet.first.second, l))
					continue; // not yet...
				if (Traits::lt(h, triplet.first.first))
					break; // already done...
				value_container &ocontainer_ = triplet.second;
				const value_container &icontainer_ = triplet_.second;
				// ContainerTraits::append(ocontainer_, icontainer_);  // **TODO**
				added_to_bucket++;
			}

			return added_to_bucket;
		}

		int cover(const triplet_type &triplet_)
		{
			int added_to_bucket = 0;

			iterator begin, end;
			const bool b_spliced = splice(triplet_.first.first, triplet_.first.second, begin, end);

			if (!b_spliced)
				return added_to_bucket;

			index_type l, h;
			Traits::assign(l, triplet_.first.first);
			Traits::assign(h, triplet_.first.second);

			if (constrained_)
			{
				// The call to splice above catches the condition below so it
				// is not needed here

				// if (traits::lt(h, low_) || traits::lt(high_, l))
				//   return false;

				// They now must overlap somewhere and their range can now be
				// constricted to the bounds
				if (Traits::lt(l, low_))
					Traits::assign(l, low_);
				if (Traits::lt(high_, h))
					Traits::assign(h, high_);
			}

			iterator next = buckets_.erase(begin, end);

			// triplet_type triplet2_(l, h, triplet_.second); // **TODO**
			// buckets_.insert(next, triplet2_);

			added_to_bucket++;

			return added_to_bucket;
		}

	public:
		int spread(index_type low, index_type high, value_type value)
		{
			value_container container_;
			ContainerTraits::add(container_, value);
			// triplet_type triplet_(low, high, container_); // **TODO**

			// return spread(triplet_); // **TODO**
		}

		int cover(index_type low, index_type high, value_type value)
		{
			value_container container_;
			ContainerTraits::add(container_, value);
			// triplet_type triplet_(low, high, container_); // **TODO**

			// return cover(triplet_); // **TODO**
		}

		template <class OtherContainerTraits>
		int spread(const buckets<Indices, Values, Traits, OtherContainerTraits> &bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const triplet_type &triplet = *p;
				added_to_bucket += spread(triplet);
			}

			return added_to_bucket;
		}

		template <class OtherContainerTraits>
		int cover(const buckets<Indices, Values, Traits, OtherContainerTraits> &bucket_)
		{
			int added_to_bucket = 0;

			for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p)
			{
				const triplet_type &triplet = *p;
				added_to_bucket += cover(triplet);
			}

			return added_to_bucket;
		}
	};
}

#endif // MASUTILS_BUCKET_MAP_H_
