#pragma once

#include <concepts>
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

} // namespace masutils 