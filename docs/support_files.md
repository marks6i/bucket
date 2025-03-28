# Support Files Documentation

This document describes the support files used by the Bucket Template Library.

## Overview

The library includes several support files that provide common functionality and traits for the main components:

- `buckets.h`: Common bucket functionality and types
- `bucket_compare_traits.h`: Traits for index comparison
- `bucket_value_traits.h`: Traits for value containers

## buckets.h

### Purpose

Defines common types and functionality used by both `bucket_map` and `bucket_list`.

### Key Components

```cpp
// Common bucket type used by both containers
template<typename Indices, typename Values>
struct bucket_type {
    Indices low;
    Indices high;
    Values values;
};

// Common accessor functions
template<typename Bucket>
struct bucket_accessor {
    static Indices low(const Bucket& bucket);
    static Indices high(const Bucket& bucket);
    static const Values& values(const Bucket& bucket);
};
```

## bucket_compare_traits.h

### Purpose

Provides traits for comparing indices in range operations.

### Key Components

```cpp
template<typename T>
struct bucket_compare_traits {
    // Comparison operators
    static bool less(const T& a, const T& b);
    static bool equal(const T& a, const T& b);
    static bool greater(const T& a, const T& b);
    
    // Range operations
    static T min(const T& a, const T& b);
    static T max(const T& a, const T& b);
};
```

### Specializations

- Built-in numeric types (int, float, double, etc.)
- std::chrono::time_point
- Custom types can be specialized as needed

## bucket_value_traits.h

### Purpose

Defines traits for value containers used in buckets.

### Key Components

```cpp
template<typename T>
struct bucket_value_traits {
    // Container type
    using container_type = std::list<T>;
    
    // Container operations
    static void add(container_type& container, const T& value);
    static void remove(container_type& container, const T& value);
    static bool contains(const container_type& container, const T& value);
};
```

### Specializations

- Single value containers (default)
- Set-based containers
- Custom containers can be specialized as needed

## Usage Examples

### Custom Comparison Traits

```cpp
struct custom_index {
    int value;
};

template<>
struct bucket_compare_traits<custom_index> {
    static bool less(const custom_index& a, const custom_index& b) {
        return a.value < b.value;
    }
    // ... other required methods
};
```

### Custom Value Traits

```cpp
template<>
struct bucket_value_traits<std::string> {
    using container_type = std::set<std::string>;
    
    static void add(container_type& container, const std::string& value) {
        container.insert(value);
    }
    // ... other required methods
};
```

## Thread Safety

All support files are designed to be thread-safe when used with the main components. The traits classes are stateless and their methods are thread-safe by design.

## Performance Considerations

- Trait methods should be lightweight and efficient
- Container operations should have O(log n) or better complexity
- Memory usage should be optimized for the specific use case

## See Also

- [bucket_map API Documentation](bucket_map_api.md)
- [bucket_list API Documentation](bucket_list_api.md) 