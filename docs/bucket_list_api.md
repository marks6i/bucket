# bucket_list API Documentation

## Overview

`bucket_list` is a template class that provides a list-like container for storing ordered ranges with associated values. It is optimized for range-based operations and maintains ranges in sorted order.

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
bucket_list();

// Constructor with range constraints
bucket_list(Indices low, Indices high);
```

### Range Operations

```cpp
// Add a range with a value
void add(Indices low, Indices high, const Values& value);

// Query values in a range
value_container_type query(Indices low, Indices high) const;

// Remove a range
void remove(Indices low, Indices high);
```

### Accessors

```cpp
// Get the number of ranges
size_t size() const;

// Check if the list is empty
bool empty() const;

// Get the lower bound of the first range
Indices low() const;

// Get the upper bound of the last range
Indices high() const;
```

### Iteration

```cpp
// Iterator types
using iterator = typename bucket_list_type::iterator;
using const_iterator = typename bucket_list_type::const_iterator;

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
// Get the lower bound of a range
static Indices low(const bucket_type& bucket);

// Get the upper bound of a range
static Indices high(const bucket_type& bucket);

// Get the value in a range
static const Values& value(const bucket_type& bucket);
```

## Example Usage

```cpp
#include <bucket/bucket_list.h>
#include <string>
#include <iostream>

int main() {
    masutils::bucket_list<int, std::string> list;
    
    // Add a range with a value
    list.add(0, 10, "first");
    
    // Add another range
    list.add(5, 15, "second");
    
    // Query values in a range
    auto values = list.query(3, 8);
    
    // Iterate over ranges
    for (const auto& range : list) {
        std::cout << "Range [" << range.first << ", " << range.second.first 
                  << ") contains: " << range.second.second << std::endl;
    }
    
    return 0;
}
```

## Thread Safety

All public member functions are thread-safe. The container uses internal synchronization to ensure safe concurrent access.

## Performance Considerations

- Range operations (add, query, remove) have O(log n) complexity
- Iteration is O(n) where n is the number of ranges
- Memory usage is O(n) where n is the number of ranges

## Error Handling

- Range operations with invalid ranges (low > high) will throw std::invalid_argument
- Constrained constructors will throw std::invalid_argument if the range is invalid
- Iterator operations follow standard container iterator rules

## See Also

- [bucket_map API Documentation](bucket_map_api.md)
- [Support Files Documentation](support_files.md) 