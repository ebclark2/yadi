cmake_minimum_required(VERSION 3.2)

project(yadi)

set (CMAKE_CXX_STANDARD 14)

add_definitions(-W -Wall -Wextra -pedantic)

set(SOURCES src/yadi/factory.hpp)

add_library(yadi INTERFACE)
target_include_directories(yadi INTERFACE src)

set(TEST_SOURCES test/yadi/factory_test.cpp)

enable_testing()

add_executable(yadi_test ${TEST_SOURCES})
target_link_libraries(yadi_test yadi)

add_test(yadi_test yadi_test)
