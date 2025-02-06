#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Collision
{
inline void cleanup(ECM &ecm)
{
}

inline bool checkForFriendlyFire(ECM &ecm, auto &projectileComps, auto &playerComps, auto &hiveAiComps)
{
    if (!projectileComps)
        return false;

    using Movement = decltype(ProjectileComponent::movement);
    auto &movement = projectileComps.peek(&ProjectileComponent::movement);
    return playerComps && movement == Movement::UP || hiveAiComps && movement == Movement::DOWN;
}

inline void handleCollisions(ECM &ecm)
{
    ecm.getAll<CollisionCheckEvent>().each([&](EId eId1, auto &checkEvents) {
        auto [projectileComps1, playerComps1, hiveAiComps1] =
            ecm.gather<ProjectileComponent, PlayerComponent, HiveAIComponent>(eId1);
        auto [cX, cY, cW, cH] = checkEvents.peek(&CollisionCheckEvent::bounds).box();
        ecm.getAll<PositionComponent>().each([&](EId eId2, auto &positionComps) {
            if (eId1 == eId2)
                return;

            auto [projectileComps2, playerComps2, hiveAiComps2] =
                ecm.gather<ProjectileComponent, PlayerComponent, HiveAIComponent>(eId2);
            if (checkForFriendlyFire(ecm, projectileComps2, playerComps1, hiveAiComps1) ||
                checkForFriendlyFire(ecm, projectileComps1, playerComps2, hiveAiComps2))
                return;

            auto [pX, pY, pW, pH] = positionComps.peek(&PositionComponent::bounds).box();
            bool isX = (cX >= pX && cX <= pW) || (cW >= pX && cW <= pW);
            bool isY = (cY >= pY && cY <= pH) || (cH >= pY && cH <= pH);
            if (!isX || !isY)
                return;

            ecm.add<DamageEvent>(eId1, eId2);
            ecm.add<DamageEvent>(eId2, eId1);
        });
    });
}

inline auto update(ECM &ecm)
{
    handleCollisions(ecm);

    return cleanup;
};
}; // namespace Systems::Collision
