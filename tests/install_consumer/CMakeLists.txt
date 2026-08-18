cmake_minimum_required(VERSION 3.20)
project(SC2AIOptimizeInstalledConsumer LANGUAGES CXX)

find_package(SC2AIOptimize CONFIG REQUIRED)

add_executable(sc2opt_installed_consumer main.cpp)
target_link_libraries(sc2opt_installed_consumer PRIVATE sc2opt::sc2opt)
target_compile_features(sc2opt_installed_consumer PRIVATE cxx_std_20)

enable_testing()
add_test(NAME sc2opt_installed_consumer COMMAND sc2opt_installed_consumer)
