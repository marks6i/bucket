# bucket_map API Documentation

## Overview

`bucket_map` is a template class that provides a map-like container for storing values associated with ranges of indices. It supports operations like spreading values across ranges, covering ranges with values, and erasing values from ranges.

## Template Parameters

```cpp
template<typename Indices, typename Values, typename CompareTraits = bucket_compare_traits<Indices>, typename ValueTraits = bucket_value_traits<Values>>
```

- `Indices`: The type used for range indices (e.g., int, size_t)
- `Values`: The type of values to store (e.g., std::string)
- `CompareTraits`: Traits for index comparison (default: bucket_compare_traits<Indices>)
- `ValueTraits`: Traits for value container (default: bucket_value_traits<Values>)

## Public Interface

### Constructors

```cpp
// Default constructor (unconstrained)
bucket_map();

// Constructor with range constraints
bucket_map(Indices low, Indices high);
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

// Check if the map is constrained
[[nodiscard]] constexpr bool is_constrained() const noexcept;

// Get the lower bound of a constrained map
[[nodiscard]] index_type lower_bound() const;

// Get the upper bound of a constrained map
[[nodiscard]] index_type upper_bound() const;
```

### Iteration

```cpp
// Iterator types
using iterator = typename bucket_type_map::iterator;
using const_iterator = typename bucket_type_map::const_iterator;
using reverse_iterator = typename bucket_type_map::reverse_iterator;
using const_reverse_iterator = typename bucket_type_map::const_reverse_iterator;

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
bucket_range<bucket_map<Indices, Values, CompareTraits, ValueTraits>, false> range(Indices start, Indices end);
```