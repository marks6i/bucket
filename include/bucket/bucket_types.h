#pragma once

#include <concepts>
#include <type_traits>

namespace masutils {

// Forward declarations
template <typename Container, bool IsConst>
class bucket_range;

template <typename Indices, typename Values, typename CompareTraits, typename ValueTraits>
class bucket_map;

template <typename Indices, typename Values, typename CompareTraits, typename ValueTraits>
class bucket_list;

/**
 * @brief Type trait that checks if a type supports all ordering operations.
 * @tparam T The type to check.
 */
template <typename T>
struct has_ordering_ops {
  template <typename U>
  static auto test(int)
      -> decltype(std::declval<U>() < std::declval<U>(),
                  std::declval<U>() <= std::declval<U>(),
                  std::declval<U>() > std::declval<U>(),
                  std::declval<U>() >= std::declval<U>(), std::true_type());
  template <typename> static auto test(...) -> std::false_type;
  static constexpr bool value = decltype(test<T>(0))::value;
};

/**
 * @brief Type trait that checks if a type supports equality operations.
 * @tparam T The type to check.
 */
template <typename T>
struct has_equality_ops {
  template <typename U>
  static auto test(int)
      -> decltype(std::declval<U>() == std::declval<U>(),
                  std::declval<U>() != std::declval<U>(), std::true_type());
  template <typename> static auto test(...) -> std::false_type;
  static constexpr bool value = decltype(test<T>(0))::value;
};

/**
 * @brief Concept that checks if a type has the required bucket interface.
 * @tparam T The type to check.
 * 
 * This concept ensures that any bucket type provides:
 * - Type definitions for indices and value containers
 * - Accessor methods for range bounds (low/high)
 * - Accessor method for values
 * - Both const and non-const versions of these accessors
 */
template <typename T>
concept has_bucket_interface = requires(T t) {
  typename T::index_type;              // Must have an index type
  typename T::value_container_type;     // Must have a value container type
  { t.low() } -> std::same_as<typename T::index_type &>;         // Must have low()
  { t.high() } -> std::same_as<typename T::index_type &>;        // Must have high()
  { t.values() } -> std::same_as<typename T::value_container_type &>; // Must have values()
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
 * 
 * This concept ensures that container types:
 * - Define their bucket implementation type
 * - Use bucket types that satisfy the bucket interface
 * - Have consistent index types throughout
 */
template <typename T>
concept has_bucket_type = requires {
  typename T::bucket_type;                 // Must have a bucket type
  requires has_bucket_interface<typename T::bucket_type>; // Must satisfy bucket interface
  typename T::index_type;                  // Must have an index type
  requires std::same_as<typename T::index_type,  // Index types must match
                       typename T::bucket_type::index_type>;
};

} // namespace masutils 