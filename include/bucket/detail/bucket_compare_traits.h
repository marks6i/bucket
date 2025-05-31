/**
 * @file  bucket_compare_traits.h
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
 * @brief Traits for comparing key elements in a bucket container.
 * 
 * The bucket_compare_traits provides a unified interface for comparing and
 * assigning key elements in a bucket container. It allows for custom
 * specializations to define comparison and assignment operations for types
 * that cannot be modified directly.
 */

#pragma once

#include <type_traits>
#include <chrono>
#include <concepts>
#include <utility>

namespace masutils {

#if __cplusplus >= 202002L
// Define concepts for types that support comparison operations
template<typename T>
concept LessThanComparable = requires(const T& a, const T& b) {
    { a < b } -> std::convertible_to<bool>;
};

template<typename T>
concept EqualityComparable = requires(const T& a, const T& b) {
    { a == b } -> std::convertible_to<bool>;
};
#endif // __cplusplus >= 202002L

/**
 * @brief Traits class for comparing and assigning bucket key elements
 * @tparam IndexType The type of the keys in the bucket
 * 
 * This class provides a unified interface for comparing and assigning
 * key elements in a bucket container. Specializations can be defined
 * to provide custom comparison and assignment logic for types that
 * cannot be modified directly.
 */
template<class IndexType>
struct bucket_compare_traits {
    using index_type = IndexType;

    /**
     * @brief Compare two elements for equality
     * @param x First element to compare
     * @param y Second element to compare
     * @return true if elements are equal, false otherwise
     */
    static constexpr bool eq(const IndexType& x, const IndexType& y) noexcept {
        return (x == y);
    }

    /**
     * @brief Compare two elements for less-than relationship
     * @param x First element to compare
     * @param y Second element to compare
     * @return true if x is less than y, false otherwise
     */
    static constexpr bool lt(const IndexType& x, const IndexType& y) noexcept {
        return (x < y);
    }

    /**
     * @brief Compare two elements for less-than-or-equal relationship
     * @param x First element to compare
     * @param y Second element to compare
     * @return true if x is less than or equal to y, false otherwise
     */
    static constexpr bool le(const IndexType& x, const IndexType& y) noexcept {
        return lt(x, y) || eq(x, y);
    }

    /**
     * @brief Assign a value to another
     * @param x The target to assign to
     * @param y The value to assign
     */
    static constexpr void assign(IndexType& x, const IndexType& y) noexcept {
        x = y;
    }

    /**
     * @brief Assign a value to a reference
     * @param x The target to assign to
     * @param y The source value (rvalue)
     */
    static constexpr void assign(IndexType& x, IndexType&& y) noexcept {
        x = std::move(y);
    }

private:
    bucket_compare_traits() = delete;
};

/**
 * @brief Traits class for descending order comparison
 * @tparam IndexType The type of the keys in the bucket
 */
template<class IndexType>
struct bucket_compare_traits_descending : public bucket_compare_traits<IndexType> {
    static constexpr bool lt(const IndexType& x, const IndexType& y) noexcept {
        return bucket_compare_traits<IndexType>::lt(y, x);
    }
};

} // namespace masutils
