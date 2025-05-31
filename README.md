# Bucket Template Library

A modern C++20 template library for managing ranges with associated values. The library provides two main components:

- `bucket_map`: A map-like container for storing values associated with ranges of indices
- `bucket_list`: A list-like container for storing ordered ranges with associated values

## Features

Common features for both components:
- Range-based operations (spread, cover, erase)
- Efficient storage and retrieval of values associated with ranges
- Support for custom index and value types
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

### Clean Build Process

To perform a clean build with all the correct settings:

```bash
# Remove existing build directory if it exists
rm -rf build    # On Unix-like systems
# OR
rmdir /s /q build    # On Windows

# Create new build directory with correct settings
cmake -B build \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug

# Build the project
cmake --build build --config Debug

# Run tests
cd build
ctest -C Debug --output-on-failure
```

### Build Options

- `CMAKE_BUILD_TYPE`: Set to `Debug` for development, `Release` for production
- `CMAKE_CXX_STANDARD`: Set to `20` for C++20 features
- `CMAKE_CXX_STANDARD_REQUIRED`: Set to `ON` to enforce C++20
- `CMAKE_MSVC_RUNTIME_LIBRARY`: 
  - `MultiThreadedDebug` for Debug builds
  - `MultiThreaded` for Release builds

### Quick Build (for development)

For quick rebuilds during development, you can use:

```bash
# If build directory already exists with correct settings
cmake --build build --config Debug

# Run tests
cd build
ctest -C Debug --output-on-failure
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

This project is licensed under the Apache License, Version 2.0 - see the LICENSE file for details.

## Installation

The bucket library is header-only, which means there are no binaries to compile or link against. You just need to include the header files in your project.

### Option 1: Download from GitHub Releases

1. Visit the [Releases page](https://github.com/yourusername/bucket/releases) of the bucket library
2. Download the latest release package `bucket-x.y.z.tar.gz`
3. Extract the package:
   ```bash
   tar xzf bucket-x.y.z.tar.gz
   ```

### Option 2: Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/bucket.git
   ```

2. Optional: Create source package
   ```bash
   mkdir build && cd build
   cmake ..
   cpack --config CPackSourceConfig.cmake
   ```

### Using with CMake Projects

Add the library to your project using one of these methods:

1. **FetchContent** (recommended for version control):
   ```cmake
   include(FetchContent)
   FetchContent_Declare(
       bucket
       GIT_REPOSITORY https://github.com/yourusername/bucket.git
       GIT_TAG v1.0.0  # Specify the version you want
   )
   FetchContent_MakeAvailable(bucket)
   
   target_link_libraries(your_target PRIVATE bucket::bucket)
   ```

2. **find_package** (if installed system-wide):
   ```cmake
   find_package(bucket 1.0.0 REQUIRED)
   target_link_libraries(your_target PRIVATE bucket::bucket)
   ```

### Using without CMake

Since this is a header-only library, you can simply:

1. Copy the headers to your project:
   - Copy the `include/bucket` directory to your project's include path
   - Or copy to a system-wide location:
     ```bash
     sudo cp -r include/bucket /usr/local/include/
     ```

2. Include in your C++ code:
   ```cpp
   #include <bucket/bucket_list.h>
   #include <bucket/bucket_map.h>
   ```

3. Compile with C++20 support:
   ```bash
   g++ -std=c++20 your_code.cpp -I/path/to/bucket/include
   ```

### Documentation

Documentation can be found in the `docs/` directory of the source package.
