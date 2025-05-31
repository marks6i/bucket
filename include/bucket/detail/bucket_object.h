/**
 * @file  bucket_object.h
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
 * @brief Defines the bucket_object class that represents a bucket with low index, high index, and value container.
 */

#pragma once

#include <utility>
#include "./bucket_compare_traits.h"

namespace masutils
{
    /**
     * @brief The bucket_object class represents a bucket with low index, high index, and value container.
     * @tparam IndexType The type of the indices.
     * @tparam ValueContainerType The type of the value container.
     */
    template <class IndexType, class ValueContainerType>
    class bucket_object
    {
    public:
        using index_type = IndexType;
        using value_container_type = ValueContainerType;

        /**
         * @brief Constructor.
         * @param low The low index.
         * @param high The high index.
         * @param values The value container.
         */
        bucket_object(index_type low, index_type high, value_container_type values)
            : low_(low), high_(high), values_(std::move(values))
        {
        }

        /**
         * @brief Constructor that creates a new bucket with default value container.
         * @param low The low index.
         * @param high The high index.
         */
        bucket_object(index_type low, index_type high)
            : low_(low), high_(high), values_()
        {
        }

        /**
         * @brief Get the low index.
         * @return The low index.
         */
        [[nodiscard]] constexpr const index_type& low() const noexcept
        {
            return low_;
        }

        /**
         * @brief Get the low index.
         * @return The low index.
         */
        [[nodiscard]] constexpr index_type& low() noexcept
        {
            return low_;
        }

        /**
         * @brief Set the low index.
         * @param value The new low index value.
         */
        constexpr void set_low(const index_type& value) noexcept
        {
            low_ = value;
        }

        /**
         * @brief Get the high index.
         * @return The high index.
         */
        [[nodiscard]] constexpr const index_type& high() const noexcept
        {
            return high_;
        }

        /**
         * @brief Get the high index.
         * @return The high index.
         */
        [[nodiscard]] constexpr index_type& high() noexcept
        {
            return high_;
        }

        /**
         * @brief Set the high index.
         * @param value The new high index value.
         */
        constexpr void set_high(const index_type& value) noexcept
        {
            high_ = value;
        }

        /**
         * @brief Get the value container.
         * @return The value container.
         */
        [[nodiscard]] constexpr const value_container_type& values() const noexcept
        {
            return values_;
        }

        /**
         * @brief Get the value container.
         * @return The value container.
         */
        [[nodiscard]] constexpr value_container_type& values() noexcept
        {
            return values_;
        }

        /**
         * @brief Compare two bucket_objects for equality based on their ranges.
         * @param other The other bucket_object to compare with.
         * @return True if the bucket_objects have equal ranges, false otherwise.
         */
        [[nodiscard]] constexpr bool operator==(const bucket_object& other) const noexcept
        {
            return bucket_compare_traits<index_type>::eq(low_, other.low_) && 
                   bucket_compare_traits<index_type>::eq(high_, other.high_);
        }

        /**
         * @brief Compare two bucket_objects for inequality based on their ranges.
         * @param other The other bucket_object to compare with.
         * @return True if the bucket_objects have different ranges, false otherwise.
         */
        [[nodiscard]] constexpr bool operator!=(const bucket_object& other) const noexcept
        {
            return !(*this == other);
        }

    private:
        index_type low_;
        index_type high_;
        value_container_type values_;
    };
} 