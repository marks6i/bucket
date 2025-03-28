# buckets.h Legacy API Documentation

> **LEGACY WARNING**: This component is a legacy implementation that has been superseded by `bucket_list.h`. It is maintained for backward compatibility and should not be used in new code. Use `bucket_list.h` instead.

## Overview

`buckets.h` is a legacy template class that provides a basic implementation of a bucket-based container for storing values associated with ranges. This implementation has been replaced by the more feature-rich `bucket_list.h`.

## Template Parameters

```cpp
template<typename Indices, typename Values>
```

- `Indices`: The type used for range indices (e.g., int, size_t)
- `Values`: The type of values to store (e.g., std::string)

## Public Interface

### Constructors

```cpp
// Default constructor
buckets();

// Constructor with range constraints
buckets(Indices low, Indices high);
```

### Range Operations

```cpp
// Add a range with a value
void add(Indices low, Indices high, const Values& value);

// Query values in a range
std::list<Values> query(Indices low, Indices high) const;

// Remove a range
void remove(Indices low, Indices high);
```

### Accessors

```cpp
// Get the number of ranges
size_t size() const;

// Check if the container is empty
bool empty() const;

// Get the lower bound of the first range
Indices low() const;

// Get the upper bound of the last range
Indices high() const;
```

### Iteration

```cpp
// Iterator types
using iterator = typename bucket_type::iterator;
using const_iterator = typename bucket_type::const_iterator;

// Begin/End iterators
iterator begin();
iterator end();
const_iterator begin() const;
const_iterator end() const;
const_iterator cbegin() const;
const_iterator cend() const;
```

## Example Usage

```cpp
#include <bucket/buckets.h>
#include <string>
#include <iostream>

int main() {
    masutils::buckets<int, std::string> buckets;
    
    // Add a range with a value
    buckets.add(0, 10, "first");
    
    // Add another range
    buckets.add(5, 15, "second");
    
    // Query values in a range
    auto values = buckets.query(3, 8);
    
    // Iterate over ranges
    for (const auto& range : buckets) {
        std::cout << "Range [" << range.first << ", " << range.second.first 
                  << ") contains: " << range.second.second << std::endl;
    }
    
    return 0;
}
```

## Limitations

- Limited to C++17 features
- Less efficient than `bucket_list.h`
- No support for advanced range operations
- No thread safety guarantees
- No support for custom comparison or value traits

## Migration Guide

To migrate from `buckets.h` to `bucket_list.h`:

1. Replace the include:
```cpp
// Old
#include <bucket/buckets.h>
// New
#include <bucket/bucket_list.h>
```

2. Update the type:
```cpp
// Old
masutils::buckets<int, std::string>
// New
masutils::bucket_list<int, std::string>
```

3. The API is largely compatible, but `bucket_list.h` provides additional features and better performance.

## See Also

- [bucket_list API Documentation](bucket_list_api.md)
- [bucket_map API Documentation](bucket_map_api.md) 