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

#include <concepts>
#include <functional>
#include <list>
#include <ranges>
#include <span>
#include <stdexcept>
#include <type_traits>

#include "bucket_compare_traits.h"
#include "bucket_iterator.h"
#include "bucket_object.h"
#include "bucket_range.h"
#include "bucket_value_traits.h"

namespace masutils {
/**
 * @brief A bucket_list is a collection of buckets, where each bucket
 * represents a non-overlapping range on an ordered axis.
 *
 * @tparam Indices The type of the indices used to define the ranges.
 * @tparam Values The type of the values stored in the buckets.
 * @tparam CompareTraits The traits class used to define the comparison
 * operations.
 * @tparam ValueTraits The traits class used to define the value type and its
 * operations.
 */
template <typename Indices, typename Values,
          typename CompareTraits = bucket_compare_traits<Indices>,
          typename ValueTraits = bucket_value_traits<Values>>
class bucket_list {
  static_assert(std::is_arithmetic_v<Indices> ||
                    (std::is_class_v<Indices> &&
                     std::is_convertible_v<
                         decltype(CompareTraits::lt(std::declval<Indices>(),
                                                    std::declval<Indices>())),
                         bool> &&
                     std::is_convertible_v<
                         decltype(CompareTraits::eq(std::declval<Indices>(),
                                                    std::declval<Indices>())),
                         bool>),
                "Indices must be either an arithmetic type or a class type "
                "that supports CompareTraits operations");

public:
  using mytype = bucket_list<Indices, Values, CompareTraits, ValueTraits>;

  using index_type = Indices;
  using value_type = Values;

  // Public container type that represents the actual values
  using value_container = typename ValueTraits::value_container;
  using const_value_container = const typename ValueTraits::value_container;

  // Define the bucket type using the new bucket_object class
  using bucket_type = bucket_object<index_type, value_container>;

  // Use a list to store the buckets
  using bucket_type_list = std::list<bucket_type>;

  // Update iterator type definitions
  using iterator = bucket_iterator_base<bucket_type_list, bucket_type, false>;
  using const_iterator =
      bucket_iterator_base<bucket_type_list, bucket_type, true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  // Update iterator methods
  [[nodiscard]] iterator begin() { return iterator(buckets_.begin()); }
  [[nodiscard]] const_iterator begin() const {
    return const_iterator(buckets_.begin());
  }
  [[nodiscard]] const_iterator cbegin() const {
    return const_iterator(buckets_.begin());
  }

  [[nodiscard]] iterator end() { return iterator(buckets_.end()); }
  [[nodiscard]] const_iterator end() const {
    return const_iterator(buckets_.end());
  }
  [[nodiscard]] const_iterator cend() const {
    return const_iterator(buckets_.end());
  }

  [[nodiscard]] reverse_iterator rbegin() { return reverse_iterator(end()); }
  [[nodiscard]] const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  [[nodiscard]] const_reverse_iterator crbegin() const {
    return const_reverse_iterator(end());
  }

  [[nodiscard]] reverse_iterator rend() { return reverse_iterator(begin()); }
  [[nodiscard]] const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }
  [[nodiscard]] const_reverse_iterator crend() const {
    return const_reverse_iterator(begin());
  }

public:
  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return buckets_.size();
  }
  [[nodiscard]] constexpr bool empty() const noexcept {
    return buckets_.empty();
  }
  [[nodiscard]] index_type low() const {
    if (!constrained_) {
      throw std::runtime_error("Bounds are not constrained.");
    }
    return low_;
  }
  [[nodiscard]] index_type high() const {
    if (!constrained_) {
      throw std::runtime_error("Bounds are not constrained.");
    }
    return high_;
  }
  [[nodiscard]] constexpr bool constrained() const noexcept {
    return constrained_;
  }

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
  explicit bucket_list(index_type low, index_type high)
      : low_(low), high_(high), constrained_(true) {
    if (CompareTraits::lt(high_, low_))
      throw std::invalid_argument("Arguments not in correct order.");
  }

  /**
   * @brief Constructor of an unconstrained buckets collection.
   */
  explicit bucket_list() noexcept(
      std::is_nothrow_default_constructible<bucket_type>::value &&
      std::is_nothrow_default_constructible<index_type>::value &&
      noexcept(false))
      : low_(), high_(), constrained_(false) {}

  /**
   * @brief Default destructor.
   */
  ~bucket_list() = default;

  /**
   * @brief Default move assignment operator.
   * @param  Original buckets collection.
   * @return New buckets collection.
   */
  bucket_list &operator=(bucket_list &&) noexcept = default;

