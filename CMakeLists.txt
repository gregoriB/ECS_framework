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

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

# Set compiler flags
set(ECS_FLAGS_STANDARD "-Wall" "-Wextra" "-Wno-sign-compare" "-Wno-trigraphs"  "-Wno-unused-parameter" "-Wno-static-inline-explicit-instantiation")
set(ECS_FLAGS_UNSAFE_METHODS "-Decs_allow_unsafe")

set(ECS_FLAGS_DIST "-Decs_distribution")
set(ECS_FLAGS_UNSAFE_CASTS "-Decs_unsafe_casts")

set(ECS_FLAGS_WIN_COMPAT "-fdeclspec") 
set(ECS_FLAGS_OPTIMIZATIONS "-O3")
set(ECS_FLAGS_SANITIZE "-fno-integrated-as" "-fno-omit-frame-pointer" "-fsanitize-address-use-after-scope" "-fsanitize=address" "-fsanitize=leak")
set(ECS_FLAGS_WARN "-Decs_show_warnings")
set(ECS_FLAGS_DEBUG "-g" "-w" "-O0" "-Decs_allow_debug")
set(ECS_FLAGS_EXPERIMENTAL "-Decs_allow_experimental")
set(ECS_FLAGS_SAVE_TEMPS "-save-temps")
set(ECS_FLAGS_NO_AUTO_PRUNE "-Decs_disable_auto_prune")
set(ECS_FLAGS_RUN_WITH_BENCHMARKS "-Decs_with_benchmarks")
set(ECS_FLAGS_TEST ${ECS_FLAGS_UNSAFE_METHODS} ${ECS_FLAGS_DEBUG} "-Decs_test")
set(ECS_FLAGS_BENCHMARK ${ECS_FLAGS_TEST} "-Decs_bench_test")

set(ECS_FLAGS)

# Uncomment any of the following to set the desired option
# list(APPEND ECS_FLAGS ${ECS_FLAGS_SAVE_TEMPS})
list(APPEND ECS_FLAGS ${ECS_FLAGS_STANDARD})
list(APPEND ECS_FLAGS ${ECS_FLAGS_UNSAFE_METHODS})
list(APPEND ECS_FLAGS ${ECS_FLAGS_WARN})
list(APPEND ECS_FLAGS ${ECS_FLAGS_DEBUG})
list(APPEND ECS_FLAGS ${ECS_FLAGS_RUN_WITH_BENCHMARKS})
# list(APPEND ECS_FLAGS ${ECS_FLAGS_EXPERIMENTAL})
# list(APPEND ECS_FLAGS ${ECS_FLAGS_SANITIZE})
# list(APPEND ECS_FLAGS ${ECS_FLAGS_TEST})
# list(APPEND ECS_FLAGS ${ECS_FLAGS_BENCHMARK})

# list(APPEND ECS_FLAGS ${ECS_FLAGS_NO_AUTO_PRUNE})
list(APPEND ECS_FLAGS ${ECS_FLAGS_OPTIMIZATIONS})
list(APPEND ECS_FLAGS ${ECS_FLAGS_UNSAFE_CASTS})
# list(APPEND ECS_FLAGS ${ECS_FLAGS_WIN_COMPAT})
# list(APPEND ECS_FLAGS ${ECS_FLAGS_DIST})

# Link libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL_ttf REQUIRED)
set(TTF_LIBS SDL2_ttf)

# Create the executable
add_executable(main ${SOURCES})

# Link the libraries
target_link_libraries(main SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf)
# Apply compiler flags to the target
target_compile_options(main PRIVATE ${ECS_FLAGS})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
