#pragma once

#include <iterator>
#include <type_traits>

namespace masutils {

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
  using pointer = value_type *;
  using reference = value_type &;

  bucket_iterator_base() = default;
  explicit bucket_iterator_base(typename Container::iterator it) : it_(it) {}
  bucket_iterator_base(const bucket_iterator_base &) = default;
  bucket_iterator_base &operator=(const bucket_iterator_base &) = default;

  // Allow conversion from non-const to const iterator
  template <bool OtherIsConst,
            typename = std::enable_if_t<IsConst && !OtherIsConst>>
  bucket_iterator_base(
      const bucket_iterator_base<Container, ValueType, OtherIsConst,
                                 IteratorCategory> &other)
      : it_(other.get_underlying()) {}

  // Allow conversion from std::list iterator
  template <typename OtherIterator,
            typename = std::enable_if_t<std::is_convertible_v<
                OtherIterator, typename Container::iterator>>>
  bucket_iterator_base(OtherIterator it) : it_(it) {}

  // Basic iterator operations
  reference operator*() const { return *it_; }
  pointer operator->() const { return &(*it_); }

  // Forward iterator requirements
  bucket_iterator_base &operator++() {
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
  bucket_iterator_base &operator--() {
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
  bool operator==(const OtherIterator &other) const {
    if constexpr (std::is_base_of_v<bucket_iterator_base,
                                    std::decay_t<OtherIterator>>) {
      return it_ == other.get_underlying();
    } else {
      return it_ == other;
    }
  }
  template <typename OtherIterator>
  bool operator!=(const OtherIterator &other) const {
    if constexpr (std::is_base_of_v<bucket_iterator_base,
                                    std::decay_t<OtherIterator>>) {
      return it_ != other.get_underlying();
    } else {
      return it_ != other;
    }
  }

protected:
  // Protected access to underlying iterator for derived classes
  typename Container::iterator &get_underlying() { return it_; }
  const typename Container::iterator &get_underlying() const { return it_; }

private:
  typename Container::iterator it_;
};

// Specialization for const iterators
template <typename Container, typename ValueType, typename IteratorCategory>
class bucket_iterator_base<Container, ValueType, true, IteratorCategory> {
public:
  // Add friend declaration for bucket_list
  template <typename Indices, typename Values, typename CompareTraits,
            typename ValueTraits>
  friend class bucket_list;

  using iterator_category = IteratorCategory;
  using value_type = const ValueType;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type *;
  using reference = value_type &;

  bucket_iterator_base() = default;
  explicit bucket_iterator_base(typename Container::const_iterator it)
      : it_(it) {}
  bucket_iterator_base(const bucket_iterator_base &) = default;
  bucket_iterator_base &operator=(const bucket_iterator_base &) = default;

  // Allow conversion from non-const to const iterator
  template <bool OtherIsConst,
            typename = std::enable_if_t<true && !OtherIsConst>>
  bucket_iterator_base(
      const bucket_iterator_base<Container, ValueType, OtherIsConst,
                                 IteratorCategory> &other)
      : it_(other.get_underlying()) {}

  // Allow conversion from std::list iterator
  template <typename OtherIterator,
            typename = std::enable_if_t<std::is_convertible_v<
                OtherIterator, typename Container::const_iterator>>>
  bucket_iterator_base(OtherIterator it) : it_(it) {}

  // Basic iterator operations
  reference operator*() const { return *it_; }
  pointer operator->() const { return &(*it_); }

  // Forward iterator requirements
  bucket_iterator_base &operator++() {
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
  bucket_iterator_base &operator--() {
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
  bool operator==(const OtherIterator &other) const {
    if constexpr (std::is_base_of_v<bucket_iterator_base,
                                    std::decay_t<OtherIterator>>) {
      return it_ == other.get_underlying();
    } else {
      return it_ == other;
    }
  }
  template <typename OtherIterator>
  bool operator!=(const OtherIterator &other) const {
    if constexpr (std::is_base_of_v<bucket_iterator_base,
                                    std::decay_t<OtherIterator>>) {
      return it_ != other.get_underlying();
    } else {
      return it_ != other;
    }
  }

protected:
  // Protected access to underlying iterator for derived classes
  typename Container::const_iterator &get_underlying() { return it_; }
  const typename Container::const_iterator &get_underlying() const {
    return it_;
  }

private:
  typename Container::const_iterator it_;
};

/**
 * @brief Specialization for map containers
 * @tparam Container The underlying map container type
 * @tparam ValueType The type of value being iterated over
 * @tparam IsConst Whether this is a const iterator
 * @tparam IteratorCategory The iterator category
 */
template <typename Container, typename ValueType, bool IsConst,
          typename IteratorCategory = std::bidirectional_iterator_tag>
class bucket_map_iterator_base {
public:
  // Add friend declaration for bucket_map
  template <typename Indices, typename Values, typename CompareTraits,
            typename ValueTraits>
  friend class bucket_map;

  using iterator_category = IteratorCategory;
  using value_type = std::conditional_t<IsConst, const ValueType, ValueType>;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type *;
  using reference = value_type &;

  bucket_map_iterator_base() = default;
  explicit bucket_map_iterator_base(typename Container::iterator it)
      : it_(it) {}
  bucket_map_iterator_base(const bucket_map_iterator_base &) = default;
  bucket_map_iterator_base &
  operator=(const bucket_map_iterator_base &) = default;

  // Allow conversion from non-const to const iterator
  template <bool OtherIsConst,
            typename = std::enable_if_t<IsConst && !OtherIsConst>>
  bucket_map_iterator_base(
      const bucket_map_iterator_base<Container, ValueType, OtherIsConst,
                                     IteratorCategory> &other)
      : it_(other.get_underlying()) {}

  // Map-specific dereference
  reference operator*() const { return it_->second; }
  pointer operator->() const { return &(it_->second); }

  // Forward iterator requirements
  bucket_map_iterator_base &operator++() {
    ++it_;
    return *this;
  }
  bucket_map_iterator_base operator++(int) {
    bucket_map_iterator_base tmp = *this;
    ++it_;
    return tmp;
  }

  // Bidirectional iterator requirements (if supported)
  template <
      typename = std::enable_if_t<
          std::is_same_v<IteratorCategory, std::bidirectional_iterator_tag> ||
          std::is_same_v<IteratorCategory, std::random_access_iterator_tag>>>
  bucket_map_iterator_base &operator--() {
    --it_;
    return *this;
  }
  template <
      typename = std::enable_if_t<
          std::is_same_v<IteratorCategory, std::bidirectional_iterator_tag> ||
          std::is_same_v<IteratorCategory, std::random_access_iterator_tag>>>
  bucket_map_iterator_base operator--(int) {
    bucket_map_iterator_base tmp = *this;
    --it_;
    return tmp;
  }

  // Equality comparison
  bool operator==(const bucket_map_iterator_base &other) const {
    return it_ == other.it_;
  }
  bool operator!=(const bucket_map_iterator_base &other) const {
    return it_ != other.it_;
  }

protected:
  // Protected access to underlying iterator for derived classes
  typename Container::iterator &get_underlying() { return it_; }
  const typename Container::iterator &get_underlying() const { return it_; }

private:
  typename Container::iterator it_;
};

// Specialization for const map iterators
template <typename Container, typename ValueType, typename IteratorCategory>
class bucket_map_iterator_base<Container, ValueType, true, IteratorCategory> {
public:
  // Add friend declaration for bucket_map
  template <typename Indices, typename Values, typename CompareTraits,
            typename ValueTraits>
  friend class bucket_map;

  using iterator_category = IteratorCategory;
  using value_type = const ValueType;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type *;
  using reference = value_type &;

  bucket_map_iterator_base() = default;
  explicit bucket_map_iterator_base(typename Container::const_iterator it)
      : it_(it) {}
  bucket_map_iterator_base(const bucket_map_iterator_base &) = default;
  bucket_map_iterator_base &
  operator=(const bucket_map_iterator_base &) = default;

  // Allow conversion from non-const to const iterator
  template <bool OtherIsConst,
            typename = std::enable_if_t<true && !OtherIsConst>>
  bucket_map_iterator_base(
      const bucket_map_iterator_base<Container, ValueType, OtherIsConst,
                                     IteratorCategory> &other)
      : it_(other.get_underlying()) {}

  // Map-specific dereference
  reference operator*() const { return it_->second; }
  pointer operator->() const { return &(it_->second); }

  // Forward iterator requirements
  bucket_map_iterator_base &operator++() {
    ++it_;
    return *this;
  }
  bucket_map_iterator_base operator++(int) {
    bucket_map_iterator_base tmp = *this;
    ++it_;
    return tmp;
  }

  // Bidirectional iterator requirements (if supported)
  template <
      typename = std::enable_if_t<
          std::is_same_v<IteratorCategory, std::bidirectional_iterator_tag> ||
          std::is_same_v<IteratorCategory, std::random_access_iterator_tag>>>
  bucket_map_iterator_base &operator--() {
    --it_;
    return *this;
  }
  template <
      typename = std::enable_if_t<
          std::is_same_v<IteratorCategory, std::bidirectional_iterator_tag> ||
          std::is_same_v<IteratorCategory, std::random_access_iterator_tag>>>
  bucket_map_iterator_base operator--(int) {
    bucket_map_iterator_base tmp = *this;
    --it_;
    return tmp;
  }

  // Equality comparison
  bool operator==(const bucket_map_iterator_base &other) const {
    return it_ == other.it_;
  }
  bool operator!=(const bucket_map_iterator_base &other) const {
    return it_ != other.it_;
  }

protected:
  // Protected access to underlying iterator for derived classes
  typename Container::const_iterator &get_underlying() { return it_; }
  const typename Container::const_iterator &get_underlying() const {
    return it_;
  }

private:
  typename Container::const_iterator it_;
};

// Alias templates for convenience
template <typename Container, typename ValueType,
          typename IteratorCategory = std::bidirectional_iterator_tag>
using bucket_iterator =
    bucket_iterator_base<Container, ValueType, false, IteratorCategory>;

template <typename Container, typename ValueType,
          typename IteratorCategory = std::bidirectional_iterator_tag>
using bucket_const_iterator =
    bucket_iterator_base<Container, ValueType, true, IteratorCategory>;

template <typename Container, typename ValueType,
          typename IteratorCategory = std::bidirectional_iterator_tag>
using bucket_map_iterator =
    bucket_map_iterator_base<Container, ValueType, false, IteratorCategory>;

template <typename Container, typename ValueType,
          typename IteratorCategory = std::bidirectional_iterator_tag>
using bucket_map_const_iterator =
    bucket_map_iterator_base<Container, ValueType, true, IteratorCategory>;

} // namespace masutils