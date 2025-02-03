#pragma once

inline constexpr bool defaultComponentStacking = false;

#include "tags.hpp"
#include "utilities.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

template <typename... Args> constexpr void print(const Args &...args)
{
    std::cout << "\n  ";
    ((std::cout << " " << args), ...);
    std::cout << '\n';
};

inline template <typename... Args> constexpr void debugWarningPrint(const Args &...args)
{
    std::cout << "\n ================= GAME ENGINE DEBUG LOG ===================\n";
    print(args..., '\n');
};

inline template <typename... Args> constexpr void benchmarkPrint(const Args &...args)
{
    std::cout << "\n ================= GAME ENGINE BENCHMARK LOG ===================\n";
    print(args..., '\n');
};

inline template <typename... Args> constexpr void debugWarningLog(const Args &...args)
{
    // TODO Task : Update to log to file
    std::cout << "\n ================= GAME ENGINE LOG ===================\n";
    print(args..., '\n');
};

#define PRINT(...) print(__VA_ARGS__);

#ifdef ecs_show_game_data
#define PRINT_GAME_DATA(...) print(__VA_ARGS__);
#else
#define PRINT_GAME_DATA(...) ;
#endif

#ifdef ecs_show_warnings
#define ECS_LOG_WARNING(...) debugWarningPrint(__VA_ARGS__);
#elif ECS_LOG_WARNINGs
#define ECS_LOG_WARNING(...) debugWarningLog(__VA_ARGS__);
#else
#define ECS_LOG_WARNING(...) ;
#endif

#ifdef ecs_allow_debug
#define PRINT_BENCHMARKS(...) benchmarkPrint(__VA_ARGS__);
#else
#define PRINT_BENCHMARKS(...) ;
#endif

inline constexpr size_t reservedEntities = 10;

enum class State
{
    NONE = 0,
    QUIT,
    RESET,
};

inline void withBenchmarks(std::function<float()> fn)
{
    auto start = std::chrono::high_resolution_clock::now();

    int cycles = fn();
    if (cycles == 0)
        cycles = 1;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> duration = end - start;
    auto avg = duration.count() / cycles;
    auto framerate = cycles / (avg * cycles);

    PRINT_BENCHMARKS("average frame time:", avg, "for", cycles, "frames\n", "  average FPS:", framerate);
};

enum class Transformation
{
    DEFAULT,
    PRESERVE,
    TRANSFORM
};
