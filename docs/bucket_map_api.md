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
// Default constructor
bucket_map();

// Constructor with range constraints
bucket_map(Indices low, Indices high);
```

### Range Operations

```cpp
// Spread a value across a range
int spread(Indices low, Indices high, const Values& value);

// Cover a range with a value
void cover(Indices low, Indices high, const Values& value);

// Erase values from a range
void erase(Indices low, Indices high);
```

### Accessors

```cpp
// Get the number of buckets
size_t size() const;

// Check if the map is empty
bool empty() const;

// Get the lower bound of the first bucket
Indices low() const;

// Get the upper bound of the last bucket
Indices high() const;
```

### Iteration

```cpp
// Iterator types
using iterator = typename bucket_map_type::iterator;
using const_iterator = typename bucket_map_type::const_iterator;

// Begin/End iterators
iterator begin();
iterator end();
const_iterator begin() const;
const_iterator end() const;
const_iterator cbegin() const;
const_iterator cend() const;
```

### Accessor Functions

```cpp
// Get the lower bound of a bucket
static Indices low(const bucket_type& bucket);

// Get the upper bound of a bucket
static Indices high(const bucket_type& bucket);

// Get the values in a bucket
static const value_container_type& values(const bucket_type& bucket);
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
    for (const auto& bucket : map) {
        std::cout << "Range [" << bucket.first << ", " << bucket.second.first 
                  << ") contains: ";
        for (const auto& value : bucket.second.second) {
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

## Error Handling

- Range operations with invalid ranges (low > high) will throw std::invalid_argument
- Constrained constructors will throw std::invalid_argument if the range is invalid
- Iterator operations follow standard container iterator rules

## See Also

- [bucket_list API Documentation](bucket_list_api.md)
- [Support Files Documentation](support_files.md) 