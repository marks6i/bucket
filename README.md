# Bucket Template Library

A modern C++20 template library for managing ranges with associated values. The library provides two main components:

- `bucket_map`: A map-like container for storing values associated with ranges of indices
- `bucket_list`: A list-like container for storing ordered ranges with associated values

## Features

Common features for both components:
- Range-based operations (spread, cover, erase)
- Efficient storage and retrieval of values associated with ranges
- Support for custom index and value types
- Thread-safe operations
- Modern C++20 design

### bucket_map
- Map-like interface for range-value associations
- Efficient lookup of values at specific indices
- Support for overlapping ranges
- Automatic range splitting and merging

### bucket_list
- List-like interface for ordered ranges
- Optimized for range-based operations
- Maintains sorted order of ranges
- Support for multiple values per range

## Prerequisites

- C++20 compliant compiler
- CMake 3.15 or higher
- Google Test (automatically downloaded by CMake)

## Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running Tests

```bash
# Windows
.\Debug\bucket_list_test.exe
.\Debug\bucket_map_test.exe
.\Debug\bucket_compare_traits_test.exe
.\Debug\bucket_value_traits_test.exe

# Unix-like systems
./bucket_list_test
./bucket_map_test
./bucket_compare_traits_test
./bucket_value_traits_test
```

## Continuous Integration

The project uses GitHub Actions for CI. The pipeline:
- Builds on Windows and Linux
- Runs all tests
- Checks for C++20 compliance
- Validates CMake configuration

## Usage

### bucket_map

```cpp
#include <bucket/bucket_map.h>

masutils::bucket_map<int, std::string> map;

// Spread a value across a range
map.spread(0, 5, "test1");

// Cover a range with a value
map.cover(2, 4, "test2");

// Query values at a point
auto values = map.query(3);  // Returns both "test1" and "test2"

// Erase values from a range
map.erase(1, 3);
```

### bucket_list

```cpp
#include <bucket/bucket_list.h>

masutils::bucket_list<int, std::string> list;

// Add a range with a value
list.spread(0, 5, "test1");

// Cover a range with a value
list.cover(2, 4, "test2");

// Query values at a point
auto values = list.query(3);  // Returns both "test1" and "test2"

// Erase values from a range
list.erase(1, 3);
```

## Project Structure

```
bucket/
├── include/
│   └── bucket/
│       ├── bucket_map.h
│       ├── bucket_list.h
│       ├── bucket_compare_traits.h
│       └── bucket_value_traits.h
├── tests/
│   ├── bucket_map_test.cpp
│   ├── bucket_list_test.cpp
│   ├── bucket_compare_traits_test.cpp
│   └── bucket_value_traits_test.cpp
├── docs/
│   ├── bucket_map_api.md
│   ├── bucket_list_api.md
│   └── support_files.md
├── CMakeLists.txt
└── README.md
```

## Documentation

- [bucket_map API Documentation](docs/bucket_map_api.md)
- [bucket_list API Documentation](docs/bucket_list_api.md)
- [Support Files Documentation](docs/support_files.md)

## Contributing

1. Fork the repository
2. Create a feature branch from `develop`
3. Make your changes
4. Run tests and ensure they pass
5. Submit a pull request to `develop`

## License

This project is licensed under the MIT License - see the LICENSE file for details. 
