# Developer Guide for Bucket Template Library

This document provides detailed instructions for developers working on the Bucket Template Library, including how to build, test, and create releases.

## Development Environment Setup

### Prerequisites
- C++20 compliant compiler (e.g., GCC 10+, Clang 10+, MSVC 2019+)
- CMake 3.15 or higher
- Git
- CPack (comes with CMake)
- CTest (comes with CMake)

## Building the Project

### Debug Build (for development)

```bash
# Create and enter build directory
mkdir -p build && cd build

# Configure with Debug settings
cmake -B . \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug \
    ..

# Build
cmake --build . --config Debug
```

### Release Build (for production/releases)

```bash
# Create and enter build directory
mkdir -p build-release && cd build-release

# Configure with Release settings
cmake -B . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_STANDARD=20 \
    -DCMAKE_CXX_STANDARD_REQUIRED=ON \
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded \
    ..

# Build
cmake --build . --config Release
```

## Running Tests

Tests are automatically built with the project. To run them:

```bash
# From the build directory
ctest -C Debug --output-on-failure  # For debug builds
# or
ctest -C Release --output-on-failure  # For release builds
```

To run specific test executables directly:

```bash
# From the build directory
./bucket_list_test
./bucket_map_test
./bucket_compare_traits_test
./bucket_value_traits_test
```

## Creating Releases

The Bucket Template Library uses CMake's installation and packaging system to create releases. Here's the process for creating and publishing a new release:

### 1. Prepare for Release

1. Update version numbers:
   - In `CMakeLists.txt`, update the project version:
     ```cmake
     project(bucket VERSION X.Y.Z)
     ```
   - Update any version references in documentation

2. Ensure all tests pass:
   ```bash
   mkdir -p build && cd build
   cmake ..
   cmake --build .
   ctest --output-on-failure
   ```

### 2. Create Release Packages

1. Generate source package:
   ```bash
   # From the build directory
   cpack --config CPackSourceConfig.cmake
   ```

   This will create:
   - `bucket-X.Y.Z.tar.gz` - Source archive
   - `bucket-X.Y.Z.zip` - Source archive (Windows-friendly)

2. Generate binary package (if applicable):
   ```bash
   # From the build directory
   cpack
   ```

### 3. Publishing on GitHub

1. Create a new tag:
   ```bash
   git tag -a vX.Y.Z -m "Release version X.Y.Z"
   git push origin vX.Y.Z
   ```

2. Create a new release on GitHub:
   - Go to the repository's "Releases" page
   - Click "Create a new release"
   - Select the tag you just created
   - Title: "Bucket Template Library vX.Y.Z"
   - Description: Include:
     - Major changes and features
     - Breaking changes (if any)
     - Bug fixes
     - Upgrade instructions (if needed)
   
3. Upload the release artifacts:
   - Upload the source packages (`*.tar.gz` and `*.zip`)
   - Upload any binary packages if created
   - Upload the generated documentation

### 4. Post-Release

1. Update the main branch with the release version
2. Create a new development version (if needed)
3. Update the installation instructions in README.md if necessary

## Installation Artifacts

The following files are created during the release process:

```
bucket-X.Y.Z/
├── include/
│   └── bucket/
│       ├── bucket_list.h         # Public API
│       ├── bucket_map.h          # Public API
│       ├── buckets_supp.h        # Public API
│       └── detail/               # Implementation details
│           ├── bucket_compare_traits.h
│           ├── bucket_fwd.h
│           ├── bucket_iterator.h
│           ├── bucket_object.h
│           ├── bucket_range.h
│           ├── bucket_traits.h
│           ├── bucket_types.h
│           └── bucket_value_traits.h
├── docs/
│   ├── bucket_map_api.md
│   ├── bucket_list_api.md
│   └── support_files.md
├── CMakeLists.txt
├── LICENSE
└── README.md
```

The header files are organized as follows:
- Public API headers (in `include/bucket/`):
  - `bucket_list.h`: Main bucket list container
  - `bucket_map.h`: Main bucket map container
  - `buckets_supp.h`: Supplemental utilities and traits
- Implementation headers (in `include/bucket/detail/`):
  - Internal implementation details
  - Not intended for direct use by library users
  - May change between versions without notice

## CMake Integration

The library provides CMake configuration files that allow users to integrate it using `find_package()`:

```cmake
find_package(bucket REQUIRED)
target_link_libraries(your_target PRIVATE bucket::bucket)
```

## Troubleshooting

### Common Build Issues

1. CMake configuration fails:
   - Check CMake version (3.15+ required)
   - Ensure C++20 compiler is available
   - Verify all dependencies are installed

2. Tests fail:
   - Check compiler compatibility
   - Verify Google Test installation
   - Run tests with verbose output: `ctest -V`

### Getting Help

If you encounter issues:
1. Check existing GitHub issues
2. Review the documentation
3. Create a new issue with:
   - Description of the problem
   - Build environment details
   - Steps to reproduce
   - Relevant error messages 