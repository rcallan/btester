cmake_minimum_required(VERSION 3.16)

add_subdirectory(../lib lib)

include_directories(${SHARED_LIB_INCLUDE_DIR})

set(SRCS Comps_test.cpp)
add_executable(Comps_test ${SRCS})
target_link_libraries(
    Comps_test
    lib
    GTest::gtest_main
    GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(Comps_test)