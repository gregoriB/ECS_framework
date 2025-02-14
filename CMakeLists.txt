cmake_minimum_required(VERSION 3.16...3.31 FATAL_ERROR)

project(ecslib LANGUAGES CXX)

add_library(ecs INTERFACE)
add_library(ECS::ecs ALIAS ecs)

target_include_directories(ecs INTERFACE
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_compile_features(ecs INTERFACE cxx_std_20)

target_compile_options(ecs INTERFACE
    -g
    -w
    -Decs_show_warnings
    -Decs_allow_debug
    -Decs_allow_unsafe
)

set_target_properties(ecs PROPERTIES EXPORT_NAME ECS)

enable_testing()
add_subdirectory(test)
