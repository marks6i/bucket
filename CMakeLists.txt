cmake_minimum_required(VERSION 3.15)

project(bucket VERSION 1.0.0 LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Platform-specific compiler settings
if(MSVC)
    add_compile_options(/W4 /std:c++20)
    # Use the same runtime library for all targets
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -std=c++20)
endif()

# Add clang-tidy target
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    # Copy run-clang-tidy.sh to build directory
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/run-clang-tidy.sh
        ${CMAKE_CURRENT_BINARY_DIR}/run-clang-tidy.sh
        COPYONLY
    )
    
    # Make the script executable
    file(CHMOD ${CMAKE_CURRENT_BINARY_DIR}/run-clang-tidy.sh
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
    
    # Add custom target
    add_custom_target(
        ctidy
        COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR} && ${CMAKE_CURRENT_BINARY_DIR}/run-clang-tidy.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running clang-tidy..."
        VERBATIM
    )
else()
    message(STATUS "clang-tidy not found. The 'ctidy' target will not be available.")
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
        set_target_properties(${TEST_NAME} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            VS_DEBUGGER_ENVIRONMENT "PATH=${CMAKE_CURRENT_BINARY_DIR}"
        )
        
        # Enable Google Test discovery for VS
        gtest_discover_tests(${TEST_NAME}
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            PROPERTIES
                LABELS "gtest"
                ENVIRONMENT "PATH=${CMAKE_CURRENT_BINARY_DIR}"
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

# Add test executables
add_gtest_executable(bucket_list_test)
add_gtest_executable(bucket_map_test)
add_gtest_executable(bucket_compare_traits_test)
add_gtest_executable(bucket_value_traits_test)
add_gtest_executable(bucket_traits_test)

# Configure package version
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/bucket-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Configure package
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/bucket-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/bucket-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bucket
)

# Install targets
include(GNUInstallDirs)
install(TARGETS bucket
    EXPORT bucket-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install header files
install(DIRECTORY include/bucket/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bucket
    FILES_MATCHING 
    PATTERN "*.h"
    PATTERN "detail" EXCLUDE
)
install(DIRECTORY include/bucket/detail/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/bucket/detail
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT bucket-targets
    FILE bucket-targets.cmake
    NAMESPACE bucket::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bucket
)

# Install package configuration files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/bucket-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/bucket-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/bucket
)

# Configure CPack for creating source package only (header-only library)
include(CPack)
set(CPACK_PACKAGE_NAME "bucket")
set(CPACK_PACKAGE_VENDOR "masutils")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern C++20 header-only bucket container library")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})

# Source package settings only - no binary packages needed for header-only library
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES
    "/\\\\.git/"
    "/\\\\.github/"
    "/build/"
    "/\\\\.vscode/"
    "/\\\\.vs/"
    "\\\\.gitignore$"
    ".*~$"
)

# Install additional directories for source distribution
install(DIRECTORY docs/
    DESTINATION share/doc/bucket
    FILES_MATCHING PATTERN "*.md") 