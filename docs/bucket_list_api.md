# bucket_list API Documentation

## Overview

`bucket_list` is a template class that provides a list-like container for storing ordered ranges with associated values. Each bucket represents a non-overlapping range on an ordered axis, with gaps allowed if there are no values in the given range.

## Template Parameters

```cpp
template<typename Indices, typename Values, typename CompareTraits = bucket_compare_traits<Indices>, typename ValueTraits = bucket_value_traits<Values>>
```

- `Indices`: The type used for range indices (must be either arithmetic or a class type supporting CompareTraits operations)
- `Values`: The type of values to store
- `CompareTraits`: Traits for index comparison (default: bucket_compare_traits<Indices>)
- `ValueTraits`: Traits for value container (default: bucket_value_traits<Values>)

## Public Types

```cpp
using index_type = Indices;
using value_type = Values;
using value_container = typename ValueTraits::value_container;
using const_value_container = const typename ValueTraits::value_container;
using bucket_type = bucket_object<index_type, value_container>;
```

## Public Interface

### Constructors

```cpp
// Default constructor (unconstrained)
explicit bucket_list() noexcept;

// Constructor with range constraints
explicit bucket_list(index_type low, index_type high);

// Default destructor
~bucket_list() = default;

// Default move assignment operator
bucket_list& operator=(bucket_list&&) noexcept = default;
```

### Range Operations

```cpp
// Spread a value across a range
[[nodiscard]] int spread(index_type low, index_type high, value_type value);

// Cover a range with a value
[[nodiscard]] int cover(index_type low, index_type high, value_type value);

// Erase values from a range
[[nodiscard]] bool erase(index_type low, index_type high);

// Erase all values
[[nodiscard]] bool erase();

// Find a range
[[nodiscard]] iterator find_range(index_type low, index_type high);

// Find a bucket containing a specific index
[[nodiscard]] iterator find(index_type index);
[[nodiscard]] const_iterator find(index_type index) const;
```

### Accessors

```cpp
// Get the number of buckets
[[nodiscard]] constexpr std::size_t size() const noexcept;

// Check if the list is empty
[[nodiscard]] constexpr bool empty() const noexcept;

// Check if the list is constrained
[[nodiscard]] constexpr bool constrained() const noexcept;

// Get the lower bound of a constrained list (throws if unconstrained)
[[nodiscard]] index_type low() const;
[[nodiscard]] index_type lower_bound() const;

// Get the upper bound of a constrained list (throws if unconstrained)
[[nodiscard]] index_type high() const;
[[nodiscard]] index_type upper_bound() const;
```

### Iteration

```cpp
// Iterator types
using iterator = bucket_iterator_base<bucket_type_list, bucket_type, false>;
using const_iterator = bucket_iterator_base<bucket_type_list, bucket_type, true>;
using reverse_iterator = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;

// Forward iteration
[[nodiscard]] iterator begin();
[[nodiscard]] const_iterator begin() const;
[[nodiscard]] const_iterator cbegin() const;
[[nodiscard]] iterator end();
[[nodiscard]] const_iterator end() const;
[[nodiscard]] const_iterator cend() const;

// Reverse iteration
[[nodiscard]] reverse_iterator rbegin();
[[nodiscard]] const_reverse_iterator rbegin() const;
[[nodiscard]] const_reverse_iterator crbegin() const;
[[nodiscard]] reverse_iterator rend();
[[nodiscard]] const_reverse_iterator rend() const;
[[nodiscard]] const_reverse_iterator crend() const;
```


## Error Handling

- Throws `std::invalid_argument` if high is less than low in constrained constructor
- Throws `std::runtime_error` when accessing bounds of an unconstrained list