protected:

  /**
   * @brief Splice a range of buckets.
   * @param low Lower bound of the range.
   * @param high Upper bound of the range.
   * @param begin [out] Iterator to the beginning of the affected range.
   * @param end [out] Iterator to the end of the affected range.
   * @return True if the splice was successful.
   */
  [[nodiscard]] bool splice(index_type low, index_type high, 
                           typename bucket_type_list::iterator &begin,
                           typename bucket_type_list::iterator &end) {
    index_type l, h;
    CompareTraits::assign(l, low);
    CompareTraits::assign(h, high);

    if (constrained_) {
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

    // First, handle any existing buckets that overlap with our range
    auto p = buckets_.begin();

	// Skip buckets that are completely before the range
    while (p != buckets_.end() && CompareTraits::lt(p->high(), l)) {
      ++p;
    }

	// Iterate through the buckets and handle overlaps
    for (; p != buckets_.end(); ) {

	  // If the current bucket is completely after the range, we can stop
      if (CompareTraits::lt(l, h) != true)
        break;

      bucket_type &current_bucket = *p;

      // If we have a gap before the current bucket
      if (CompareTraits::lt(l, current_bucket.low())) {
        value_container container_;
        if (CompareTraits::lt(current_bucket.low(), h)) {
          // Create a new bucket for the gap since its in our range
          bucket_type new_bucket = bucket_type(l, current_bucket.low(), container_);
          p = buckets_.insert(p, new_bucket);
          ++p;  // Move past the newly inserted bucket
          CompareTraits::assign(l, current_bucket.low());
          continue;
        } else {
          // Create a new bucket for the entire gap
          bucket_type new_bucket = bucket_type(l, h, container_);
          p = buckets_.insert(p, new_bucket);
          CompareTraits::assign(l, h);
          break;
        }
      }

      // If we have an overlap with the current bucket
      if (CompareTraits::lt(l, current_bucket.high())) {
        if (CompareTraits::lt(current_bucket.low(), l)) {
          // Split the current bucket
          value_container container_ = current_bucket.values();
          bucket_type new_bucket = bucket_type(current_bucket.low(), l, container_);
          p = buckets_.insert(p, new_bucket);
          ++p;  // Move past the newly inserted bucket
          current_bucket.set_low(l);
        }

        if (CompareTraits::lt(h, current_bucket.high())) {
          // Split the current bucket again
          value_container container_ = current_bucket.values();
          bucket_type new_bucket = bucket_type(h, current_bucket.high(), container_);
          p = buckets_.insert(std::next(p), new_bucket);
          current_bucket.set_high(h);
        }

        CompareTraits::assign(l, current_bucket.high());
        ++p;
      } else {
        ++p;
      }
    }

    // Handle any remaining gap at the end
    if (CompareTraits::lt(l, h)) {
      value_container container_;
      bucket_type new_bucket = bucket_type(l, h, container_);
      buckets_.insert(buckets_.end(), new_bucket);
    }

    // Find and set the begin and end iterators
    begin = buckets_.begin();
    end = buckets_.end();
    for (auto it = buckets_.begin(); it != buckets_.end(); ++it) {
      if (CompareTraits::eq(it->low(), lowest_)) {
        begin = it;
      }
      if (CompareTraits::eq(it->high(), highest_)) {
        end = std::next(it);
        break;
      }
    }

    return true;
  }

public:
  /**
   * @brief Spread a bucket into the collection.
   * @param bucket_ The bucket to spread.
   * @return Number of buckets that were added to.
   */
  int spread(const bucket_type &bucket_) {
    int added_to_bucket = 0;

    typename bucket_type_list::iterator begin, end;
    const bool b_spliced = splice(bucket_.low(), bucket_.high(), begin, end);

    if (!b_spliced)
      return added_to_bucket;

    index_type l, h;
    CompareTraits::assign(l, bucket_.low());
    CompareTraits::assign(h, bucket_.high());

    if (constrained_) {
      if (CompareTraits::lt(l, low_))
        CompareTraits::assign(l, low_);
      if (CompareTraits::lt(high_, h))
        CompareTraits::assign(h, high_);
    }

    // Add values to all buckets in the range
    for (auto p = begin; p != end; ++p) {
      bucket_type &bucket = *p;
      value_container &ocontainer_ = bucket.values();
      const value_container &icontainer_ = bucket_.values();
      ValueTraits::append(ocontainer_, icontainer_);
      added_to_bucket++;
    }

    return added_to_bucket;
  }

  /**
   * @brief Cover a bucket in the collection.
   * @param bucket_ The bucket to cover.
   * @return Number of buckets that were added to.
   */
  int cover(const bucket_type &bucket_) {
    int added_to_bucket = 0;

    typename bucket_type_list::iterator begin, end;
    const bool b_spliced = splice(bucket_.low(), bucket_.high(), begin, end);

    if (!b_spliced)
      return added_to_bucket;

    index_type l, h;
    CompareTraits::assign(l, bucket_.low());
    CompareTraits::assign(h, bucket_.high());

    if (constrained_) {
      if (CompareTraits::lt(l, low_))
        CompareTraits::assign(l, low_);
      if (CompareTraits::lt(high_, h))
        CompareTraits::assign(h, high_);
    }

    // Find the range in the internal collection
    auto internal_begin = begin;
    auto internal_end = end;
    
    // Erase the range and get the position for insertion
    auto insert_pos = buckets_.erase(internal_begin, internal_end);

    bucket_type new_bucket = bucket_type(l, h, bucket_.values());
    buckets_.insert(insert_pos, new_bucket);

    added_to_bucket++;

    return added_to_bucket;
  }

  /**
   * @brief Spread a value into the collection.
   * @param low Lower bound of the range.
   * @param high Upper bound of the range.
   * @param value The value to spread.
   * @return Number of buckets that were added to.
   */
  int spread(index_type low, index_type high, value_type value) {
    value_container container_;
    ValueTraits::add(container_, value);
    bucket_type bucket_ = bucket_type(low, high, container_);
    return spread(bucket_);
  }

  /**
   * @brief Cover a value in the collection.
   * @param low Lower bound of the range.
   * @param high Upper bound of the range.
   * @param value The value to cover.
   * @return Number of buckets that were added to.
   */
  int cover(index_type low, index_type high, value_type value) {
    value_container container_;
    ValueTraits::add(container_, value);
    bucket_type bucket_ = bucket_type(low, high, container_);
    return cover(bucket_);
  }

  /**
   * @brief Erase a range from the collection.
   * @param low Lower bound of the range.
   * @param high Upper bound of the range.
   * @return True if the erase was successful.
   */
  bool erase(index_type low, index_type high) {
    if (CompareTraits::lt(high, low)) {
      throw std::invalid_argument("high must be greater than low");
    }
    if (constrained_ &&
        (CompareTraits::lt(low, low_) || CompareTraits::lt(high_, high))) {
      throw std::out_of_range("range is outside of constrained bounds");
    }
    return erase_impl(low, high);
  }

  /**
   * @brief Erase all buckets in the container
   * @return true if any buckets were erased, false otherwise
   */
  bool erase() {
    if (buckets_.empty()) {
      return false;
    }
    buckets_.clear();
    return true;
  }

  /**
   * @brief Find the first bucket that overlaps with the given range
   * @param low Lower bound of the range
   * @param high Upper bound of the range
   * @return Iterator to the first overlapping bucket, or end() if none found
   */
  iterator find_range(index_type low, index_type high) {
    if (CompareTraits::lt(high, low)) {
      throw std::invalid_argument("high must be greater than low");
    }
    if (constrained_ &&
        (CompareTraits::lt(low, low_) || CompareTraits::lt(high_, high))) {
      throw std::out_of_range("range is outside of constrained bounds");
    }
    return find_first_overlapping_bucket(low, high);
  }

  /**
   * @brief Repeated spread each element of a bucket into another bucket.
   * @tparam OtherValueTraits The container traits of the passed bucket.
   * @param bucket_ The bucket to spread.
   * @return Number of buckets that all the values were added to.
   */
  template <class OtherValueTraits>
  int spread(const bucket_list<Indices, Values, CompareTraits, OtherValueTraits>
                 &bucket_) {
    int added_to_bucket = 0;

    for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p) {
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
  int cover(const bucket_list<Indices, Values, CompareTraits, OtherValueTraits>
                &bucket_) {
    int added_to_bucket = 0;

    for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p) {
      const bucket_type &bucket = *p;
      added_to_bucket += cover(bucket);
    }

    return added_to_bucket;
  }

  // Add a method to create a bucket_range
  bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, false>
  range(Indices start, Indices end) {
    return bucket_range<
        bucket_list<Indices, Values, CompareTraits, ValueTraits>, false>(
        *this, start, end);
  }

  // Add a const method to create a bucket_range
  bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, true>
  range(Indices start, Indices end) const {
    return bucket_range<
        bucket_list<Indices, Values, CompareTraits, ValueTraits>, true>(
        *this, start, end);
  }

  [[nodiscard]] iterator find(index_type index) {
    for (auto it = begin(); it != end(); ++it) {
      if (CompareTraits::lt(it->low(), index) && CompareTraits::lt(index, it->high())) {
        return it;
      }
    }
    return end();
  }

  [[nodiscard]] const_iterator find(index_type index) const {
    for (auto it = begin(); it != end(); ++it) {
      if (CompareTraits::lt(it->low(), index) && CompareTraits::lt(index, it->high())) {
        return it;
      }
    }
    return end();
  }

protected:
  /**
   * @brief Erase all buckets in the range [low, high)
   * @param low Lower bound of the range
   * @param high Upper bound of the range
   * @return true if any buckets were erased, false otherwise
   */
  bool erase_impl(index_type low, index_type high) {
    typename bucket_type_list::iterator begin, end;
    const bool b_spliced = splice(low, high, begin, end);

    if (!b_spliced)
      return false;

    // Find the range in the internal collection
    auto internal_begin = begin;
    auto internal_end = end;
    
    // Erase the range
    buckets_.erase(internal_begin, internal_end);
    return true;
  }

  /**
   * @brief Find the first bucket that overlaps with the given range
   * @param low Lower bound of the range
   * @param high Upper bound of the range
   * @return Iterator to the first overlapping bucket, or end() if none found
   */
  iterator find_first_overlapping_bucket(index_type low, index_type high) {
    for (auto it = buckets_.begin(); it != buckets_.end(); ++it) {
      if (CompareTraits::lt(it->low(), high) &&
          CompareTraits::lt(low, it->high())) {
        return iterator(it);
      }
    }
    return end();
  }

  // Make these functions available to bucket_range
  friend class bucket_range<mytype, false>;
  friend class bucket_range<mytype, true>;

private:
  bucket_list(const mytype &) = default;
  mytype &operator=(const mytype &) = default;

  bucket_type_list buckets_;
  const index_type low_;
  const index_type high_;
  const bool constrained_;
};
} // namespace masutils

#endif // MASUTILS_BUCKET_LIST_H_
