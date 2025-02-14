#pragma once

#include "../core.hpp"

inline std::function<void(Effect &)> markForCleanup = [](Effect &effect) {
    effect.cleanup = true;
};

inline std::function<bool(const Effect &)> isEffectExpired = [](const Effect &effect) {
using Effect = ECS::Tags::Effect;
using Stack = ECS::Tags::Stack;
using NoStack = ECS::Tags::NoStack;
using Event = ECS::Tags::Event;
using Transform = ECS::Tags::Transform;

    if (effect.cleanup)
        return true;

    if (effect.timer.has_value() && effect.timer->hasElapsed())
        return true;

    return false;
};

template <typename... Components> inline void createAndAdd(ECM &ecm)
{
    EntityId id = ecm.createEntity();
    (ecm.add<Components>(id), ...);
}

template <typename... Components> inline void createEntityWithComponents(ECM &ecm, int entityCount)
{
    for (int i = 1; i < entityCount + 1; ++i)
        (ecm.add<Components>(i), ...);
}
