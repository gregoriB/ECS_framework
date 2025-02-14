add_executable(run_tests run_tests.cpp)

# Ensure test executable knows where to find headers
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link the ECS library
target_link_libraries(run_tests PRIVATE ECS::ecs)

# Register the test with CTest
add_test(NAME testAll COMMAND run_tests)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
