#pragma once

#include "core.hpp"
#include "timer.hpp"
#include "utilities.hpp"

namespace Tags
{

struct Stack
{
};
struct Event
{
};
struct NoStack
{
};
struct Transform
{
};

struct Effect
{
    bool cleanup{false};
    std::optional<Timer> timer;

    Effect()
    {
    }

    Effect(float _duration) : timer{_duration}
    {
    }
};

template <typename T> bool constexpr isTransform()
{
    return isBase<T, Transform>();
}

template <typename T> constexpr bool isNotStacked()
{
    return isBase<T, NoStack>();
}

template <typename T> constexpr bool isEvent()
{
    return isBase<T, Event>();
}

template <typename T> constexpr bool isEffect()
{
    return isBase<T, Effect>();
}

constexpr bool shouldDefaultToStack()
{
    return defaultComponentStacking;
}

template <typename T> constexpr bool isStacked()
{
    if (isNotStacked<T>())
        return false;

    if (isEvent<T>())
        return true;

    if (isBase<T, Stack>())
        return true;

    return shouldDefaultToStack();
}

} // namespace Tags
