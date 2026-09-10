cmake_minimum_required(VERSION 3.15)

project(bucket VERSION 1.0.0 LANGUAGES CXX)

# The library (include/*.h) is written to compile as C++14; do not raise
# this without re-verifying source compatibility (see include/triplet.h).
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(MSVC)
    add_compile_options(/W4)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Optional code-coverage instrumentation (GCC/Clang only): configure with
# -DBUCKET_ENABLE_COVERAGE=ON to build with --coverage instrumentation.
option(BUCKET_ENABLE_COVERAGE "Build with gcov/lcov coverage instrumentation" OFF)
if(BUCKET_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(WARNING "BUCKET_ENABLE_COVERAGE is only supported with GCC/Clang")
    endif()
endif()

enable_testing()

# This is a header-only library; the headers live directly under include/.
add_library(bucket INTERFACE)
target_include_directories(bucket INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# The demonstration application (bucket/main.cpp) that exercises the library.
add_executable(bucket_app bucket/main.cpp)
target_link_libraries(bucket_app PRIVATE bucket)

add_subdirectory(tests)

include(GNUInstallDirs)
install(TARGETS bucket
    EXPORT bucket-targets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.h"
)
install(EXPORT bucket-targets
    FILE bucket-targets.cmake
    NAMESPACE bucket::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bucket
)
