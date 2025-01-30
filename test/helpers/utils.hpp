#pragma once

#include "../../core.hpp"
#include <cassert>

inline std::function<void(Tags::Effect &)> markForCleanup = [](Tags::Effect &effect) {
    effect.cleanup = true;
};

inline std::function<bool(const Tags::Effect &)> isEffectExpired = [](const Tags::Effect &effect) {
    if (effect.cleanup)
        return true;

    if (effect.timer.has_value() && effect.timer->hasElapsed())
        return true;

    return false;
};

template <typename... Components> inline void createEntityWithComponents(ECM &ecm, int entityCount)
{
    for (int i = 0; i < entityCount; ++i)
    {
        (ecm.add<Components>(i), ...);
    }
}
