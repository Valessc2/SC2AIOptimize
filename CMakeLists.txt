cmake_minimum_required(VERSION 3.20)
project(SC2AIOptimize VERSION 0.1.0 LANGUAGES CXX)

option(SC2OPT_BUILD_TESTS "Build SC2AIOptimize tests" ON)
option(SC2OPT_BUILD_BENCHMARKS "Build calibration/benchmark tools" OFF)
option(SC2OPT_BUILD_PYTHON "Build the optional generic Python adapter" OFF)
option(SC2OPT_WARNINGS_AS_ERRORS "Treat project warnings as errors" OFF)

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

add_library(sc2opt_registry STATIC
    SC2Registry/SC2Registry75689.cpp
)
add_library(sc2opt::registry ALIAS sc2opt_registry)
target_include_directories(sc2opt_registry PUBLIC include)
target_compile_features(sc2opt_registry PUBLIC cxx_std_20)
set_target_properties(sc2opt_registry PROPERTIES POSITION_INDEPENDENT_CODE ON)
sc2opt_apply_warnings(sc2opt_registry)

add_library(sc2opt_kernel STATIC
    Kernel/Hot/BatchGeometry.cpp
    Kernel/Hot/Filtering.cpp
    Kernel/Hot/SpatialGrid.cpp
    Kernel/Compute/CombatEvaluation.cpp
    Kernel/Compute/Influence.cpp
    Kernel/Compute/Pathing.cpp
    Kernel/Compute/Placement.cpp
    Kernel/Compute/TargetSelection.cpp
)
add_library(sc2opt::kernel ALIAS sc2opt_kernel)
target_include_directories(sc2opt_kernel PUBLIC include)
target_compile_features(sc2opt_kernel PUBLIC cxx_std_20)
set_target_properties(sc2opt_kernel PROPERTIES POSITION_INDEPENDENT_CODE ON)
sc2opt_apply_warnings(sc2opt_kernel)

add_library(sc2opt_tuner STATIC
    Tuner/NetBenefit.cpp
    Tuner/TunableRegistry.cpp
    Tuner/Champion.cpp
    Tuner/Context.cpp
    Tuner/Persistence.cpp
)
add_library(sc2opt::tuner ALIAS sc2opt_tuner)
target_include_directories(sc2opt_tuner PUBLIC include)
target_compile_features(sc2opt_tuner PUBLIC cxx_std_20)
set_target_properties(sc2opt_tuner PROPERTIES POSITION_INDEPENDENT_CODE ON)
sc2opt_apply_warnings(sc2opt_tuner)

add_library(sc2opt_adapters STATIC
    Adapters/Cpp/DenseTypeMap.cpp
)
add_library(sc2opt::adapters ALIAS sc2opt_adapters)
target_include_directories(sc2opt_adapters PUBLIC include)
target_link_libraries(sc2opt_adapters PUBLIC sc2opt_registry)
target_compile_features(sc2opt_adapters PUBLIC cxx_std_20)
set_target_properties(sc2opt_adapters PROPERTIES POSITION_INDEPENDENT_CODE ON)
sc2opt_apply_warnings(sc2opt_adapters)

add_library(sc2opt INTERFACE)
add_library(sc2opt::sc2opt ALIAS sc2opt)
target_link_libraries(sc2opt INTERFACE
    sc2opt_registry
    sc2opt_kernel
    sc2opt_tuner
    sc2opt_adapters
)

if(SC2OPT_BUILD_PYTHON)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    set(PYBIND11_FINDPYTHON ON)
    find_package(pybind11 CONFIG REQUIRED)

    pybind11_add_module(_sc2opt Adapters/Python/module.cpp)
    target_link_libraries(_sc2opt PRIVATE sc2opt)
    target_compile_features(_sc2opt PRIVATE cxx_std_20)
    sc2opt_apply_warnings(_sc2opt)
endif()

if(SC2OPT_BUILD_TESTS)
    enable_testing()

    add_executable(sc2opt_core_tests tests/core_tests.cpp)
    target_link_libraries(sc2opt_core_tests PRIVATE sc2opt)
    target_compile_features(sc2opt_core_tests PRIVATE cxx_std_20)
    sc2opt_apply_warnings(sc2opt_core_tests)
    add_test(NAME sc2opt_core_tests COMMAND sc2opt_core_tests)

    add_executable(sc2opt_adapter_batch_tests tests/adapter_batch_tests.cpp)
    target_link_libraries(sc2opt_adapter_batch_tests PRIVATE sc2opt)
    target_compile_features(sc2opt_adapter_batch_tests PRIVATE cxx_std_20)
    sc2opt_apply_warnings(sc2opt_adapter_batch_tests)
    add_test(NAME sc2opt_adapter_batch_tests COMMAND sc2opt_adapter_batch_tests)

    add_executable(sc2opt_hot_kernel_tests tests/hot_kernel_tests.cpp)
    target_link_libraries(sc2opt_hot_kernel_tests PRIVATE sc2opt)
    target_compile_features(sc2opt_hot_kernel_tests PRIVATE cxx_std_20)
    sc2opt_apply_warnings(sc2opt_hot_kernel_tests)
    add_test(NAME sc2opt_hot_kernel_tests COMMAND sc2opt_hot_kernel_tests)

    add_executable(sc2opt_compute_tests tests/compute_tests.cpp)
    target_link_libraries(sc2opt_compute_tests PRIVATE sc2opt)
    target_compile_features(sc2opt_compute_tests PRIVATE cxx_std_20)
    sc2opt_apply_warnings(sc2opt_compute_tests)
    add_test(NAME sc2opt_compute_tests COMMAND sc2opt_compute_tests)

    add_executable(sc2opt_tuner_tests tests/tuner_tests.cpp)
    target_link_libraries(sc2opt_tuner_tests PRIVATE sc2opt)
    target_compile_features(sc2opt_tuner_tests PRIVATE cxx_std_20)
    sc2opt_apply_warnings(sc2opt_tuner_tests)
    add_test(NAME sc2opt_tuner_tests COMMAND sc2opt_tuner_tests)

    add_executable(sc2opt_synthetic_cpp_consumer examples/synthetic_cpp_consumer.cpp)
    target_link_libraries(sc2opt_synthetic_cpp_consumer PRIVATE sc2opt)
    target_compile_features(sc2opt_synthetic_cpp_consumer PRIVATE cxx_std_20)
    sc2opt_apply_warnings(sc2opt_synthetic_cpp_consumer)
    add_test(NAME sc2opt_synthetic_cpp_consumer COMMAND sc2opt_synthetic_cpp_consumer)

    if(SC2OPT_BUILD_PYTHON)
        add_test(
            NAME sc2opt_python_adapter
            COMMAND ${CMAKE_COMMAND} -E env
                "PYTHONPATH=$<TARGET_FILE_DIR:_sc2opt>"
                ${Python_EXECUTABLE}
                ${CMAKE_CURRENT_SOURCE_DIR}/Adapters/Python/tests/test_adapter.py
        )
    endif()
endif()

if(SC2OPT_BUILD_BENCHMARKS)
    add_executable(sc2opt_spatial_query_bench Bench/Micro/spatial_query_bench.cpp)
    target_link_libraries(sc2opt_spatial_query_bench PRIVATE sc2opt)
    target_compile_features(sc2opt_spatial_query_bench PRIVATE cxx_std_20)
    sc2opt_apply_warnings(sc2opt_spatial_query_bench)
endif()
