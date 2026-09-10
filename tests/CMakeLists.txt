include(GoogleTest)

# Each test file is built as its own executable and linked against
# gtest_main/gmock_main, which supplies the test runner's main().
add_executable(bucket_list_test bucket_list_test.cpp)
target_link_libraries(bucket_list_test PRIVATE bucket gtest_main gmock_main)
gtest_discover_tests(bucket_list_test)

add_executable(bucket_map_test bucket_map_test.cpp)
target_link_libraries(bucket_map_test PRIVATE bucket gtest_main gmock_main)
gtest_discover_tests(bucket_map_test) 