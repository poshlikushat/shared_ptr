cmake_minimum_required(VERSION 3.20)
project(shared_ptr_test CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(shared_ptr_lib INTERFACE)
target_include_directories(shared_ptr_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

if (BUILD_TESTING)
    add_executable(shared_ptr_test test.cpp)
    target_link_libraries(shared_ptr_test PRIVATE shared_ptr_lib GTest::gtest GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(shared_ptr_test)
endif ()
