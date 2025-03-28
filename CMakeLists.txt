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
FetchContent_MakeAvailable(googletest)

# Add library target
add_library(bucket INTERFACE)
target_include_directories(bucket INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add modern test executables (C++20)
add_executable(bucket_list_test tests/bucket_list_test.cpp)
target_link_libraries(bucket_list_test PRIVATE bucket GTest::gtest_main GTest::gmock_main)
target_include_directories(bucket_list_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME bucket_list_test COMMAND bucket_list_test)

add_executable(bucket_map_test tests/bucket_map_test.cpp)
target_link_libraries(bucket_map_test PRIVATE bucket GTest::gtest_main GTest::gmock_main)
target_include_directories(bucket_map_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME bucket_map_test COMMAND bucket_map_test)

add_executable(bucket_compare_traits_test tests/bucket_compare_traits_test.cpp)
target_link_libraries(bucket_compare_traits_test PRIVATE bucket GTest::gtest_main GTest::gmock_main)
target_include_directories(bucket_compare_traits_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME bucket_compare_traits_test COMMAND bucket_compare_traits_test)

add_executable(bucket_value_traits_test tests/bucket_value_traits_test.cpp)
target_link_libraries(bucket_value_traits_test PRIVATE bucket GTest::gtest_main GTest::gmock_main)
target_include_directories(bucket_value_traits_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME bucket_value_traits_test COMMAND bucket_value_traits_test)

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