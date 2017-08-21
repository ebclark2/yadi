cmake_minimum_required(VERSION 3.2)

project(yadi)

set (CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


add_definitions(-W -Wall -Wextra -pedantic)

set(SOURCES src/yadi/yadi.hpp  src/yadi/std_types.hpp)

add_library(yadi INTERFACE)
target_include_directories(yadi INTERFACE src)
target_link_libraries(yadi INTERFACE CONAN_PKG::yaml-cpp)

set(TEST_SOURCES test/yadi/shared_ptr_test.cpp test/yadi/unique_ptr_test.cpp test/yadi/raw_ptr_test.cpp test/yadi/main.cpp test/yadi/test.hpp test/yadi/yaml_test.cpp test/yadi/alias_test.cpp test/yadi/by_value_test.cpp test/yadi/example.cpp)

enable_testing()

add_executable(yadi_test ${TEST_SOURCES})
target_link_libraries(yadi_test yadi)

add_test(yadi_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/yadi_test)
