#pragma once

#include "components.hpp"
#include "timer.hpp"
#include "utilities.hpp"

constexpr bool defaultComponentStacking = false;

namespace ECS
{
namespace Tags
{

/**
 * @brief Forces components to be arranged together in a vector
 */
struct Stack
{
};
/**
 * @brief Always stacks unless specifically tagged otherwise.  Intended to be used with transient components which needs to be batch-removed
 */
struct Event
{
};
/**
 * @brief Prevents components from stacking to allow for only a single instance of a component per entity.  Also gives access to a few additional accessor methods
 */
struct NoStack
{
};
/**
 * @brief Enables component transformation pipelines as the default behavior when a component is accessed
 */
struct Transform
{
};
/**
 * @brief Will cause a failed assertion if the component does not exist
 */
struct Required
{
};
/**
 * @brief Will enforce that only a single instance of a component exists at any given time.  Also provides a more convenient access method
 */
struct Unique
{
};

/**
 * @deprecated This will be removed in a future version once custom tags are implemented
 * 
 * @brief Has no special behavior within the library.  This is a purely game logic component
 */
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
