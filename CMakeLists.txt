project(yadi)

set (CMAKE_CXX_STANDARD 17)

set(SOURCES src/yadi/factory.hpp)

add_library(yadi INTERFACE)
target_include_directories(yadi INTERFACE src)

set(TEST_SOURCES test/yadi/factory_test.hpp test/yadi/factory_test.hpp.cpp)

add_executable(yadi_test ${TEST_SOURCES})
target_link_libraries(yadi_test yadi)

