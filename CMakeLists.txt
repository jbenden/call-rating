cmake_minimum_required (VERSION 2.8.7)

option(coverage "Build with test coverage." FALSE)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake-modules")

if (coverage)
    include(CodeCoverage)
    setup_target_for_coverage(coverage "${CMAKE_SOURCE_DIR}/src/tests" coverage)
endif()

add_subdirectory(gtest-1.7.0)
add_subdirectory(src)
