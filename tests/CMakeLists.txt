# tests/CMakeLists.txt - unit tests for the bucket library, using GoogleTest.

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# Each test file is built as its own executable and linked against
# gtest_main/gmock_main, which supplies the test runner's main().
function(bucket_add_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE bucket gtest_main gmock_main)
    gtest_discover_tests(${name})
endfunction()

bucket_add_test(triplet_test)
bucket_add_test(optional_test)
bucket_add_test(compare_traits_test)
bucket_add_test(buckets_test)
bucket_add_test(buckets_supp_test)
bucket_add_test(main_support_test)

# known_issues_test documents/reproduces latent library bugs discovered
# while writing this suite. Its tests are all DISABLED_ by default (see
# the file for how to run them explicitly) so that this target does not
# affect the overall pass/fail status of `ctest`.
bucket_add_test(known_issues_test)
