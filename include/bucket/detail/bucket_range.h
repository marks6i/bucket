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
 * @brief Provides iterators for iterating over a range of buckets in a bucket
 * collection.
 */

#pragma once

#include <algorithm>
#include <iterator>
#include <type_traits>

#include "./bucket_types.h"
#include "./bucket_traits.h"

namespace masutils {

/**
 * @brief Provides iterators for iterating over a range of buckets in a bucket
 * collection.
 * @tparam Container The type of the bucket container.
 * @tparam IsConst Whether the iteration is const.
 */
template <typename Container, bool IsConst>
class bucket_range {
public:
  using container_type =
      std::conditional_t<IsConst, const Container, Container>;
  using container_iterator =
      std::conditional_t<IsConst, typename container_type::const_iterator,
                         typename container_type::iterator>;
  using bucket_type = typename container_type::bucket_type;
  using value_type = bucket_type;
  using reference =
      std::conditional_t<IsConst, const value_type &, value_type &>;
  using pointer = std::conditional_t<IsConst, const value_type *, value_type *>;
  using CompareTraits = typename container_type::compare_traits;

  /**
   * @brief Constructor.
   * @param container The bucket container.
   * @param start The start of the range.
   * @param end The end of the range.
   */
  bucket_range(container_type &container,
               typename container_type::index_type low,
               typename container_type::index_type high)
      : container_(&container), low_(low), high_(high) {}

  /**
   * @brief Iterator class for bucket_range.
   */
  class iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = typename container_type::bucket_type;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
    using reference = std::conditional_t<IsConst, const value_type&, value_type&>;

    iterator() = default;

    iterator(container_type* container, container_iterator current,
            typename container_type::index_type low,
            typename container_type::index_type high,
            bool is_forward)
        : container_(container),
          current_(current),
          low_(low),
          high_(high),
          is_forward_(is_forward) {}

    reference operator*() const {
      return *current_;
    }

    pointer operator->() const {
      return &(*current_);
    }

    iterator& operator++() {
      if (current_ == container_->end()) {
        return *this;
      }

      if (is_forward_) {
        ++current_;
        while (current_ != container_->end() && !overlaps(*current_)) {
          ++current_;
        }
      } else {
        if (current_ == container_->begin()) {
          current_ = container_->end();
        } else {
          --current_;
          while (current_ != container_->begin() && !overlaps(*current_)) {
            --current_;
          }
          if (!overlaps(*current_)) {
            current_ = container_->end();
          }
        }
      }
      return *this;
    }

    iterator& operator--() {
      if (is_forward_) {
        if (current_ == container_->begin()) {
          current_ = container_->end();
        } else {
          if (current_ == container_->end()) {
            auto last = container_->end();
            --last;
            while (last != container_->begin() && !overlaps(*last)) {
              --last;
            }
            if (overlaps(*last)) {
              current_ = last;
              return *this;
            }
          }
          --current_;
          while (current_ != container_->begin() && !overlaps(*current_)) {
            --current_;
          }
          if (!overlaps(*current_)) {
            current_ = container_->end();
          }
        }
      } else {
        if (current_ == container_->end()) {
          auto first = container_->begin();
          while (first != container_->end() && !overlaps(*first)) {
            ++first;
          }
          current_ = first;
        } else {
          ++current_;
          while (current_ != container_->end() && !overlaps(*current_)) {
            ++current_;
          }
        }
      }
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    bool operator==(const iterator& other) const {
      return current_ == other.current_ && is_forward_ == other.is_forward_;
    }

    bool operator!=(const iterator& other) const { return !(*this == other); }

  protected:
    friend class bucket_range;
    container_type *container_ = nullptr;
    container_iterator current_;
    typename container_type::index_type low_ = 0;
    typename container_type::index_type high_ = 0;
    bool is_forward_ = true;

    bool overlaps(const bucket_type& bucket) const {
      return !CompareTraits::lt(bucket.high(), low_) &&
             !CompareTraits::lt(high_, bucket.low());
    }
  };

