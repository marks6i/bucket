# Bucket Map Library

A C++ library that provides a map-like container with bucket-based range operations. This library allows you to store values associated with ranges of indices and perform operations like spreading values across ranges and covering ranges with values.

## Features

- Range-based value storage
- Spread operation for distributing values across ranges
- Cover operation for setting values across ranges
- Erase operation for removing values from ranges
- Automatic range ordering
- Constrained range operations
- Thread-safe operations

## Prerequisites

- C++20 compliant compiler
- CMake 3.15 or higher
- Git (for cloning the repository)

### Windows-Specific Requirements

- Visual Studio 2022 with C++ development tools (recommended)
- Windows SDK 10.0 or higher
- PowerShell 7 or higher (recommended for better command-line experience)

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/yourusername/bucket.git
cd bucket
```

2. Create a build directory and configure the project:
```bash
mkdir build
cd build
cmake ..
```

3. Build the project:
```bash
cmake --build .
```

### Building Tests

The project includes Google Test for unit testing. Tests are built automatically with the main project. To run the tests:

```bash
# From the build directory
ctest --output-on-failure
```

Or run individual test executables:
```bash
# On Windows
.\Debug\bucket_map_test.exe
.\Debug\bucket_list_test.exe

# On Unix-like systems
./Debug/bucket_map_test
./Debug/bucket_list_test
```

### Continuous Integration

This project uses GitHub Actions for continuous integration. The CI pipeline:
- Runs on push to main branch and pull requests
- Tests on multiple platforms (Ubuntu, Windows, macOS)
- Builds and tests both Debug and Release configurations
- Automatically downloads and builds dependencies (like Google Test)

To run the same tests locally as the CI pipeline:
```bash
# Configure with the same build type as CI
cmake -B build -DCMAKE_BUILD_TYPE=Debug
# or
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build and test
cmake --build build
ctest --test-dir build --output-on-failure
```

## Usage

Include the header files in your project:

```cpp
#include <bucket/bucket_map.h>
#include <bucket/bucket_list.h>
```

Example usage:

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

## Project Structure

```
bucket/
├── include/
│   └── bucket/
│       ├── bucket_map.h
│       ├── bucket_list.h
│       └── buckets.h
├── tests/
│   ├── bucket_map_test.cpp
│   └── bucket_list_test.cpp
├── external/
│   └── googletest/    # Google Test library
├── CMakeLists.txt
└── README.md
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details. 