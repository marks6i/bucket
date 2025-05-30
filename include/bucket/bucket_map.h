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
 * @brief The bucket_map class.
 * @tparam Indices The type of the keys in the bucket.
 * @tparam Values The type of the values in the bucket.
 * @tparam CompareTraits The operations that can be performed on the keys.
 * @tparam ValueTraits The operations that can be performed on the value container.
 */
template <typename Indices, typename Values,
          typename CompareTraits = bucket_compare_traits<Indices>,
          typename ValueTraits = bucket_value_traits<Values>>
class bucket_map {
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
  using mytype = bucket_map<Indices, Values, CompareTraits, ValueTraits>;

  using index_type = Indices;
  using value_type = Values;

  // Public container type that represents the actual values
  using value_container = typename ValueTraits::value_container;
  using const_value_container = const typename ValueTraits::value_container;

  // Define the bucket type using the bucket_object class
  using bucket_type = bucket_object<index_type, value_container>;

  // Use a map to store the buckets
  using bucket_type_map = std::map<index_type, bucket_type>;

  // Update iterator type definitions
  using iterator = bucket_iterator_base<bucket_type_map, bucket_type, false>;
  using const_iterator =
      bucket_iterator_base<bucket_type_map, bucket_type, true>;
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

  // Capacity methods
  /**
   * @brief Returns the number of buckets in the collection.
   * @return The number of buckets.
   */
  [[nodiscard]] constexpr std::size_t size() const noexcept {
    return buckets_.size();
  }

  /**
   * @brief Returns true if the collection is empty.
   * @return True if the collection is empty.
   */
  [[nodiscard]] constexpr bool empty() const noexcept {
    return buckets_.empty();
  }

  // Range operations
  [[nodiscard]] iterator find_range(index_type low, index_type high) {
    if (CompareTraits::lt(high, low)) {
      throw std::invalid_argument("high must be greater than low");
    }
    if (constrained_ &&
        (CompareTraits::lt(low, low_) || CompareTraits::lt(high_, high))) {
      throw std::out_of_range("range is outside of constrained bounds");
    }
    return find_first_overlapping_bucket(low, high);
  }