  using const_iterator = iterator;  // Since our iterator already handles const/non-const

  iterator begin() {
    auto it = iterator(container_, container_->begin(), low_, high_, true);
    while (it.current_ != container_->end() && !it.overlaps(*it.current_)) {
      ++it.current_;
    }
    return it;
  }

  iterator end() {
    return iterator(container_, container_->end(), low_, high_, true);
  }

  const_iterator begin() const {
    auto it = const_iterator(container_, container_->begin(), low_, high_, true);
    while (it.current_ != container_->end() && !it.overlaps(*it.current_)) {
      ++it.current_;
    }
    return it;
  }

  const_iterator end() const {
    return const_iterator(container_, container_->end(), low_, high_, true);
  }

  iterator rbegin() {
    auto it = iterator(container_, container_->end(), low_, high_, false);
    if (!container_->empty()) {
      auto last = container_->end();
      --last;
      while (last != container_->begin() && !it.overlaps(*last)) {
        --last;
      }
      if (it.overlaps(*last)) {
        it.current_ = last;
      } else {
        it.current_ = container_->end();
      }
    }
    return it;
  }

  iterator rend() {
    return iterator(container_, container_->end(), low_, high_, false);
  }

  const_iterator rbegin() const {
    auto it = const_iterator(container_, container_->end(), low_, high_, false);
    if (!container_->empty()) {
      auto last = container_->end();
      --last;
      while (last != container_->begin() && !it.overlaps(*last)) {
        --last;
      }
      if (it.overlaps(*last)) {
        it.current_ = last;
      } else {
        it.current_ = container_->end();
      }
    }
    return it;
  }

  const_iterator rend() const {
    return const_iterator(container_, container_->end(), low_, high_, false);
  }

  /**
   * @brief Check if a bucket contains the given index within the range
   * @param index The index to check
   * @return true if a bucket contains the index, false otherwise
   */
  [[nodiscard]] bool contains(typename container_type::index_type index) const {
    if (CompareTraits::lt(index, low_) || CompareTraits::lt(high_, index)) {
      return false;
    }
    
    for (auto it = begin(); it != end(); ++it) {
      if (!CompareTraits::lt(index, it->low()) && CompareTraits::lt(index, it->high())) {
        return true;
      }
    }
    return false;
  }

  /**
   * @brief Find a bucket containing the given index within the range
   * @param index The index to search for
   * @return Iterator to the bucket containing the index, or end() if not found
   * @throw std::out_of_range if the index is outside the range bounds
   */
  iterator find(typename container_type::index_type index) {
    if (CompareTraits::lt(index, low_) || CompareTraits::lt(high_, index)) {
      throw std::out_of_range("Index is outside the range bounds");
    }
    
    for (auto it = begin(); it != end(); ++it) {
      if (!CompareTraits::lt(index, it->low()) && CompareTraits::lt(index, it->high())) {
        return it;
      }
    }
    return end();
  }

  /**
   * @brief Find a bucket containing the given index within the range (const version)
   * @param index The index to search for
   * @return Const iterator to the bucket containing the index, or end() if not found
   * @throw std::out_of_range if the index is outside the range bounds
   */
  const_iterator find(typename container_type::index_type index) const {
    if (CompareTraits::lt(index, low_) || CompareTraits::lt(high_, index)) {
      throw std::out_of_range("Index is outside the range bounds");
    }
    
    for (auto it = begin(); it != end(); ++it) {
      if (!CompareTraits::lt(index, it->low()) && CompareTraits::lt(index, it->high())) {
        return it;
      }
    }
    return end();
  }

  /**
   * @brief Get a bucket at the given index within the range
   * @param index The index to search for
   * @return Iterator to the bucket containing the index
   * @throw std::out_of_range if no bucket contains the index or if index is outside range bounds
   */
  iterator at(typename container_type::index_type index) {
    auto it = find(index);
    if (it == end()) {
      throw std::out_of_range("No bucket contains the specified index");
    }
    return it;
  }

