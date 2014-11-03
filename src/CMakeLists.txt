cmake_minimum_required (VERSION 2.8.7)

# option(coverage "Build with code-coverage support." FALSE)

project(rate-calls)
# set(GTEST_ROOT "${CMAKE_SOURCE_DIR}../gtest-1.7.0")

add_executable(rate-calls matcher.h main.cpp csvparser.cpp csvparser.h)
install(TARGETS rate-calls DESTINATION bin)

find_library(POCO_FOUNDATION_LIBRARY NAMES PocoFoundation)
if (POCO_FOUNDATION_LIBRARY)
    add_executable(rate-http http.cpp matcher.h csvparser.cpp csvparser.h)
    target_link_libraries(rate-http PocoFoundation PocoNet PocoUtil)
    install(TARGETS rate-http DESTINATION bin)
endif()

enable_testing()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0")
include_directories("${CMAKE_SOURCE_DIR}/../gtest-1.7.0/include")
add_executable(tests tests.cpp matcher.h)
target_link_libraries(tests gtest_main)

