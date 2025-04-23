# bucket_list API Documentation

## Overview

`bucket_list` is a template class that provides a list-like container for storing ordered ranges with associated values. It is optimized for range-based operations and maintains ranges in sorted order.

## Template Parameters

```cpp
template<typename Indices, typename Values, typename CompareTraits = bucket_compare_traits<Indices>, typename ValueTraits = bucket_value_traits<Values>>
ere```

- `Indices`: The type used for range indices (e.g., int, size_t)
- `Values`: The type of values to store (e.g., std::string)
- `CompareTraits`: Traits for index comparison (default: bucket_compare_traits<Indices>)
- `ValueTraits`: Traits for value container (default: bucket_value_traits<Values>)

## Public Interface

### Constructors

```cpp
// Default constructor (unconstrained)
bucket_list();

// Constructor with range constraints
bucket_list(Indices low, Indices high);
```

### Range Operations

```cpp
// Spread a value across a range
[[nodiscard]] int spread(Indices low, Indices high, const Values& value);

// Cover a range with a value
[[nodiscard]] int cover(Indices low, Indices high, const Values& value);

// Erase values from a range
[[nodiscard]] bool erase(Indices low, Indices high);
```

### Accessors

```cpp
// Get the number of buckets
[[nodiscard]] size_t size() const noexcept;

// Check if the list is constrained
[[nodiscard]] constexpr bool is_constrained() const noexcept;

// Get the lower bound of a constrained list
[[nodiscard]] index_type lower_bound() const;

// Get the upper bound of a constrained list
[[nodiscard]] index_type upper_bound() const;
```

### Iteration

```cpp
// Iterator types
using iterator = typename std::list<bucket_type>::iterator;
using const_iterator = typename std::list<bucket_type>::const_iterator;
using reverse_iterator = typename std::list<bucket_type>::reverse_iterator;
using const_reverse_iterator = typename std::list<bucket_type>::const_reverse_iterator;

// Forward iteration
iterator begin() noexcept;
iterator end() noexcept;
const_iterator begin() const noexcept;
const_iterator end() const noexcept;
const_iterator cbegin() const noexcept;
const_iterator cend() const noexcept;

// Reverse iteration
reverse_iterator rbegin() noexcept;
reverse_iterator rend() noexcept;
const_reverse_iterator rbegin() const noexcept;
const_reverse_iterator rend() const noexcept;
const_reverse_iterator crbegin() const noexcept;
const_reverse_iterator crend() const noexcept;
```

### Range View

```cpp
// Create a range view over buckets in [start, end)
bucket_range<bucket_list<Indices, Values, CompareTraits, ValueTraits>, false> range(Indices start, Indices end);
```