  [[nodiscard]] const_iterator find_range(index_type low,
                                          index_type high) const {
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
   * @brief Erase all buckets in the range [low, high)
   * @param low Lower bound of the range
   * @param high Upper bound of the range
   * @return true if any buckets were erased, false otherwise
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

  // Bound accessors
  /**
   * @brief Returns the lower bound of a constrained bucket collection.
   * @return The lower bound.
   * @throw std::runtime_error if the collection is not constrained.
   */
  [[nodiscard]] index_type low() const {
    if (!constrained_) {
      throw std::runtime_error("Bounds are not constrained.");
    }
    return low_;
  }

  /**
   * @brief Returns the upper bound of a constrained bucket collection.
   * @return The upper bound.
   * @throw std::runtime_error if the collection is not constrained.
   */
  [[nodiscard]] index_type high() const {
    if (!constrained_) {
      throw std::runtime_error("Bounds are not constrained.");
    }
    return high_;
  }

  /**
   * @brief Returns true if the bucket collection is constrained.
   * @return True if the collection is constrained.
   */
  [[nodiscard]] constexpr bool constrained() const noexcept {
    return constrained_;
  }

  /**
   * @brief Constructor of an unconstrained buckets collection.
   */
  explicit bucket_map() noexcept(
      std::is_nothrow_default_constructible<bucket_type>::value &&
      std::is_nothrow_default_constructible<index_type>::value &&
      noexcept(false))
      : low_(), high_(), constrained_(false) {}

  /**
   * @brief Constructor of a constrained buckets collection.
   * @param low Lower bounds of the buckets collection.
   * @param high Upper bounds of the buckets collection.
   */
  explicit bucket_map(index_type low, index_type high)
      : low_(low), high_(high), constrained_(true) {
    if (CompareTraits::lt(high_, low_))
      throw std::invalid_argument("Arguments not in correct order.");
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
  bucket_map &operator=(bucket_map &&) noexcept = default;

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

  // Spread operation
  int spread(index_type low, index_type high, value_type value) {
    if (CompareTraits::lt(high, low)) {
      throw std::invalid_argument("high must be greater than low");
    }
    if (constrained_ &&
        (CompareTraits::lt(low, low_) || CompareTraits::lt(high_, high))) {
      throw std::out_of_range("range is outside of constrained bounds");
    }

    value_container container_;
    ValueTraits::add(container_, value);
    bucket_type bucket_(low, high, container_);
    return spread(bucket_);
  }

  int spread(const bucket_type &bucket_) {
    int added_to_bucket = 0;

    typename bucket_type_map::iterator begin, end;
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
      bucket_type &bucket = p->second;
      value_container &ocontainer_ = bucket.values();
      const value_container &icontainer_ = bucket_.values();
      ValueTraits::append(ocontainer_, icontainer_);
      added_to_bucket++;
    }

    return added_to_bucket;
  }

  // Cover operation
  int cover(index_type low, index_type high, value_type value) {
    if (CompareTraits::lt(high, low)) {
      throw std::invalid_argument("high must be greater than low");
    }
    if (constrained_ &&
        (CompareTraits::lt(low, low_) || CompareTraits::lt(high_, high))) {
      throw std::out_of_range("range is outside of constrained bounds");
    }

    value_container container_;
    ValueTraits::add(container_, value);
    bucket_type bucket_(low, high, container_);
    return cover(bucket_);
  }

  int cover(const bucket_type &bucket_) {
    int added_to_bucket = 0;

    typename bucket_type_map::iterator begin, end;
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
  
    // Erase the range
    buckets_.erase(internal_begin, internal_end);

    bucket_type new_bucket(l, h, bucket_.values());
    buckets_.insert({new_bucket.low(), new_bucket});
  
    added_to_bucket++;

    return added_to_bucket;
  }

  // Add a method to create a bucket_range
  bucket_range<bucket_map, false>
  range(index_type start, index_type end) {
    return bucket_range<bucket_map, false>(*this, start, end);
  }

  // Add a const method to create a bucket_range
  bucket_range<bucket_map, true>
  range(index_type start, index_type end) const {
    return bucket_range<bucket_map, true>(*this, start, end);
  }

public:
  /**
   * @brief Find the first bucket that overlaps with the given range
   * @param low Lower bound of the range
   * @param high Upper bound of the range
   * @return Iterator to the first overlapping bucket, or end() if none found
   */
  iterator find_first_overlapping_bucket(index_type low, index_type high) {
    auto it = buckets_.lower_bound(low);
    if (it != buckets_.end() && CompareTraits::lt(it->first, high)) {
      return iterator(it);
    }
    return end();
  }

  /**
   * @brief Find the first bucket that starts after the given range
   * @param low Lower bound of the range
   * @param high Upper bound of the range
   * @return Iterator to the first bucket after the range, or end() if none found
   */
  iterator find_first_after_range([[maybe_unused]] index_type low, index_type high) {
    return iterator(buckets_.upper_bound(high));
  }

  /**
   * @brief Splice a range of buckets.
   * @param low Lower bound of the range.
   * @param high Upper bound of the range.
   * @param begin [out] Iterator to the beginning of the affected range.
   * @param end [out] Iterator to the end of the affected range.
   * @return True if the splice was successful.
   */
  [[nodiscard]] bool splice(index_type low, index_type high,
      typename bucket_type_map::iterator& begin,
      typename bucket_type_map::iterator& end) {
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
      while (p != buckets_.end() && CompareTraits::lt(p->second.high(), l)) {
          ++p;
      }

      // Iterate through the buckets and handle overlaps
      for (; p != buckets_.end(); ) {

          // If the current bucket is completely after the range, we can stop
          if (CompareTraits::lt(l, h) != true)
              break;

          bucket_type* current_bucket = &p->second;

          // If we have a gap before the current bucket
          if (CompareTraits::lt(l, current_bucket->low())) {
              if (CompareTraits::lt(current_bucket->low(), h)) {
                  // Create a new bucket for the gap since its in our range
                  bucket_type new_bucket(l, current_bucket->low());
                  buckets_.emplace(l, new_bucket);
                  CompareTraits::assign(l, current_bucket->low());
                  continue;
              }
              else {
                  // Create a new bucket for the entire gap
                  bucket_type new_bucket(l, h);
                  buckets_.emplace(l, new_bucket);
                  CompareTraits::assign(l, h);
                  break;
              }
          }

          // If we have an overlap with the current bucket
          if (CompareTraits::lt(l, current_bucket->high())) {

              if (CompareTraits::lt(current_bucket->low(), l)) {
                 // Split the current bucket
                  bucket_type new_bucket(*current_bucket);
        		 			current_bucket->set_high(l); // first half
                  new_bucket.set_low(l);
                  auto result = buckets_.emplace(new_bucket.low(), new_bucket); // Second half
                  current_bucket = &result.first->second;
                  ++p;  // Move past the newly inserted bucket
              }

              if (CompareTraits::lt(h, current_bucket->high())) {
                  // Split the current bucket again
                  bucket_type new_bucket(*current_bucket);
                  current_bucket->set_high(h); // First half
                  new_bucket.set_low(h);
                  buckets_.emplace(new_bucket.low(), new_bucket); // Second half
              }

              CompareTraits::assign(l, current_bucket->high());
              ++p;
          }
          else {
              ++p;
          }
      }

      // Handle any remaining gap at the end
      if (CompareTraits::lt(l, h)) {
          bucket_type new_bucket(l, h);
          buckets_.emplace(l, new_bucket);
      }

      // Find and set the begin and end iterators
      begin = buckets_.begin();
      end = buckets_.end();
      for (auto it = buckets_.begin(); it != buckets_.end(); ++it) {
          if (CompareTraits::eq(it->second.low(), lowest_)) {
              begin = it;
          }
          if (CompareTraits::eq(it->second.high(), highest_)) {
              end = std::next(it);
              break;
          }
      }

      return true;
  }

  /**
   * @brief Erase all buckets in the range [low, high)
   * @param low Lower bound of the range
   * @param high Upper bound of the range
   * @return true if any buckets were erased, false otherwise
   */
  bool erase_impl(index_type low, index_type high) {
    typename bucket_type_map::iterator begin, end;
    const bool b_spliced = splice(low, high, begin, end);  // Empty container is fine for erase

    if (b_spliced) {
      buckets_.erase(begin, end);
      return true;
    }

    return false;
  }

public:
  template <class OtherValueTraits>
  int spread(const bucket_map<Indices, Values, CompareTraits, OtherValueTraits>
          &bucket_) {
    int added_to_bucket = 0;

    for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p) {
      const bucket_type &bucket = *p;
      added_to_bucket += spread(bucket);
    }

    return added_to_bucket;
  }

