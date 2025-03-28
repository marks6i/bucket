# Create test executable
add_executable(bucket_tests
    main.cpp
)

# Link against Google Test
target_link_libraries(bucket_tests PRIVATE
    GTest::gtest_main
    GTest::gmock_main
)

# Include directories
target_include_directories(bucket_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Register tests
gtest_discover_tests(bucket_tests) 