  /**
   * @brief Get a bucket at the given index within the range (const version)
   * @param index The index to search for
   * @return Const iterator to the bucket containing the index
   * @throw std::out_of_range if no bucket contains the index or if index is outside range bounds
   */
  const_iterator at(typename container_type::index_type index) const {
    auto it = find(index);
    if (it == end()) {
      throw std::out_of_range("No bucket contains the specified index");
    }
    return it;
  }

  /**
   * @brief Find the next bucket relative to the given index within the range
   * @param index The index to search from
   * @return Iterator to the current bucket if index is in it, otherwise the next bucket. Returns end() if no suitable bucket exists.
   */
  iterator next(typename container_type::index_type index) {
    if (CompareTraits::lt(index, low_) || CompareTraits::lt(high_, index)) {
      return end();
    }

    for (auto it = begin(); it != end(); ++it) {
      if (!CompareTraits::lt(index, it->low()) && CompareTraits::lt(index, it->high())) {
        return it;  // Return current bucket if index is in it
      }
      if (CompareTraits::lt(index, it->low())) {
        return it;  // Return this bucket as it's the next one
      }
    }
    return end();
  }

  /**
   * @brief Find the next bucket relative to the given index within the range (const version)
   * @param index The index to search from
   * @return Const iterator to the current bucket if index is in it, otherwise the next bucket. Returns end() if no suitable bucket exists.
   */
  const_iterator next(typename container_type::index_type index) const {
    if (CompareTraits::lt(index, low_) || CompareTraits::lt(high_, index)) {
      return end();
    }

    for (auto it = begin(); it != end(); ++it) {
      if (!CompareTraits::lt(index, it->low()) && CompareTraits::lt(index, it->high())) {
        return it;  // Return current bucket if index is in it
      }
      if (CompareTraits::lt(index, it->low())) {
        return it;  // Return this bucket as it's the next one
      }
    }
    return end();
  }

  /**
   * @brief Find the previous bucket relative to the given index within the range
   * @param index The index to search from
   * @return Iterator to the current bucket if index is in it, otherwise the previous bucket. Returns end() if no suitable bucket exists.
   */
  iterator previous(typename container_type::index_type index) {
    if (CompareTraits::lt(index, low_) || CompareTraits::lt(high_, index)) {
      return end();
    }

    iterator prev = end();
    for (auto it = begin(); it != end(); ++it) {
      if (!CompareTraits::lt(index, it->low()) && CompareTraits::lt(index, it->high())) {
        return it;  // Return current bucket if index is in it
      }
      if (CompareTraits::lt(index, it->low())) {
        return prev;  // Return previous bucket
      }
      prev = it;
    }
    return prev;  // Return last bucket if index is beyond all buckets
  }

  /**
   * @brief Find the previous bucket relative to the given index within the range (const version)
   * @param index The index to search from
   * @return Const iterator to the current bucket if index is in it, otherwise the previous bucket. Returns end() if no suitable bucket exists.
   */
  const_iterator previous(typename container_type::index_type index) const {
    if (CompareTraits::lt(index, low_) || CompareTraits::lt(high_, index)) {
      return end();
    }

    const_iterator prev = end();
    for (auto it = begin(); it != end(); ++it) {
      if (!CompareTraits::lt(index, it->low()) && CompareTraits::lt(index, it->high())) {
        return it;  // Return current bucket if index is in it
      }
      if (CompareTraits::lt(index, it->low())) {
        return prev;  // Return previous bucket
      }
      prev = it;
    }
    return prev;  // Return last bucket if index is beyond all buckets
  }

private:
  container_type *container_;
  typename container_type::index_type low_;
  typename container_type::index_type high_;
};

} // namespace masutils