  template <class OtherValueTraits>
  int cover(const bucket_map<Indices, Values, CompareTraits, OtherValueTraits>
                &bucket_) {
    int added_to_bucket = 0;

    for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p) {
      const bucket_type &bucket = *p;
      added_to_bucket += cover(bucket);
    }

    return added_to_bucket;
  }

  /**
   * @brief Find a bucket containing the given index
   * @param index The index to search for
   * @return Iterator to the bucket containing the index, or end() if not found
   */
  [[nodiscard]] iterator find(index_type index) {
    for (auto it = begin(); it != end(); ++it) {
      if (CompareTraits::lt(it->low(), index) && CompareTraits::lt(index, it->high())) {
        return it;
      }
    }
    return end();
  }

  /**
   * @brief Find a bucket containing the given index (const version)
   * @param index The index to search for
   * @return Const iterator to the bucket containing the index, or end() if not found
   */
  [[nodiscard]] const_iterator find(index_type index) const {
    for (auto it = begin(); it != end(); ++it) {
      if (CompareTraits::lt(it->low(), index) && CompareTraits::lt(index, it->high())) {
        return it;
      }
    }
    return end();
  }

private:
  bucket_type_map buckets_;
  index_type low_{};
  index_type high_{};
  bool constrained_{false};
};
} // namespace masutils