#pragma once

#include "timer.hpp"
#include "utilities.hpp"

constexpr bool defaultComponentStacking = false;

namespace ECS {
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
struct Required
{
};
struct Unique
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

namespace Utils {
template <typename T, typename Base> [[nodiscard]] constexpr bool isBase()
{
    return Utilities::isBase<T, Base>();
}

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
    return isBase<T, Stack>();
}

template <typename T> constexpr bool isRequired()
{
    return isBase<T, Required>();
}

template <typename T> constexpr bool isUnique()
{
    return isBase<T, Unique>();
}

template <typename T> constexpr bool shouldStack()
{
    if (isNotStacked<T>())
        return false;

    if (isEvent<T>())
        return true;

    if (isStacked<T>())
        return true;

    return shouldDefaultToStack();
}

}
}
} // namespace Tags
