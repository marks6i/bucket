# Support Files Documentation

This document describes the support files used by the Bucket Template Library.

## Overview

The support files provide common functionality and traits used by both `bucket_map` and `bucket_list` components. These files are designed to be extensible, allowing users to customize behavior for their specific types.

## Files

### bucket_compare_traits.h

The `bucket_compare_traits.h` file provides a unified interface for comparing key elements in a bucket container. It uses C++20 concepts to ensure type safety and provides sensible defaults for common types.

### Key Features

- **Modern C++20 Concepts**: Uses concepts to define type requirements:
  - `LessThanComparable`: Ensures types support less-than comparison
  - `EqualityComparable`: Ensures types support equality comparison

- **Core Comparison Functions**:
  - `eq`: Compares two elements for equality
  - `lt`: Compares two elements for less-than relationship
  - `assign`: Assigns one value to another

- **Descending Order Support**: Provides `bucket_compare_traits_descending` for reverse ordering

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

// Using descending order
using descending_traits = bucket_compare_traits_descending<int>;
```

### bucket_value_traits.h

The `bucket_value_traits.h` file defines operations for working with bucket values, including container type definitions and value containment checks.

### Key Features

- **Container Type Flexibility**: Supports any container type that meets the basic requirements
- **Value Operations**:
  - `contains`: Checks if a value exists in the container
  - `add`: Adds a value to a bucket
  - `append`: Appends one value container into another
  - `remove`: Removes a value from a bucket
- **C++20 Move Semantics Support**:
  - Move-aware `add` and `append` operations
  - Container concepts for type safety

### Usage Example

```cpp
// Using with standard container
using traits = bucket_value_traits<int, std::list<int>>;

// Using with custom container
struct MyContainer {
    void push_back(const int&);
    void insert(iterator, const int&);
    iterator erase(iterator);
    // ... other required operations
};
using custom_traits = bucket_value_traits<int, MyContainer>;
```

### Container Requirements

A container type must support:
- `begin()` and `end()` iterators
- `push_back()` for adding elements
- `insert()` for inserting elements
- `erase()` for removing elements
- Forward iteration capabilities

## Common Usage Pattern

These traits are typically used together in bucket containers:

```cpp
template<typename KeyType, typename ValueType>
class bucket {
    using compare_traits = bucket_compare_traits<KeyType>;
    using value_traits = bucket_value_traits<ValueType>;
    
    // Use compare_traits for key comparisons
    // Use value_traits for value operations
};
```

## Usage Examples

### Custom Comparison Traits

```cpp
struct custom_index {
    int value;
    bool operator<(const custom_index& other) const {
        return value < other.value;
    }
};

namespace masutils {
template<>
struct bucket_compare_traits<custom_index> {
    static bool less(const custom_index& a, const custom_index& b) {
        return a.value < b.value;
    }
    static bool equal(const custom_index& a, const custom_index& b) {
        return a.value == b.value;
    }
    static custom_index min(const custom_index& a, const custom_index& b) {
        return {std::min(a.value, b.value)};
    }
    static custom_index max(const custom_index& a, const custom_index& b) {
        return {std::max(a.value, b.value)};
    }
};
}
```

### Custom Value Traits

```cpp
struct custom_value {
    int id;
    std::string name;
    bool operator==(const custom_value& other) const {
        return id == other.id && name == other.name;
    }
};

namespace masutils {
template<>
struct bucket_value_traits<custom_value> {
    using container_type = std::list<custom_value>;
    
    static void add(container_type& container, const custom_value& value) {
        container.push_back(value);
    }
    
    static bool contains(const container_type& container, const custom_value& value) {
        return std::find(container.begin(), container.end(), value) != container.end();
    }
    
    static void remove(container_type& container, const custom_value& value) {
        container.remove(value);
    }
};
}
```

## Thread Safety

The support files are designed to be thread-safe. All trait methods are static and don't maintain any state. Container operations should be synchronized by the user if accessed from multiple threads.

## Performance Considerations

- Trait methods should be efficient as they are called frequently during container operations
- Container operations (add, remove, contains) should have appropriate complexity for the use case
- Consider using specialized containers (e.g., std::set) for better lookup performance

## Related Documentation

- [bucket_map API Documentation](bucket_map_api.md)
- [bucket_list API Documentation](bucket_list_api.md) 