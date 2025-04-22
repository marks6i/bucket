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
 * assigning key elements in a bucket container. It uses SFINAE to ensure
 * type safety and provides sensible defaults for common types.
 */

#pragma once

#include <type_traits>
#include <chrono>
#include <concepts>

namespace masutils {

    /**
     * @brief Concept for types that support less-than comparison
     * @tparam T The type to check
     */
    template<typename T>
    concept LessThanComparable = requires(T a, T b) {
        { a < b } -> std::convertible_to<bool>;
    };

    /**
     * @brief Concept for types that support equality comparison
     * @tparam T The type to check
     */
    template<typename T>
    concept EqualityComparable = requires(T a, T b) {
        { a == b } -> std::convertible_to<bool>;
    };

    /**
     * @brief Traits class for comparing and assigning bucket key elements
     * @tparam IndexType The type of the keys in the bucket
     * 
     * This class provides a unified interface for comparing and assigning
     * key elements in a bucket container. It uses C++20 concepts to ensure
     * type safety and provides sensible defaults for common types.
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
        template<typename T = IndexType>
        requires EqualityComparable<T>
        static constexpr bool eq(const T& x, const T& y) noexcept {
            return (x == y);
        }

        /**
         * @brief Compare two elements for less-than relationship
         * @param x First element to compare
         * @param y Second element to compare
         * @return true if x is less than y, false otherwise
         */
        template<typename T = IndexType>
        requires LessThanComparable<T>
        static constexpr bool lt(const T& x, const T& y) noexcept {
            return (x < y);
        }

        /**
         * @brief Assign a value to another
         * @param x The target to assign to
         * @param y The value to assign
         */
        template<typename T = IndexType>
        static constexpr void assign(T& x, const T& y) noexcept {
            x = y;
        }

        /**
         * @brief Assign a value to a reference
         * @param x The target to assign to
         * @param y The value to assign
         */
        template<typename T = IndexType>
        static constexpr void assign(T& x, T&& y) noexcept {
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
