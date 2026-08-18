cmake_minimum_required(VERSION 3.20)

project(SC2AIOptimize VERSION 0.1.0 LANGUAGES CXX)

option(SC2OPT_BUILD_TESTS "Build SC2AIOptimize component tests" ON)
option(SC2OPT_WARNINGS_AS_ERRORS "Treat SC2AIOptimize warnings as errors" ON)

add_library(sc2aiopt STATIC
    src/registry/registry.cpp
    src/kernel/spatial_index.cpp
    src/tuner/net_benefit.cpp
)

add_library(sc2opt::sc2aiopt ALIAS sc2aiopt)

target_compile_features(sc2aiopt PUBLIC cxx_std_20)
target_include_directories(sc2aiopt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(MSVC)
    target_compile_options(sc2aiopt PRIVATE /W4 /permissive-)
    if(SC2OPT_WARNINGS_AS_ERRORS)
        target_compile_options(sc2aiopt PRIVATE /WX)
    endif()
else()
    target_compile_options(sc2aiopt PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    if(SC2OPT_WARNINGS_AS_ERRORS)
        target_compile_options(sc2aiopt PRIVATE -Werror)
    endif()
endif()

if(SC2OPT_BUILD_TESTS)
    enable_testing()
    add_executable(sc2aiopt_tests tests/test_core.cpp)
    target_link_libraries(sc2aiopt_tests PRIVATE sc2opt::sc2aiopt)
    target_compile_features(sc2aiopt_tests PRIVATE cxx_std_20)
    add_test(NAME sc2aiopt_core COMMAND sc2aiopt_tests)
endif()
