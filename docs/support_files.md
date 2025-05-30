# Support Files Documentation

This document describes the support files used by the Bucket Template Library.

## Overview

The support files provide common functionality and traits used by both `bucket_map` and `bucket_list` components. These files are designed to be extensible, allowing users to customize behavior for their specific types.

## Files

### bucket_types.h

This file is fundamental to the library's type safety and interface consistency. It provides a comprehensive type system that includes forward declarations, type traits, and concepts that ensure all bucket containers adhere to the required interfaces.

#### Key Features

- **Forward Declarations**:
  - Declares the main container types (`bucket_map`, `bucket_list`, `bucket_range`)
  - Enables circular references without full includes

- **Type Traits**:
  - `has_ordering_ops<T>`: Checks if a type supports all ordering operations (<, <=, >, >=)
  - `has_equality_ops<T>`: Checks if a type supports equality operations (==, !=)

- **Core Concepts**:
  - `has_bucket_interface`: A C++20 concept that defines the required interface for any bucket type:
    ```cpp
    template <typename T>
    concept has_bucket_interface = requires(T t) {
      typename T::index_type;              // Must have an index type
      typename T::value_container_type;     // Must have a value container type
      { t.low() } -> std::same_as<typename T::index_type &>;         // Must have low()
      { t.high() } -> std::same_as<typename T::index_type &>;        // Must have high()
      { t.values() } -> std::same_as<typename T::value_container_type &>; // Must have values()
    };
    ```
    This concept ensures that any type used as a bucket must provide:
    - Type definitions for indices and value containers
    - Accessor methods for the range bounds (low/high)
    - Accessor method for the values
    - Both const and non-const versions of these accessors

  - `has_bucket_type`: A concept that ensures container types properly expose their bucket implementation:
    ```cpp
    template <typename T>
    concept has_bucket_type = requires {
      typename T::bucket_type;                 // Must have a bucket type
      requires has_bucket_interface<typename T::bucket_type>; // Must satisfy bucket interface
      typename T::index_type;                  // Must have an index type
      requires std::same_as<typename T::index_type,  // Index types must match
                          typename T::bucket_type::index_type>;
    };
    ```
    This concept ensures that container types:
    - Define their bucket implementation type
    - Use bucket types that satisfy the bucket interface
    - Have consistent index types throughout

#### Interface Guarantees

The type system in `bucket_types.h` provides several important guarantees:

1. **Type Safety**: All bucket-related types must explicitly define their index and value container types.
2. **Interface Consistency**: All bucket types must provide the same core interface (low, high, values).
3. **Const Correctness**: Both const and non-const access patterns are enforced.
4. **Type Compatibility**: Index types must be consistent between containers and their buckets.

#### Usage in Container Classes

The type system is used throughout the library to ensure type safety:

```cpp
// bucket_map and bucket_list both satisfy has_bucket_type
static_assert(has_bucket_type<bucket_map<int, std::string>>);
static_assert(has_bucket_type<bucket_list<int, std::string>>);

// bucket_object satisfies has_bucket_interface
static_assert(has_bucket_interface<bucket_object<int, std::vector<std::string>>>);

// bucket_range works with any container satisfying has_bucket_type
bucket_range<bucket_map<int, std::string>, false> range;  // non-const range
bucket_range<bucket_list<int, std::string>, true> const_range;  // const range
```

### bucket_compare_traits.h

Provides a unified interface for comparing key elements in a bucket container. It uses C++20 concepts to ensure type safety and provides sensible defaults for common types.

### Key Features

- **Type Requirements**:
  - Must be either arithmetic or a class type supporting comparison operations
  - If class type, must support `lt` and `eq` operations through the traits

- **Core Comparison Functions**:
  - `lt`: Compares two elements for less-than relationship
  - `eq`: Compares two elements for equality
  - `assign`: Assigns one value to another

### bucket_value_traits.h

Defines operations for working with bucket values, including container type definitions and value operations.

### Key Features

- **Container Type Definition**: Defines the container type used to store values
- **Value Operations**:
  - `add`: Adds a value to a container
  - `append`: Appends one container's values to another

### bucket_object.h

Defines the core bucket type used by both `bucket_map` and `bucket_list`. Each bucket represents a non-overlapping range on an ordered axis.

### Key Features

- **Range Representation**: 
  - `low()`: Lower bound of the range
  - `high()`: Upper bound of the range
  - `values()`: Container of values in the range

### bucket_range.h

Provides range-based operations and iterators for bucket containers.

### Key Features

- **Range Operations**:
  - `spread`: Spreads a value across a range
  - `cover`: Covers a range with a value
  - `erase`: Erases values from a range

### bucket_iterator.h

Implements iterator types for bucket containers.

### Key Features

- **Iterator Types**:
  - `bucket_iterator_base`: Base iterator template
  - Support for both const and non-const iteration
  - Forward and reverse iteration support

### bucket_traits.h

Defines common traits and concepts used across the library.

### Key Features

- **Type Traits**:
  - Type checking and validation
  - Common type definitions

### buckets_supp.h

Provides supplementary functionality used by bucket containers.

### Key Features

- **Support Functions**:
  - Range manipulation
  - Container operations
  - Error handling

## Common Usage Pattern

```cpp
// Using with standard types
using compare_traits = bucket_compare_traits<int>;
using value_traits = bucket_value_traits<std::string>;

// Using with custom types
struct MyType {
    bool operator<(const MyType& other) const;
    bool operator==(const MyType& other) const;
};

using custom_compare_traits = bucket_compare_traits<MyType>;
using custom_value_traits = bucket_value_traits<std::string, std::vector<std::string>>;

// Creating a bucket container
bucket_list<int, std::string> list;
bucket_map<MyType, std::string> map;
```

## Thread Safety

The support files themselves are thread-safe as they contain only static methods and no shared state. However, bucket containers should be synchronized by the user when accessed from multiple threads.

## Error Handling

- Invalid range bounds throw `std::invalid_argument`
- Accessing bounds of unconstrained containers throws `std::runtime_error`

## Performance Considerations

- Trait operations are designed to be efficient and inlined where possible
- Container operations maintain optimal complexity
- Iterator operations are optimized for traversal 