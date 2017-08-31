cmake_minimum_required(VERSION 3.2)

project(yadi)

set (CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_check_compiler()
#conan_output_dirs_setup()
conan_set_find_library_paths()
message(STATUS "Conan: Using cmake targets configuration")
conan_define_targets()
conan_set_rpath()
conan_set_vs_runtime()
conan_set_libcxx()
conan_set_find_paths()

set(BUILD_SHARED_LIBS ON)

add_definitions(-W -Wall -Wextra -pedantic)

set(YADI_SOURCES src/yadi/yadi.hpp src/yadi/yadi.cpp src/yadi/details/demangle.cpp src/yadi/details/demangle.hpp src/yadi/details/help.cpp src/yadi/details/help.hpp src/yadi/details/initializers.hpp src/yadi/details/factory.hpp src/yadi/details/factory_specializations.hpp src/yadi/details/create_utils.hpp src/yadi/details/create_utils.cpp src/yadi/details/type_utils.hpp src/yadi/details/registration.hpp)

add_library(yadi ${YADI_SOURCES})
target_include_directories(yadi PUBLIC src)
target_link_libraries(yadi CONAN_PKG::yaml-cpp CONAN_PKG::Boost)

set(YADI_INSPECTOR_SOURCES inspector/yadi/inspector.cpp test/yadi/example.cpp)

add_executable(yadi_inspector ${YADI_INSPECTOR_SOURCES})
target_link_libraries(yadi_inspector yadi)

set(TEST_SOURCES test/yadi/shared_ptr_test.cpp test/yadi/unique_ptr_test.cpp test/yadi/raw_ptr_test.cpp test/yadi/main.cpp test/yadi/test.hpp test/yadi/yaml_test.cpp test/yadi/alias_test.cpp test/yadi/by_value_test.cpp test/yadi/example.cpp test/yadi/yaml_bindings_test.cpp test/yadi/parse_test.cpp)

enable_testing()

add_executable(yadi_test ${TEST_SOURCES})
target_link_libraries(yadi_test yadi)

add_test(yadi_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/yadi_test)
