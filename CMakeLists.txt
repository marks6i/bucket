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

# Platform-specific compiler settings
if(MSVC)
    add_compile_options(/W4)
    # Use the same runtime library for all targets
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
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

# Include Google Test CMake module
include(GoogleTest)

# Configure test directory
set(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Add helper function for creating tests
function(add_gtest_executable TEST_NAME)
    add_executable(${TEST_NAME} ${TEST_DIR}/${TEST_NAME}.cpp)
    target_link_libraries(${TEST_NAME} PRIVATE bucket GTest::gtest_main GTest::gmock_main)
    target_include_directories(${TEST_NAME} PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${googletest_SOURCE_DIR}/googletest/include
        ${googletest_SOURCE_DIR}/googlemock/include
        ${googletest_SOURCE_DIR}/googletest
        ${googletest_SOURCE_DIR}/googlemock
    )
    
    # Add test with platform-specific settings
    if(MSVC)
        # Windows-specific test configuration
        gtest_discover_tests(${TEST_NAME}
            PROPERTIES
                VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                VS_DEBUGGER_ENVIRONMENT "PATH=${CMAKE_CURRENT_BINARY_DIR}"
        )
        set_target_properties(${TEST_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            VS_DEBUGGER_ENVIRONMENT "PATH=${CMAKE_CURRENT_BINARY_DIR}"
        )
    else()
        # Linux/Unix test configuration
        gtest_discover_tests(${TEST_NAME}
            PROPERTIES
                ENVIRONMENT "PATH=${CMAKE_CURRENT_BINARY_DIR}"
        )
    endif()
endfunction()

# Add library target
add_library(bucket INTERFACE)
target_include_directories(bucket INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Add test executables using the helper function
add_gtest_executable(bucket_list_test)
add_gtest_executable(bucket_map_test)
add_gtest_executable(bucket_compare_traits_test)
add_gtest_executable(bucket_value_traits_test)
add_gtest_executable(bucket_traits_test)

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