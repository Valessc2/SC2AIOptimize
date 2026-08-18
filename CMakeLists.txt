cmake_minimum_required(VERSION 3.20)
project(SC2AIOptimize VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(cmake/SC2OptHardening.cmake)

option(SC2OPT_BUILD_TESTS "Build SC2AIOptimize tests" ON)
option(SC2OPT_BUILD_BENCHMARKS "Build calibration/benchmark tools" OFF)
option(SC2OPT_BUILD_PYTHON "Build the optional generic Python adapter" OFF)
option(SC2OPT_WARNINGS_AS_ERRORS "Treat project warnings as errors" OFF)
option(SC2OPT_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(SC2OPT_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(SC2OPT_ENABLE_LTO "Enable Release IPO/LTO" OFF)
option(SC2OPT_ENABLE_AVX2 "Enable explicit AVX2 hardware contract" OFF)
option(SC2OPT_PGO_GENERATE "Build with PGO instrumentation" OFF)
option(SC2OPT_PGO_USE "Build using a trained PGO profile" OFF)
option(SC2OPT_ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)
set(SC2OPT_PGO_PROFILE_DIR "" CACHE PATH "PGO profile directory for GNU PGO")

if(SC2OPT_PGO_GENERATE AND SC2OPT_PGO_USE)
    message(FATAL_ERROR "SC2OPT_PGO_GENERATE and SC2OPT_PGO_USE are mutually exclusive")
endif()

if(SC2OPT_ENABLE_CLANG_TIDY)
    find_program(SC2OPT_CLANG_TIDY_EXE NAMES clang-tidy)
    if(NOT SC2OPT_CLANG_TIDY_EXE)
        message(FATAL_ERROR "SC2OPT_ENABLE_CLANG_TIDY requested but clang-tidy was not found")
    endif()
    set(CMAKE_CXX_CLANG_TIDY "${SC2OPT_CLANG_TIDY_EXE}")
endif()

function(sc2opt_apply_project_settings target)
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
    sc2opt_apply_hardening(${target})
endfunction()

function(sc2opt_public_includes target)
    target_include_directories(${target} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
endfunction()

add_library(sc2opt_registry STATIC
    SC2Registry/SC2Registry75689.cpp
)
add_library(sc2opt::registry ALIAS sc2opt_registry)
set_target_properties(sc2opt_registry PROPERTIES POSITION_INDEPENDENT_CODE ON EXPORT_NAME registry)
sc2opt_public_includes(sc2opt_registry)
target_compile_features(sc2opt_registry PUBLIC cxx_std_20)
sc2opt_apply_project_settings(sc2opt_registry)

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
set_target_properties(sc2opt_kernel PROPERTIES POSITION_INDEPENDENT_CODE ON EXPORT_NAME kernel)
sc2opt_public_includes(sc2opt_kernel)
target_compile_features(sc2opt_kernel PUBLIC cxx_std_20)
sc2opt_apply_project_settings(sc2opt_kernel)

add_library(sc2opt_tuner STATIC
    Tuner/NetBenefit.cpp
    Tuner/TunableRegistry.cpp
    Tuner/Champion.cpp
    Tuner/Context.cpp
    Tuner/Persistence.cpp
    Tuner/Telemetry.cpp
    Tuner/BudgetGovernor.cpp
    Tuner/Calibration.cpp
)
add_library(sc2opt::tuner ALIAS sc2opt_tuner)
set_target_properties(sc2opt_tuner PROPERTIES POSITION_INDEPENDENT_CODE ON EXPORT_NAME tuner)
sc2opt_public_includes(sc2opt_tuner)
target_compile_features(sc2opt_tuner PUBLIC cxx_std_20)
sc2opt_apply_project_settings(sc2opt_tuner)

add_library(sc2opt_adapters STATIC
    Adapters/Cpp/DenseTypeMap.cpp
)
add_library(sc2opt::adapters ALIAS sc2opt_adapters)
set_target_properties(sc2opt_adapters PROPERTIES POSITION_INDEPENDENT_CODE ON EXPORT_NAME adapters)
sc2opt_public_includes(sc2opt_adapters)
target_link_libraries(sc2opt_adapters PUBLIC sc2opt_registry)
target_compile_features(sc2opt_adapters PUBLIC cxx_std_20)
sc2opt_apply_project_settings(sc2opt_adapters)

add_library(sc2opt_proof STATIC
    Proof/Profiles.cpp
    Proof/Manifest.cpp
    Proof/Regression.cpp
)
add_library(sc2opt::proof ALIAS sc2opt_proof)
set_target_properties(sc2opt_proof PROPERTIES POSITION_INDEPENDENT_CODE ON EXPORT_NAME proof)
sc2opt_public_includes(sc2opt_proof)
target_link_libraries(sc2opt_proof PUBLIC sc2opt_tuner)
target_compile_features(sc2opt_proof PUBLIC cxx_std_20)
sc2opt_apply_project_settings(sc2opt_proof)

add_library(sc2opt INTERFACE)
add_library(sc2opt::sc2opt ALIAS sc2opt)
set_target_properties(sc2opt PROPERTIES EXPORT_NAME sc2opt)
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
    sc2opt_apply_project_settings(_sc2opt)
endif()

if(SC2OPT_BUILD_TESTS)
    enable_testing()

    function(sc2opt_add_test target source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE sc2opt)
        target_compile_features(${target} PRIVATE cxx_std_20)
        sc2opt_apply_project_settings(${target})
        add_test(NAME ${target} COMMAND ${target})
    endfunction()

    sc2opt_add_test(sc2opt_core_tests tests/core_tests.cpp)
    sc2opt_add_test(sc2opt_adapter_batch_tests tests/adapter_batch_tests.cpp)
    sc2opt_add_test(sc2opt_hot_kernel_tests tests/hot_kernel_tests.cpp)
    sc2opt_add_test(sc2opt_compute_tests tests/compute_tests.cpp)
    sc2opt_add_test(sc2opt_tuner_tests tests/tuner_tests.cpp)
    sc2opt_add_test(sc2opt_runtime_optimization_tests tests/runtime_optimization_tests.cpp)
    sc2opt_add_test(sc2opt_deep_contract_tests tests/deep_contract_tests.cpp)
    sc2opt_add_test(sc2opt_synthetic_cpp_consumer examples/synthetic_cpp_consumer.cpp)

    add_executable(sc2opt_manifest_tests tests/manifest_tests.cpp)
    target_link_libraries(sc2opt_manifest_tests PRIVATE sc2opt_proof)
    target_compile_features(sc2opt_manifest_tests PRIVATE cxx_std_20)
    sc2opt_apply_project_settings(sc2opt_manifest_tests)
    add_test(NAME sc2opt_manifest_tests COMMAND sc2opt_manifest_tests)

    add_executable(sc2opt_performance_regression_tests tests/performance_regression_tests.cpp)
    target_link_libraries(sc2opt_performance_regression_tests PRIVATE sc2opt_proof)
    target_compile_features(sc2opt_performance_regression_tests PRIVATE cxx_std_20)
    sc2opt_apply_project_settings(sc2opt_performance_regression_tests)
    add_test(NAME sc2opt_performance_regression_tests COMMAND sc2opt_performance_regression_tests)

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
    sc2opt_apply_project_settings(sc2opt_spatial_query_bench)
endif()

install(TARGETS
    sc2opt_registry
    sc2opt_kernel
    sc2opt_tuner
    sc2opt_adapters
    sc2opt_proof
    sc2opt
    EXPORT SC2AIOptimizeTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_package_config_file(
    cmake/SC2AIOptimizeConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SC2AIOptimizeConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SC2AIOptimize
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SC2AIOptimizeConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(EXPORT SC2AIOptimizeTargets
    FILE SC2AIOptimizeTargets.cmake
    NAMESPACE sc2opt::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SC2AIOptimize
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SC2AIOptimizeConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SC2AIOptimizeConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SC2AIOptimize
)
