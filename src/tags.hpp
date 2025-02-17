#pragma once

#include "timer.hpp"
#include "utilities.hpp"

constexpr bool defaultComponentStacking = false;

namespace ECS
{
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
    std::optional<ECS::internal::Utilities::Timer> timer;

    Effect()
    {
    }

    Effect(float _duration) : timer{_duration}
    {
    }
};

} // namespace Tags

namespace internal
{
namespace Utilities
{
template <typename T> bool constexpr isTransform()
{
    return isBase<T, Tags::Transform>();
}

template <typename T> constexpr bool isNotStacked()
{
    return isBase<T, Tags::NoStack>();
}

template <typename T> constexpr bool isEvent()
{
    return isBase<T, Tags::Event>();
}

template <typename T> constexpr bool isEffect()
{
    return isBase<T, Tags::Effect>();
}

constexpr bool shouldDefaultToStack()
{
    return defaultComponentStacking;
}

template <typename T> constexpr bool isStacked()
{
    return isBase<T, Tags::Stack>();
}

template <typename T> constexpr bool isRequired()
{
    return isBase<T, Tags::Required>();
}

template <typename T> constexpr bool isUnique()
{
    return isBase<T, Tags::Unique>();
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

} // namespace Utilities
} // namespace internal
} // namespace ECS
