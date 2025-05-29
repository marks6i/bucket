# Support Files Documentation

This document describes the support files used by the Bucket Template Library.

## Overview

The support files provide common functionality and traits used by both `bucket_map` and `bucket_list` components. These files are designed to be extensible, allowing users to customize behavior for their specific types.

## Files

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