# Support Files Documentation

This document describes the support files used by the Bucket Template Library.

## Overview

The support files provide common functionality and traits used by both `bucket_map` and `bucket_list` components. These files are designed to be extensible, allowing users to customize behavior for their specific types.

## Files

### bucket_compare_traits.h

The `bucket_compare_traits.h` file provides a unified interface for comparing key elements in a bucket container. It uses C++20 concepts to ensure type safety and provides sensible defaults for common types.

### Key Features

- **Modern C++20 Concepts**: Uses concepts to define type requirements:
  - `std::totally_ordered`: Ensures types support total ordering
  - `std::equality_comparable`: Ensures types support equality comparison

- **Core Comparison Functions**:
  - `eq`: Compares two elements for equality
  - `lt`: Compares two elements for less-than relationship
  - `assign`: Assigns one value to another (both lvalue and rvalue references)

### Usage Example

```cpp
// Using with standard types
using traits = bucket_compare_traits<int>;

// Using with custom types (must implement < and == operators)
struct MyType {
    bool operator<(const MyType& other) const;
    bool operator==(const MyType& other) const;
};
using custom_traits = bucket_compare_traits<MyType>;
```

### bucket_value_traits.h

The `bucket_value_traits.h` file defines operations for working with bucket values, including container type definitions and value containment checks.

### Key Features

- **Container Type Flexibility**: Supports any container type that meets the basic requirements
- **Value Operations**:
  - `add`: Adds a value to a container
  - `append`: Appends one container's values to another
- **Default Container Type**: Uses `std::list<ValueType>` by default

### Usage Example

```cpp
// Using with standard container
using traits = bucket_value_traits<int>;

// Using with custom container type
using custom_traits = bucket_value_traits<int, std::vector<int>>;
```

### bucket_object.h

The `bucket_object.h` file defines the core bucket type used by both `bucket_map` and `bucket_list`. It represents a bucket with low and high indices and a value container.

### Key Features

- **Range Representation**: Stores low and high indices defining a range
- **Value Storage**: Contains a value container for storing associated values
- **Accessor Methods**:
  - `low()`: Returns the lower bound of the range
  - `high()`: Returns the upper bound of the range
  - `values()`: Returns the value container

### Usage Example

```cpp
using bucket_type = bucket_object<int, std::list<std::string>>;
bucket_type bucket(1, 10, {"value1", "value2"});
```

### bucket_range.h

The `bucket_range.h` file provides a range view over a bucket container, allowing iteration over buckets that overlap with a specified range.

### Key Features

- **Range View**: Provides a view over buckets in a specified range
- **Iterator Support**: 
  - Forward and reverse iteration
  - Const and non-const iterators
- **Template Parameters**:
  - `Container`: The type of bucket container (bucket_map or bucket_list)
  - `IsConst`: Boolean indicating if the iterator is const

### Usage Example

```cpp
bucket_map<int, std::string> map;
// ... add some buckets ...
auto range = map.range(5, 15);
for (const auto& bucket : range) {
    // Process buckets in range [5, 15)
}
```

## Common Usage Pattern

These traits and support files are typically used together in bucket containers:

```cpp
template<typename Indices, typename Values>
class bucket_container {
    using compare_traits = bucket_compare_traits<Indices>;
    using value_traits = bucket_value_traits<Values>;
    using bucket_type = bucket_object<Indices, typename value_traits::value_container>;
    
    // Use compare_traits for index comparisons
    // Use value_traits for value operations
    // Use bucket_type for storing ranges and values
};
```

## Thread Safety

The support files are designed to be thread-safe. All trait methods are static and don't maintain any state. Container operations should be synchronized by the user if accessed from multiple threads.

## Performance Considerations

- Trait methods should be efficient as they are called frequently during container operations
- Container operations (add, append) should have appropriate complexity for the use case
- Range operations use efficient algorithms for finding overlapping buckets

## Related Documentation

- [bucket_map API Documentation](bucket_map_api.md)
- [bucket_list API Documentation](bucket_list_api.md) 