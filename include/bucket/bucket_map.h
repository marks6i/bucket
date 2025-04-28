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
#include "bucket_object.h"
#include "bucket_range.h"
#include "bucket_value_traits.h"

namespace masutils {

// SFINAE checks for required operations
template <typename T> struct has_ordering_ops {
  template <typename U>
  static auto test(int)
      -> decltype(std::declval<U>() < std::declval<U>(),
                  std::declval<U>() <= std::declval<U>(),
                  std::declval<U>() > std::declval<U>(),
                  std::declval<U>() >= std::declval<U>(), std::true_type());
  template <typename> static auto test(...) -> std::false_type;
  static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename T> struct has_equality_ops {
  template <typename U>
  static auto test(int)
      -> decltype(std::declval<U>() == std::declval<U>(),
                  std::declval<U>() != std::declval<U>(), std::true_type());
  template <typename> static auto test(...) -> std::false_type;
  static constexpr bool value = decltype(test<T>(0))::value;
};

// Forward declare bucket_map
template <class Indices, class Values, class CompareTraits, class ValueTraits>
class bucket_map;

/**
 * @brief The bucket_map class.
 * @tparam Indices The type of the keys in the bucket.
 * @tparam Values The type of the values in the bucket.
 * @tparam CompareTraits The operations that can be performed on the keys.
 * @tparam ValueTraits The operations that can be performed on the value
 * container.
 */
template <class Indices, class Values,
          class CompareTraits = bucket_compare_traits<Indices>,
          class ValueTraits = bucket_value_traits<Values>>
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

  // Define the bucket type using the new bucket_object class
  using bucket_type = bucket_object<index_type, value_container>;

  // Use a map to store the buckets
  using bucket_type_map = std::map<index_type, bucket_type>;

  // Iterator classes defined inside bucket_map
  class iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = bucket_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    iterator() = default;
    explicit iterator(typename bucket_type_map::iterator it) : it_(it) {}

    reference operator*() { return it_->second; }
    const reference operator*() const { return it_->second; }
    pointer operator->() { return &(it_->second); }
    const pointer operator->() const { return &(it_->second); }
    iterator &operator++() {
      ++it_;
      return *this;
    }
    iterator operator++(int) {
      iterator tmp = *this;
      ++it_;
      return tmp;
    }
    iterator &operator--() {
      --it_;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      --it_;
      return tmp;
    }
    bool operator==(const iterator &other) const { return it_ == other.it_; }
    bool operator!=(const iterator &other) const { return it_ != other.it_; }

  private:
    typename bucket_type_map::iterator it_;
    friend class bucket_map;
  };

  class const_iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const bucket_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type *;
    using reference = const value_type &;

    const_iterator() = default;
    explicit const_iterator(typename bucket_type_map::const_iterator it)
        : it_(it) {}

    reference operator*() const { return it_->second; }
    pointer operator->() const { return &(it_->second); }
    const_iterator &operator++() {
      ++it_;
      return *this;
    }
    const_iterator operator++(int) {
      const_iterator tmp = *this;
      ++it_;
      return tmp;
    }
    const_iterator &operator--() {
      --it_;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      --it_;
      return tmp;
    }
    bool operator==(const const_iterator &other) const {
      return it_ == other.it_;
    }
    bool operator!=(const const_iterator &other) const {
      return it_ != other.it_;
    }

