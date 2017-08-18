cmake_minimum_required(VERSION 3.5)

project(yadi)

set (CMAKE_CXX_STANDARD 14)

set(SOURCES src/yadi/factory.hpp)

add_library(yadi INTERFACE)
target_include_directories(yadi INTERFACE src)

set(TEST_SOURCES test/yadi/factory_test.cpp)

add_executable(yadi_test ${TEST_SOURCES})
target_link_libraries(yadi_test yadi)

