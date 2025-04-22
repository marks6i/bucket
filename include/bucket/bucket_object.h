/**
 * @file  bucket_object.h
 * @brief Defines the bucket_object class that represents a bucket with low index, high index, and value container.
 */

#pragma once

#include <utility>

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

    private:
        index_type low_;
        index_type high_;
        value_container_type values_;
    };
} 