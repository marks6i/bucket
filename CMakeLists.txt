cmake_minimum_required(VERSION 3.15)

project(bucket VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler options and runtime library
if(MSVC)
    add_compile_options(/W4)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Add include directories
include_directories(include)

# Enable testing
enable_testing()

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch and configure Google Test
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0  # Use a stable release
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Create library target
add_library(bucket INTERFACE)
target_include_directories(bucket INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add test executables
add_executable(bucket_list_test tests/bucket_list_test.cpp)
target_link_libraries(bucket_list_test PRIVATE bucket gtest_main gmock_main)
add_test(NAME bucket_list_test COMMAND bucket_list_test)

add_executable(bucket_map_test tests/bucket_map_test.cpp)
target_link_libraries(bucket_map_test PRIVATE bucket gtest_main gmock_main)
add_test(NAME bucket_map_test COMMAND bucket_map_test)

# Install targets
include(GNUInstallDirs)
install(TARGETS bucket
    EXPORT bucket-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/bucket
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)
install(EXPORT bucket-targets
    FILE bucket-targets.cmake
    NAMESPACE bucket::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bucket
) 