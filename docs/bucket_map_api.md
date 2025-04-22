# bucket_map API Documentation

## Overview

`bucket_map` is a template class that provides a map-like container for storing values associated with ranges of indices. It supports operations like spreading values across ranges, covering ranges with values, and erasing values from ranges.

## Template Parameters

```cpp
template<typename Indices, typename Values, typename Traits = bucket_compare_traits<Indices>, typename ContainerTraits = bucket_value_traits<Values>>
```

- `Indices`: The type used for range indices (e.g., int, size_t)
- `Values`: The type of values to store (e.g., std::string)
- `Traits`: Traits for index comparison (default: bucket_compare_traits<Indices>)
- `ContainerTraits`: Traits for value container (default: bucket_value_traits<Values>)

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
bucket_range<bucket_map<Indices, Values, Traits, ContainerTraits>, false> range(Indices start, Indices end);
```

### Accessor Functions

```cpp
struct accessor {
    // Getters for bucket objects
    template<typename T>
    [[nodiscard]] static constexpr auto& low(T& t) noexcept;
    template<typename T>
    [[nodiscard]] static constexpr const auto& low(const T& t) noexcept;
    template<typename T>
    [[nodiscard]] static constexpr auto& high(T& t) noexcept;
    template<typename T>
    [[nodiscard]] static constexpr const auto& high(const T& t) noexcept;
    template<typename T>
    [[nodiscard]] static constexpr auto& values(T& t) noexcept;
    template<typename T>
    [[nodiscard]] static constexpr const auto& values(const T& t) noexcept;
};
```

## Example Usage

```cpp
#include <bucket/bucket_map.h>
#include <string>
#include <iostream>

int main() {
    masutils::bucket_map<int, std::string> map;
    
    // Spread a value across a range
    map.spread(0, 10, "test");
    
    // Cover a range with a value
    map.cover(5, 15, "new_value");
    
    // Erase values from a range
    map.erase(3, 7);
    
    // Iterate over buckets
    for (const auto& [index, bucket] : map) {
        std::cout << "Range [" << bucket.low() << ", " << bucket.high() 
                  << ") contains: ";
        for (const auto& value : bucket.values()) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    
    // Use range view
    auto range = map.range(5, 15);
    for (const auto& bucket : range) {
        std::cout << "Range [" << bucket.low() << ", " << bucket.high() 
                  << ") contains: ";
        for (const auto& value : bucket.values()) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    }
    
    return 0;
}
```

## Thread Safety

All public member functions are thread-safe. The container uses internal synchronization to ensure safe concurrent access.

## Performance Considerations

- Range operations (spread, cover, erase) have O(log n) complexity
- Iteration is O(n) where n is the number of buckets
- Memory usage is O(n) where n is the number of buckets
- Range views provide efficient iteration over overlapping buckets

## Error Handling

- Range operations with invalid ranges (low > high) will throw std::invalid_argument
- Constrained constructors will throw std::invalid_argument if the range is invalid
- Calling lower_bound() or upper_bound() on an unconstrained map will throw std::runtime_error
- Iterator operations follow standard container iterator rules

## See Also

- [bucket_list API Documentation](bucket_list_api.md)
- [Support Files Documentation](support_files.md) 