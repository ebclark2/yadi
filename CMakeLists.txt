cmake_minimum_required(VERSION 3.2)

project(yadi)

set (CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_definitions(-W -Wall -Wextra -pedantic)

set(YADI_SOURCES src/yadi/yadi.hpp src/yadi/yadi.cpp src/yadi/details/demangle.cpp src/yadi/details/demangle.hpp)

add_library(yadi ${YADI_SOURCES})
target_include_directories(yadi PUBLIC src)
target_link_libraries(yadi CONAN_PKG::yaml-cpp CONAN_PKG::Boost)

set(YADI_UTIL_SOURCES util/yadi/yadi_util.cpp util/yadi/yadi_util.hpp test/yadi/example.cpp)

add_executable(yadi_util ${YADI_UTIL_SOURCES})
target_link_libraries(yadi_util yadi)

set(TEST_SOURCES test/yadi/shared_ptr_test.cpp test/yadi/unique_ptr_test.cpp test/yadi/raw_ptr_test.cpp test/yadi/main.cpp test/yadi/test.hpp test/yadi/yaml_test.cpp test/yadi/alias_test.cpp test/yadi/by_value_test.cpp test/yadi/example.cpp test/yadi/yaml_bindings_test.cpp)

enable_testing()

add_executable(yadi_test ${TEST_SOURCES})
target_link_libraries(yadi_test yadi)

add_test(yadi_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/yadi_test)
