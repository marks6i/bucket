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

// Common type traits
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

template <typename T>
struct has_equality_ops {
  template <typename U>
  static auto test(int)
      -> decltype(std::declval<U>() == std::declval<U>(),
                  std::declval<U>() != std::declval<U>(), std::true_type());
  template <typename> static auto test(...) -> std::false_type;
  static constexpr bool value = decltype(test<T>(0))::value;
};

// Common concepts
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

template <typename T>
concept has_bucket_type = requires {
  typename T::bucket_type;
  requires has_bucket_interface<typename T::bucket_type>;
  typename T::index_type;
  requires std::same_as<typename T::index_type,
                       typename T::bucket_type::index_type>;
};

} // namespace masutils 