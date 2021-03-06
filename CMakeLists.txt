cmake_minimum_required(VERSION 3.2)

project(yadi)

set (CMAKE_CXX_STANDARD 17)

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
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_definitions(-W -Wall -Wextra -pedantic)

set(YADI_SOURCES src/yadi/yadi.hpp src/yadi/yadi.cpp src/yadi/details/demangle.cpp src/yadi/details/demangle.hpp src/yadi/details/help.cpp src/yadi/details/help.hpp src/yadi/details/initializers.hpp src/yadi/details/factory.hpp src/yadi/details/create_specializations.hpp src/yadi/details/create_utils.hpp src/yadi/details/create_utils.cpp src/yadi/details/type_utils.hpp src/yadi/details/registration.hpp src/yadi/details/registration.cpp src/yadi/details/factory.cpp src/yadi/details/create_specializations.cpp src/yadi/details/initializers.cpp src/yadi/details/type_utils.cpp test/yadi/registration_mod_test.cpp)

add_library(yadi ${YADI_SOURCES})
target_include_directories(yadi PUBLIC src)
target_link_libraries(yadi CONAN_PKG::yaml-cpp)

set(YADI_INSPECTOR_LIB_SOURCES inspector/yadi/inspector.cpp inspector/yadi/inspector.hpp)

add_library(yadi_inspector_lib ${YADI_INSPECTOR_LIB_SOURCES})
target_include_directories(yadi_inspector_lib PUBLIC inspector)
target_link_libraries(yadi_inspector_lib yadi)

set(YADI_INSPECTOR_SOURCES inspector/main.cpp)

add_executable(yadi_inspector ${YADI_INSPECTOR_SOURCES} )
target_link_libraries(yadi_inspector yadi_inspector_lib)


set(TEST_SOURCES test/yadi/shared_ptr_test.cpp test/yadi/unique_ptr_test.cpp test/yadi/raw_ptr_test.cpp test/yadi/main.cpp test/yadi/test.hpp test/yadi/yaml_test.cpp test/yadi/alias_test.cpp test/yadi/by_value_test.cpp test/yadi/example.cpp test/yadi/yaml_bindings_test.cpp test/yadi/parse_test.cpp test/yadi/inspector_test.cpp test/yadi/adapter_test.cpp test/yadi/passthrough_test.cpp)

enable_testing()

add_executable(yadi_test ${TEST_SOURCES})
target_link_libraries(yadi_test yadi yadi_inspector_lib)

add_test(yadi_test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/yadi_test)
