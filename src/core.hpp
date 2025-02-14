#pragma once

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
#include <string>
#include <string_view>
#include <sys/types.h>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <stdexcept>
#include <type_traits>

template <typename... Args> constexpr void print(const Args &...args)
{
    std::cout << "\n  ";
    ((std::cout << " " << args), ...);
    std::cout << '\n';
};

template <typename... Args> constexpr void debugWarningPrint(const Args &...args)
{
    std::cout << "\n ================= ECS ENGINE DEBUG LOG ===================\n";
    print(args..., '\n');
};

template <typename... Args> constexpr void benchmarkPrint(const Args &...args)
{
    std::cout << "\n ================= ECS BENCHMARK LOG ===================\n";
    print(args..., '\n');
};

template <typename... Args> constexpr void debugWarningLog(const Args &...args)
{
    // TODO Task : Update to log to file
    std::cout << "\n ================= ECS ENGINE LOG ===================\n";
    print(args..., '\n');
};

inline void _assert(bool condition, std::string m)
{
    if (!condition)
    {
        bool SEE_ABOVE_MESSAGE = false;
        print("!!!!!!!!!    ASSERTION FAILED: ", m, "   !!!!!!!!!");
        assert(SEE_ABOVE_MESSAGE);
    }
}

#define ASSERT(condition, message) _assert(condition, message);

#define PRINT(...) print(__VA_ARGS__);

#ifdef ecs_show_warnings
#define ECS_LOG_WARNING(...) debugWarningPrint(__VA_ARGS__);
#elif ecs_log_warnings
#define ECS_LOG_WARNING(...) debugWarningLog(__VA_ARGS__);
#else
#define ECS_LOG_WARNING(...) ;
#endif

#ifdef ecs_allow_debug
#define PRINT_BENCHMARKS(...) benchmarkPrint(__VA_ARGS__);
#else
#define PRINT_BENCHMARKS(...) ;
#endif


template <typename T> using Transformer = std::function<T(T &)>;

struct DefaultComponent
{
};

constexpr int reservedEntities{10};