  private:
    typename bucket_type_map::const_iterator it_;
    friend class bucket_map;
  };

  class reverse_iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = bucket_type;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type *;
    using reference = value_type &;

    reverse_iterator() = default;
    explicit reverse_iterator(typename bucket_type_map::reverse_iterator it)
        : it_(it) {}

    reference operator*() { return it_->second; }
    const reference operator*() const { return it_->second; }
    pointer operator->() { return &(it_->second); }
    const pointer operator->() const { return &(it_->second); }
    reverse_iterator &operator++() {
      ++it_;
      return *this;
    }
    reverse_iterator operator++(int) {
      reverse_iterator tmp = *this;
      ++it_;
      return tmp;
    }
    reverse_iterator &operator--() {
      --it_;
      return *this;
    }
    reverse_iterator operator--(int) {
      reverse_iterator tmp = *this;
      --it_;
      return tmp;
    }
    bool operator==(const reverse_iterator &other) const {
      return it_ == other.it_;
    }
    bool operator!=(const reverse_iterator &other) const {
      return it_ != other.it_;
    }

  private:
    typename bucket_type_map::reverse_iterator it_;
    friend class bucket_map;
  };

  class const_reverse_iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = const bucket_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type *;
    using reference = const value_type &;

    const_reverse_iterator() = default;
    explicit const_reverse_iterator(
        typename bucket_type_map::const_reverse_iterator it)
        : it_(it) {}

    reference operator*() const { return it_->second; }
    pointer operator->() const { return &(it_->second); }
    const_reverse_iterator &operator++() {
      ++it_;
      return *this;
    }
    const_reverse_iterator operator++(int) {
      const_reverse_iterator tmp = *this;
      ++it_;
      return tmp;
    }
    const_reverse_iterator &operator--() {
      --it_;
      return *this;
    }
    const_reverse_iterator operator--(int) {
      const_reverse_iterator tmp = *this;
      --it_;
      return tmp;
    }
    bool operator==(const const_reverse_iterator &other) const {
      return it_ == other.it_;
    }
    bool operator!=(const const_reverse_iterator &other) const {
      return it_ != other.it_;
    }

  private:
    typename bucket_type_map::const_reverse_iterator it_;
    friend class bucket_map;
  };

  using size_type = std::size_t;

  // Helper to create a bucket
  [[nodiscard]] static constexpr bucket_type
  make_bucket(index_type low, index_type high, const value_container &values) {
    return bucket_type(low, high, values);
  }

  // Helper to create a bucket
  static bucket_type create_bucket(index_type index) {
    return bucket_type(index);
  }

  /**
   * @brief Constructor for a constrained bucket collection.
   * @param low The lower bound.
   * @param high The upper bound.
   * @throw std::invalid_argument if the bounds are not in the correct order.
   */
  explicit bucket_map(index_type low, index_type high)
      : low_(low), high_(high), constrained_(true) {
    if (CompareTraits::lt(high_, low_))
      throw std::invalid_argument("Arguments not in correct order.");
  }

  /**
   * @brief Default constructor.
   */
  explicit bucket_map() noexcept(
      std::is_nothrow_default_constructible<bucket_type>::value &&
      std::is_nothrow_default_constructible<index_type>::value &&
      noexcept(false))
      : low_(), high_(), constrained_(false) {}

  /**
   * @brief Default destructor.
   */
  ~bucket_map() = default;

  /**
   * @brief Default move constructor.
   */
  bucket_map(bucket_map &&) noexcept = default;

  /**
   * @brief Default copy constructor.
   */
  bucket_map(const bucket_map &) = default;

  /**
   * @brief Default move assignment operator.
   */
  bucket_map &operator=(bucket_map &&) noexcept = default;

  /**
   * @brief Default copy assignment operator.
   */
  bucket_map &operator=(const bucket_map &) = default;

  // Iterator methods
  /**
   * @brief Returns an iterator to the first element of the bucket collection.
   * @return Iterator to the first element.
   */
  [[nodiscard]] iterator begin() noexcept { return iterator(buckets_.begin()); }

  /**
   * @brief Returns an iterator to the element following the last element of the
   * bucket collection.
   * @return Iterator to the element following the last element.
   */
  [[nodiscard]] iterator end() noexcept { return iterator(buckets_.end()); }

  /**
   * @brief Returns a const iterator to the first element of the bucket
   * collection.
   * @return Const iterator to the first element.
   */
  [[nodiscard]] const_iterator begin() const noexcept {
    return const_iterator(buckets_.begin());
  }

  /**
   * @brief Returns a const iterator to the element following the last element
   * of the bucket collection.
   * @return Const iterator to the element following the last element.
   */
  [[nodiscard]] const_iterator end() const noexcept {
    return const_iterator(buckets_.end());
  }

  /**
   * @brief Returns a reverse iterator to the first element of the reversed
   * bucket collection.
   * @return Reverse iterator to the first element.
   */
  [[nodiscard]] reverse_iterator rbegin() noexcept {
    return reverse_iterator(buckets_.rbegin());
  }

  /**
   * @brief Returns a reverse iterator to the element following the last element
   * of the reversed bucket collection.
   * @return Reverse iterator to the element following the last element.
   */
  [[nodiscard]] reverse_iterator rend() noexcept {
    return reverse_iterator(buckets_.rend());
  }

  /**
   * @brief Returns a const reverse iterator to the first element of the
   * reversed bucket collection.
   * @return Const reverse iterator to the first element.
   */
  [[nodiscard]] const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(buckets_.rbegin());
  }

  /**
   * @brief Returns a const reverse iterator to the element following the last
   * element of the reversed bucket collection.
   * @return Const reverse iterator to the element following the last element.
   */
  [[nodiscard]] const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(buckets_.rend());
  }

  /**
   * @brief Returns a const iterator to the first element of the bucket
   * collection.
   * @return Const iterator to the first element.
   */
  [[nodiscard]] const_iterator cbegin() const noexcept {
    return const_iterator(buckets_.cbegin());
  }

  /**
   * @brief Returns a const iterator to the element following the last element
   * of the bucket collection.
   * @return Const iterator to the element following the last element.
   */
  [[nodiscard]] const_iterator cend() const noexcept {
    return const_iterator(buckets_.cend());
  }

  /**
   * @brief Returns a const reverse iterator to the first element of the
   * reversed bucket collection.
   * @return Const reverse iterator to the first element.
   */
  [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(buckets_.crbegin());
  }

  /**
   * @brief Returns a const reverse iterator to the element following the last
   * element of the reversed bucket collection.
   * @return Const reverse iterator to the element following the last element.
   */
  [[nodiscard]] const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(buckets_.crend());
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

  /**
   * @brief Returns the maximum number of buckets that can be stored.
   * @return The maximum number of buckets.
   */
  [[nodiscard]] constexpr std::size_t max_size() const noexcept {
    return buckets_.max_size();
  }

  // Modifiers
  void clear() noexcept { buckets_.clear(); }

  // Insert methods
  std::pair<iterator, bool> insert(const bucket_type &bucket) {
    auto [it, inserted] = buckets_.insert({bucket.index(), bucket});
    return {iterator(it), inserted};
  }

  std::pair<iterator, bool> insert(bucket_type &&bucket) {
    auto [it, inserted] = buckets_.insert({bucket.index(), std::move(bucket)});
    return {iterator(it), inserted};
  }

  template <class InputIt> void insert(InputIt first, InputIt last) {
    for (auto it = first; it != last; ++it) {
      buckets_.insert({it->index(), *it});
    }
  }

  // Erase methods
  iterator erase(const_iterator pos) {
    return iterator(buckets_.erase(pos.it_));
  }
  iterator erase(const_iterator first, const_iterator last) {
    return iterator(buckets_.erase(first.it_, last.it_));
  }
  size_type erase(const index_type &index) { return buckets_.erase(index); }

  // Lookup methods
  iterator find(const index_type &index) {
    return iterator(buckets_.find(index));
  }
  const_iterator find(const index_type &index) const {
    return const_iterator(buckets_.find(index));
  }
  size_type count(const index_type &index) const {
    return buckets_.count(index);
  }
  iterator lower_bound(const index_type &index) {
    return iterator(buckets_.lower_bound(index));
  }
  const_iterator lower_bound(const index_type &index) const {
    return const_iterator(buckets_.lower_bound(index));
  }
  iterator upper_bound(const index_type &index) {
    return iterator(buckets_.upper_bound(index));
  }
  const_iterator upper_bound(const index_type &index) const {
    return const_iterator(buckets_.upper_bound(index));
  }
  std::pair<iterator, iterator> equal_range(const index_type &index) {
    auto [first, last] = buckets_.equal_range(index);
    return {iterator(first), iterator(last)};
  }
  std::pair<const_iterator, const_iterator>
  equal_range(const index_type &index) const {
    auto [first, last] = buckets_.equal_range(index);
    return {const_iterator(first), const_iterator(last)};
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
   * @brief Returns true if the bucket collection is constrained.
   * @return True if the collection is constrained.
   */
  [[nodiscard]] constexpr bool is_constrained() const noexcept {
    return constrained_;
  }

  /**
   * @brief Returns the lower bound of a constrained bucket collection.
   * @return The lower bound.
   * @throw std::runtime_error if the collection is not constrained.
   */
  [[nodiscard]] index_type lower_bound() const {
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
  [[nodiscard]] index_type upper_bound() const {
    if (!constrained_) {
      throw std::runtime_error("Bounds are not constrained.");
    }
    return high_;
  }

  // Spread operation
  [[nodiscard]] int spread(index_type low, index_type high, value_type value) {
    value_container container_;
    ValueTraits::add(container_, value);
    bucket_type bucket_ = make_bucket(low, high, container_);
    return spread(bucket_);
  }

  // Cover operation
  [[nodiscard]] int cover(index_type low, index_type high, value_type value) {
    value_container container_;
    ValueTraits::add(container_, value);
    bucket_type bucket_ = make_bucket(low, high, container_);
    return cover(bucket_);
  }

  // Erase operation
  [[nodiscard]] bool erase(index_type low, index_type high) {
    iterator begin, end;
    const bool b_spliced = splice(low, high, begin, end);

    if (!b_spliced)
      return false;

    // Convert our custom iterators to the underlying map's iterator type
    buckets_.erase(begin.it_, end.it_);
    return true;
  }

  // Add a method to create a bucket_range
  bucket_range<bucket_map<Indices, Values, CompareTraits, ValueTraits>, false>
  range(index_type start, index_type end) {
    return bucket_range<bucket_map<Indices, Values, CompareTraits, ValueTraits>,
                        false>(*this, start, end);
  }

  // Add a const method to create a bucket_range
  bucket_range<bucket_map<Indices, Values, CompareTraits, ValueTraits>, true>
  range(index_type start, index_type end) const {
    return bucket_range<bucket_map<Indices, Values, CompareTraits, ValueTraits>,
                        true>(*this, start, end);
  }

protected:
  [[nodiscard]] bool splice(index_type low, index_type high, iterator &begin,
                            iterator &end) {
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
    for (auto p = buckets_.begin(); p != buckets_.end(); ++p) {
      if (CompareTraits::lt(l, h) != true)
        break;

      bucket_type &current_bucket = p->second;

      // If we have a gap before the current bucket
      if (CompareTraits::lt(l, current_bucket.low())) {
        value_container container_;
        if (CompareTraits::lt(current_bucket.low(), h)) {
          // Create a new bucket for the gap
          bucket_type new_bucket =
              make_bucket(l, current_bucket.low(), container_);
          buckets_.insert(p, std::make_pair(l, new_bucket));
          CompareTraits::assign(l, current_bucket.low());
        } else {
          // Create a new bucket that covers the entire range
          bucket_type new_bucket = make_bucket(l, h, container_);
          buckets_.insert(p, std::make_pair(l, new_bucket));
          CompareTraits::assign(l, current_bucket.low());
          continue;
        }
      }

      // If we're at the start of a bucket
      if (CompareTraits::eq(l, current_bucket.low())) {
        if (CompareTraits::lt(h, current_bucket.high())) {
          // Split the bucket at h
          bucket_type split_bucket(current_bucket);
          split_bucket.high() = h;
          // Copy values from original bucket
          ValueTraits::append(split_bucket.values(), current_bucket.values());
          buckets_.insert(p, std::make_pair(l, split_bucket));
          current_bucket.high() = h; // Just set the high value, keep the key
          CompareTraits::assign(l, h);
          continue;
        } else {
          CompareTraits::assign(l, current_bucket.high());
        }
      }

      // If we're in the middle of a bucket
      if (CompareTraits::lt(l, current_bucket.high())) {
        // Split the bucket at l
        bucket_type split_bucket(current_bucket);
        split_bucket.high() = l;
        // Copy values from original bucket
        ValueTraits::append(split_bucket.values(), current_bucket.values());
        buckets_.insert(p, std::make_pair(current_bucket.low(), split_bucket));
        current_bucket.low() = l;

        if (CompareTraits::lt(h, current_bucket.high())) {
          // Split the bucket at h
          bucket_type split_bucket2(current_bucket);
          split_bucket2.high() = h;
          // Copy values from original bucket
          ValueTraits::append(split_bucket2.values(), current_bucket.values());
          buckets_.insert(p, std::make_pair(l, split_bucket2));
          current_bucket.low() = h;
        }

        CompareTraits::assign(l, current_bucket.high());
      }
    }

    // Create a new bucket for any remaining range
    if (CompareTraits::lt(l, h)) {
      value_container container_;
      bucket_type _bucket = make_bucket(l, h, container_);
      buckets_.insert(buckets_.end(), std::make_pair(l, _bucket));
    }

    // Find the begin and end iterators
    bool b_begin = false, b_end = false;

    for (auto p = buckets_.begin(); p != buckets_.end(); ++p) {
      const bucket_type &bucket = p->second;
      if (CompareTraits::eq(lowest_, bucket.low())) {
        begin = iterator(p);
        b_begin = true;
      }
      if (CompareTraits::eq(highest_, bucket.high())) {
        end = iterator(std::next(p));
        b_end = true;
        break;
      }
    }

    return (b_begin && b_end);
  }

  [[nodiscard]] int spread(const bucket_type &bucket_) {
    int added_to_bucket = 0;

    iterator begin, end;
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
    for (auto p = begin.it_; p != end.it_; ++p) {
      bucket_type &bucket = p->second;
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

  [[nodiscard]] int cover(const bucket_type &bucket_) {
    int added_to_bucket = 0;

    iterator begin, end;
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

    // Get the underlying map iterator
    typename bucket_type_map::iterator next =
        buckets_.erase(begin.it_, end.it_);

    bucket_type bucket2_ = make_bucket(l, h, bucket_.values());

    // Insert using the map's iterator
    buckets_.insert(next, std::make_pair(l, bucket2_));

    added_to_bucket++;

    return added_to_bucket;
  }

  template <class OtherValueTraits>
  [[nodiscard]] int spread(const bucket_map<Indices, Values, CompareTraits,
                                            OtherValueTraits> &bucket_) {
    int added_to_bucket = 0;

    for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p) {
      const bucket_type &bucket = *p;
      added_to_bucket += spread(bucket);
    }

    return added_to_bucket;
  }

  template <class OtherValueTraits>
  [[nodiscard]] int cover(const bucket_map<Indices, Values, CompareTraits,
                                           OtherValueTraits> &bucket_) {
    int added_to_bucket = 0;

    for (const_iterator p = bucket_.begin(); p != bucket_.end(); ++p) {
      const bucket_type &bucket = *p;
      added_to_bucket += cover(bucket);
    }

    return added_to_bucket;
  }

private:
  bucket_type_map buckets_;
  index_type low_{};
  index_type high_{};
  bool constrained_{false};
};
} // namespace masutils
