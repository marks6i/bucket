/**
 * @file  bucket_value_traits.h
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
 * @brief Operations supported on the bucket value container.
 *
 * The \bbucket_value_traits struct describes all the operations that can be
 * performed on the value collection of a bucket. Any collection type can be
 * used as the value container, but the operations must be defined for the
 * container type. Currently, the operations are \badd, \bappend, and \bremove.
 */

#pragma once

#include <algorithm>
#include <list>
#include <type_traits>
#include <unordered_map>
#include <vector>

#if __cplusplus >= 202002L
#include <concepts>
#endif // __cplusplus >= 202002L

namespace masutils {

#if __cplusplus >= 202002L
// C++20 concepts for container requirements
template <typename C>
concept container = requires(C c, typename C::value_type v) {
  { c.begin() } -> std::same_as<typename C::iterator>;
  { c.end() } -> std::same_as<typename C::iterator>;
  { c.push_back(v) } -> std::same_as<void>;
  { c.insert(c.end(), v) } -> std::same_as<typename C::iterator>;
  { c.erase(c.begin()) } -> std::same_as<typename C::iterator>;
};

template <typename C>
concept forward_iterable = requires(C c) {
  { c.begin() } -> std::forward_iterator;
  { c.end() } -> std::forward_iterator;
};
#endif // __cplusplus >= 202002L

/**
 * @brief Traits class for bucket value operations.
 *
 * This traits class provides operations for working with bucket values,
 * including container type definitions and value containment checks.
 *
 * @tparam ValueType The type of values stored in the bucket
 * @tparam ContainerType The container type used to store values (defaults to
 * std::list<ValueType>)
 */
template <typename ValueType, typename ContainerType = std::list<ValueType>>
struct bucket_value_traits {
  using value_type = ValueType;
  using value_container = ContainerType;
  using container_type = ContainerType;

  /**
   * @brief Check if a value exists in the container.
   *
   * @param container The container to search in
   * @param value The value to search for
   * @return true if the value exists in the container
   */
  static bool contains(const container_type &container,
                       const value_type &value) {
    return std::find(container.begin(), container.end(), value) !=
           container.end();
  }

  /**
   * @brief Add a value to a bucket.
   * @param x the bucket's value container
   * @param y the value to add to the bucket
   */
  static constexpr void add(value_container &x, const value_type &y) {
    x.push_back(y);
  }

  /**
   * @brief Append one value container into another.
   * @tparam other_value_container
   * @param x The receiving value container
   * @param y The value container to append
   */
  template <class other_value_container>
  static constexpr void append(value_container &x,
                               const other_value_container &y) {
    for (const auto &value : y) {
      if (!contains(x, value)) {
        x.push_back(value);
      }
    }
  }

  /**
   * @brief Remove a value from a bucket.
   * @param x The value container
   * @param y The value to remove
   * @return Boolean value indicating if the value was removed
   */
  [[nodiscard]] static constexpr bool remove(value_container &x,
                                             const value_type &y) {
    for (auto p = x.begin(); p != x.end(); ++p) {
      if (*p == y) {
        x.erase(p);
        return true;
      }
    }
    return false;
  }

#if __cplusplus >= 202002L
  /**
   * @brief Add a value to a bucket using move semantics.
   * @param x the bucket's value container
   * @param y the value to add to the bucket
   */
  static constexpr void add(value_container &x, value_type &&y) {
    x.push_back(std::move(y));
  }

  /**
   * @brief Append one value container into another using move semantics.
   * @param x The receiving value container
   * @param y The value container to append
   */
  static constexpr void append(value_container &x, value_container &&y) {
    x.splice(x.end(), std::move(y));
  }
#endif // __cplusplus >= 202002L

protected:
  ~bucket_value_traits() = default;
};

} // namespace masutils
