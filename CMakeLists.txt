cmake_minimum_required(VERSION 3.20)
project(SC2AIOptimize VERSION 0.1.0 LANGUAGES CXX)

option(SC2OPT_BUILD_TESTS "Build SC2AIOptimize tests" ON)
option(SC2OPT_WARNINGS_AS_ERRORS "Treat project warnings as errors" OFF)

add_library(sc2opt_registry STATIC
    SC2Registry/SC2Registry75689.cpp
)
add_library(sc2opt::registry ALIAS sc2opt_registry)
target_include_directories(sc2opt_registry PUBLIC include)
target_compile_features(sc2opt_registry PUBLIC cxx_std_20)

add_library(sc2opt_tuner STATIC
    Tuner/NetBenefit.cpp
)
add_library(sc2opt::tuner ALIAS sc2opt_tuner)
target_include_directories(sc2opt_tuner PUBLIC include)
target_compile_features(sc2opt_tuner PUBLIC cxx_std_20)

add_library(sc2opt_kernel INTERFACE)
add_library(sc2opt::kernel ALIAS sc2opt_kernel)
target_include_directories(sc2opt_kernel INTERFACE include)
target_compile_features(sc2opt_kernel INTERFACE cxx_std_20)

add_library(sc2opt INTERFACE)
add_library(sc2opt::sc2opt ALIAS sc2opt)
target_link_libraries(sc2opt INTERFACE sc2opt_registry sc2opt_tuner sc2opt_kernel)

function(sc2opt_apply_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
        if(SC2OPT_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        if(SC2OPT_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

sc2opt_apply_warnings(sc2opt_registry)
sc2opt_apply_warnings(sc2opt_tuner)

if(SC2OPT_BUILD_TESTS)
    enable_testing()
    add_executable(sc2opt_core_tests tests/core_tests.cpp)
    target_link_libraries(sc2opt_core_tests PRIVATE sc2opt)
    target_compile_features(sc2opt_core_tests PRIVATE cxx_std_20)
    sc2opt_apply_warnings(sc2opt_core_tests)
    add_test(NAME sc2opt_core_tests COMMAND sc2opt_core_tests)
endif()
