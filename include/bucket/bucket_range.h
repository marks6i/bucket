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

#include "bucket_traits.h"
#include <algorithm>
#include <concepts>
#include <iterator>
#include <type_traits>

namespace masutils {
/**
 * @brief Concept that checks if a type has the required bucket interface.
 * @tparam T The type to check.
 */
template <typename T>
concept has_bucket_interface = requires(T t) {
  typename T::index_type;
  typename T::value_container_type;
  { t.low() } -> std::same_as<typename T::index_type &>;
  { t.high() } -> std::same_as<typename T::index_type &>;
  { t.values() } -> std::same_as<typename T::value_container_type &>;
} || requires(const T t) {
  typename T::index_type;
  typename T::value_container_type;
  { t.low() } -> std::same_as<const typename T::index_type &>;
  { t.high() } -> std::same_as<const typename T::index_type &>;
  { t.values() } -> std::same_as<const typename T::value_container_type &>;
};

/**
 * @brief Concept that checks if a container type has the required bucket type.
 * @tparam T The container type to check.
 */
template <typename T>
concept has_bucket_type = requires {
  typename T::bucket_type;
  requires has_bucket_interface<typename T::bucket_type>;
  typename T::index_type;
  requires std::same_as<typename T::index_type,
                        typename T::bucket_type::index_type>;
};

/**
 * @brief Provides iterators for iterating over a range of buckets in a bucket
 * collection.
 * @tparam Container The type of the bucket container.
 * @tparam IsConst Whether the iteration is const.
 */
template <typename Container, bool IsConst>
  requires has_bucket_type<Container>
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
    using value_type = bucket_range::value_type;
    using reference = bucket_range::reference;
    using pointer = bucket_range::pointer;
    using difference_type = std::ptrdiff_t;

    /**
     * @brief Default constructor.
     */
    iterator() = default;

    /**
     * @brief Constructor.
     * @param container The bucket container.
     * @param start The start of the range.
     * @param end The end of the range.
     * @param current The current iterator position.
     * @param forward Whether to iterate forward or backward.
     */
    iterator(container_type *container, container_iterator current,
             typename container_type::index_type low,
             typename container_type::index_type high, bool forward = true)
        : container_(container), current_(current), low_(low), high_(high),
          forward_(forward) {
      if (current_ != container_->end() && !overlaps(*current_)) {
        ++(*this);
      }
    }

    /**
     * @brief Dereference operator.
     * @return Reference to the current bucket.
     */
    reference operator*() const { return *current_; }

    /**
     * @brief Arrow operator.
     * @return Pointer to the current bucket.
     */
    pointer operator->() const {
      // Use operator* to get a reference to the bucket
      // This works for both bucket_map and bucket_list
      return &operator*();
    }

    /**
     * @brief Pre-increment operator.
     * @return Reference to this iterator.
     */
    iterator &operator++() {
      if (current_ == container_->end())
        return *this;
      if (forward_) {
        ++current_;
        while (current_ != container_->end() && !overlaps(*current_)) {
          ++current_;
        }
      } else {
        --current_;
        while (current_ != container_->begin() && !overlaps(*current_)) {
          --current_;
        }
        if (!overlaps(*current_)) {
          current_ = container_->end();
        }
      }
      return *this;
    }

    /**
     * @brief Post-increment operator.
     * @return Copy of this iterator before increment.
     */
    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    /**
     * @brief Pre-decrement operator.
     * @return Reference to this iterator.
     */
    iterator &operator--() {
      if (forward_) {
        if (current_ == container_->begin()) {
          current_ = container_->end();
          return *this;
        }
        --current_;
        while (current_ != container_->begin() && !overlaps(*current_)) {
          --current_;
        }
        if (!overlaps(*current_)) {
          current_ = container_->end();
        }
      } else {
        if (current_ == container_->end()) {
          auto last = --container_->end();
          while (last != container_->begin() && !overlaps(*last)) {
            --last;
          }
          if (overlaps(*last)) {
            current_ = last;
          }
        } else {
          ++current_;
          while (current_ != container_->end() && !overlaps(*current_)) {
            ++current_;
          }
        }
      }
      return *this;
    }

    /**
     * @brief Post-decrement operator.
     * @return Copy of this iterator before decrement.
     */
    iterator operator--(int) {
      iterator tmp = *this;
      --(*this);
      return tmp;
    }

    /**
     * @brief Equality operator.
     * @param other The iterator to compare with.
     * @return True if the iterators are equal.
     */
    bool operator==(const iterator &other) const {
      return current_ == other.current_ && forward_ == other.forward_;
    }

    /**
     * @brief Inequality operator.
     * @param other The iterator to compare with.
     * @return True if the iterators are not equal.
     */
    bool operator!=(const iterator &other) const { return !(*this == other); }

  protected:
    friend class bucket_range;
    container_type *container_ = nullptr;
    container_iterator current_;
    typename container_type::index_type low_ = 0;
    typename container_type::index_type high_ = 0;
    bool forward_ = true;

    /**
     * @brief Check if a bucket overlaps with the range.
     * @param bucket The bucket to check.
     * @return True if the bucket overlaps with the range.
     */
    bool overlaps(const bucket_type &bucket) const {
      return bucket.high() > low_ && bucket.low() < high_;
    }
  };

  /**
   * @brief Get an iterator to the beginning of the range.
   * @return Iterator to the beginning.
   */
  iterator begin() {
    return iterator(container_, container_->begin(), low_, high_);
  }

  /**
   * @brief Get an iterator to the end of the range.
   * @return Iterator to the end.
   */
  iterator end() {
    return iterator(container_, container_->end(), low_, high_);
  }

  /**
   * @brief Get a reverse iterator to the beginning of the range.
   * @return Reverse iterator to the beginning.
   */
  iterator rbegin() {
    auto last = container_->end();
    if (last != container_->begin()) {
      --last;
      while (last != container_->begin() &&
             !iterator(container_, last, low_, high_).overlaps(*last)) {
        --last;
      }
      if (!iterator(container_, last, low_, high_).overlaps(*last)) {
        last = container_->end();
      }
    }
    return iterator(container_, last, low_, high_, false);
  }

  /**
   * @brief Get a reverse iterator to the end of the range.
   * @return Reverse iterator to the end.
   */
  iterator rend() {
    return iterator(container_, container_->end(), low_, high_, false);
  }

private:
  container_type *container_;
  typename container_type::index_type low_;
  typename container_type::index_type high_;
};
} // namespace masutils