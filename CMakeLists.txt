cmake_minimum_required(VERSION 3.10)

# Project name
project(ecsEngine)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the compiler
set(CMAKE_CXX_COMPILER clang++)

# Add source files
set(SOURCES main.cpp)

# Set compiler flags
set(CMAKE_CXX_FLAGS_STANDARD "-Wall -Wextra -Wno-sign-compare -Wno-trigraphs -Wno-unused-parameter -Wno-static-inline-explicit-instantiation")
set(CMAKE_CXX_FLAGS_UNSAFE_METHODS "-Decs_allow_unsafe")

set(CMAKE_CXX_FLAGS_DIST "-Decs_distribution")
set(CMAKE_CXX_FLAGS_UNSAFE_CASTS "-Dunsafe_casts")

set(CMAKE_CXX_FLAGS_WIN_COMPAT "-fdeclspec") 
set(CMAKE_CXX_FLAGS_OPTIMIZATIONS "-O3")
set(CMAKE_CXX_FLAGS_SANITIZE "-fno-integrated-as -fno-omit-frame-pointer -fsanitize-address-use-after-scope -fsanitize=address -fsanitize=leak")
set(CMAKE_CXX_FLAGS_WARN "-Decs_show_warnings")
set(CMAKE_CXX_FLAGS_DEBUG "-g -w -O0 -Decs_allow_debug")
set(CMAKE_CXX_FLAGS_SAVE_TEMPS "-save-temps")
set(CMAKE_CXX_FLAGS_NO_AUTO_PRUNE "-Decs_disable_auto_prune")
set(CMAKE_CXX_FLAGS_TEST "${CMAKE_CXX_FLAGS_UNSAFE_METHODS} ${CMAKE_CXX_FLAGS_DEBUG} -Decs_test")
set(CMAKE_CXX_FLAGS_BENCHMARK "${CMAKE_CXX_FLAGS_TEST} -Decs_bench_test")

# Uncomment one of the following to set the desired build type
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_SAVE_TEMPS}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_STANDARD}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_UNSAFE_METHODS}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_WARN}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_SANITIZE}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_TEST}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_BENCHMARK}")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_NO_AUTO_PRUNE}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_OPTIMIZATIONS}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_UNSAFE_CASTS}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_WIN_COMPAT}")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DIST}")

# Link libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Create the executable
add_executable(main ${SOURCES})

# Link the libraries
target_link_libraries(main SDL2::SDL2 SDL2_image::SDL2_image)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
