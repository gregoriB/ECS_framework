#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::AI
{
inline void cleanup(ECM &ecm)
{
}

inline void moveHive(ECM &ecm)
{
    auto [hiveId, hiveMovementEffect] = ecm.getUniqueEntity<HiveMovementEffect>();
    hiveMovementEffect.mutate([&](HiveMovementEffect &hiveMovementEffect) {
        auto &timer = hiveMovementEffect.timer;
        if (!timer->hasElapsed() && !timer->isStopped())
            return;

        ecm.getAll<HiveAIComponent>().each(
            [&](EId eId, auto &_) { ecm.add<AIInputEvent>(eId, hiveMovementEffect.movement); });

        using Movements = decltype(HiveMovementEffect::movement);
        if (hiveMovementEffect.movement == Movements::DOWN)
            hiveMovementEffect.movement = hiveMovementEffect.nextMove;

        hiveMovementEffect.timer->restart();
    });
}
inline auto update(ECM &ecm)
{
    moveHive(ecm);

    return cleanup;
};
}; // namespace Systems::AI
