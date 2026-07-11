cmake_minimum_required(VERSION 3.16)
project(bucket CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Main application
add_executable(bucket_app
    bucket/main.cpp
)
target_include_directories(bucket_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/bucket
)

# Test executable
add_executable(bucket_test
    test/test1.cpp
)
target_include_directories(bucket_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/test
)
target_link_libraries(bucket_test PRIVATE
    GTest::gtest_main
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(bucket_test)
