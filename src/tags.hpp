#pragma once

#include "timer.hpp"
#include "utilities.hpp"

namespace Tags
{

struct Component
{
};

struct Event : public Component
{
};
struct Unique : public Component
{
};
struct Transform : public Component
{
};

using Stacked = Component;

struct Timed : public Component
{
    std::optional<Timer> timer;

    Timed()
    {
    }

    Timed(float _seconds) : timer{_seconds}
    {
    }
};

struct Effect : public Timed
{
    bool cleanup{false};

    Effect()
    {
    }

    Effect(float _duration) : Timed(_duration)
    {
    }
};

template <typename T> bool constexpr isComponent()
{
    return isBase<T, Component>();
}

template <typename T> bool constexpr isTransform()
{
    return isBase<T, Transform>();
}

template <typename T> constexpr bool isUnique()
{
    return isBase<T, Unique>();
}

template <typename T> constexpr bool isEvent()
{
    return isBase<T, Event>();
}

template <typename T> constexpr bool isEffect()
{
    return isBase<T, Effect>();
}

template <typename T> constexpr bool isStacked()
{
    return !isUnique<T>();
}

} // namespace Tags
