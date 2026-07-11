#/**
 * @file  bucket_iterator.h
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
 * @brief Defines the iterator interface for our bucket classes.
 */

#pragma once

#include <iterator>
#include <type_traits>
#include <map>

namespace masutils {

// Access policies
template <typename Container>
struct direct_access {
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  
  template <typename Iter>
  static auto& get_reference(Iter it) { return *it; }
  
  template <typename Iter>
  static auto* get_pointer(Iter it) { return &(*it); }
  
  template <typename Iter>
  static bool is_end(Iter it, Iter end) { return it == end; }
};

template <typename Container>
struct map_access {
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  
  template <typename Iter>
  static auto& get_reference(Iter it) { return it->second; }
  
  template <typename Iter>
  static auto* get_pointer(Iter it) { return &(it->second); }
  
  template <typename Iter>
  static bool is_end(Iter it, Iter end) { return it == end; }
};

// Policy selector
template <typename Container>
struct access_policy_selector {
  template <typename Key, typename Value, typename Compare, typename Alloc>
  static constexpr bool is_map(std::map<Key, Value, Compare, Alloc>*) { return true; }
  
  template <typename T>
  static constexpr bool is_map(T*) { return false; }
  
  using type = std::conditional_t<
    is_map(static_cast<Container*>(nullptr)),
    map_access<Container>,
    direct_access<Container>>;
};

template <typename Container>
using access_policy = typename access_policy_selector<Container>::type;

/**
 * @brief Base iterator class for bucket containers
 * @tparam Container The underlying container type (list or map)
 * @tparam ValueType The type of value being iterated over
 * @tparam IsConst Whether this is a const iterator
 * @tparam IteratorCategory The iterator category (forward, bidirectional, etc.)
 */
template <typename Container, typename ValueType, bool IsConst,
          typename IteratorCategory = std::bidirectional_iterator_tag>
class bucket_iterator_base {
public:
  // Add friend declaration for bucket_list
  template <typename Indices, typename Values, typename CompareTraits,
            typename ValueTraits>
  friend class bucket_list;

  using iterator_category = IteratorCategory;
  using value_type = std::conditional_t<IsConst, const ValueType, ValueType>;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;
  
  using iterator_type = std::conditional_t<IsConst, 
    typename Container::const_iterator, 
    typename Container::iterator>;

  using access_policy_type = access_policy<Container>;

  bucket_iterator_base() = default;
  explicit bucket_iterator_base(iterator_type it) : it_(it) {}
  bucket_iterator_base(const bucket_iterator_base&) = default;
  bucket_iterator_base& operator=(const bucket_iterator_base&) = default;

  // Allow conversion from non-const to const iterator
  template <bool OtherIsConst,
            typename = std::enable_if_t<IsConst && !OtherIsConst>>
  explicit bucket_iterator_base(
      const bucket_iterator_base<Container, ValueType, OtherIsConst,
                                 IteratorCategory>& other)
      : it_(other.it_) {}

  // Allow conversion from std::list iterator
  template <typename OtherIterator,
            typename = std::enable_if_t<std::is_convertible_v<
                OtherIterator, iterator_type>>>
  explicit bucket_iterator_base(OtherIterator it) : it_(it) {}

  // Basic iterator operations
  reference operator*() const { 
    return access_policy_type::get_reference(it_);
  }
  
  pointer operator->() const { 
    return access_policy_type::get_pointer(it_);
  }

  // Forward iterator requirements
  bucket_iterator_base& operator++() {
    ++it_;
    return *this;
  }
  
  bucket_iterator_base operator++(int) {
    bucket_iterator_base tmp = *this;
    ++it_;
    return tmp;
  }

  // Bidirectional iterator requirements (if supported)
  template <
      typename = std::enable_if_t<
          std::is_same_v<IteratorCategory, std::bidirectional_iterator_tag> ||
          std::is_same_v<IteratorCategory, std::random_access_iterator_tag>>>
  bucket_iterator_base& operator--() {
    --it_;
    return *this;
  }
  
  template <
      typename = std::enable_if_t<
          std::is_same_v<IteratorCategory, std::bidirectional_iterator_tag> ||
          std::is_same_v<IteratorCategory, std::random_access_iterator_tag>>>
  bucket_iterator_base operator--(int) {
    bucket_iterator_base tmp = *this;
    --it_;
    return tmp;
  }

  // Equality comparison
  template <typename OtherIterator>
  bool operator==(const OtherIterator& other) const {
    if constexpr (std::is_base_of_v<bucket_iterator_base,
                                    std::decay_t<OtherIterator>>) {
      return it_ == other.it_;
    } else {
      return it_ == other;
    }
  }
  
  template <typename OtherIterator>
  bool operator!=(const OtherIterator& other) const {
    return !(*this == other);
  }

private:
  iterator_type it_;
};

// Convenience aliases
template <typename Container, typename ValueType,
          typename IteratorCategory = std::bidirectional_iterator_tag>
using bucket_iterator =
    bucket_iterator_base<Container, ValueType, false, IteratorCategory>;

template <typename Container, typename ValueType,
          typename IteratorCategory = std::bidirectional_iterator_tag>
using bucket_const_iterator =
    bucket_iterator_base<Container, ValueType, true, IteratorCategory>;

} // namespace masutils