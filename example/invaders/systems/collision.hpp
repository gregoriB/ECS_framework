#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Collision
{
inline void cleanup(ECM &ecm)
{
}

inline bool checkFriendlyFire(ECM &ecm, auto &projectileComps, auto &hiveAiComps)
{
    if (!projectileComps || !hiveAiComps)
        return false;

    using Movement = decltype(ProjectileComponent::movement);
    auto &movement = projectileComps.peek(&ProjectileComponent::movement);
    return hiveAiComps && movement == Movement::DOWN;
}

inline void handleCollisions(ECM &ecm)
{
    ecm.getAll<CollisionCheckEvent>().each([&](EId eId1, auto &checkEvents) {
        auto [projectile1, hiveAiComps1] = ecm.gather<ProjectileComponent, HiveAIComponent>(eId1);
        auto [cX, cY, cW, cH] = checkEvents.peek(&CollisionCheckEvent::bounds).box();
        ecm.gatherGroup<CollidableComponent, PositionComponent>().each(
            [&](EId eId2, auto &collidableComps2, auto &positionComps2) {
                if (eId1 == eId2)
                    return;

                auto [projectile2, hiveAiComps2] = ecm.gather<ProjectileComponent, HiveAIComponent>(eId2);
                if (checkFriendlyFire(ecm, projectile2, hiveAiComps1) ||
                    checkFriendlyFire(ecm, projectile1, hiveAiComps2))
                    return;

                auto [pX, pY, pW, pH] = positionComps2.peek(&PositionComponent::bounds).box();
                bool isX = (cX >= pX && cX <= pW) || (cW >= pX && cW <= pW);
                bool isY = (cY >= pY && cY <= pH) || (cH >= pY && cH <= pH);
                if (!isX || !isY)
                    return;

                EId dealer1 = projectile1 ? projectile1.peek(&ProjectileComponent::shooterId) : eId1;
                EId dealer2 = projectile2 ? projectile2.peek(&ProjectileComponent::shooterId) : eId2;
                ecm.add<DamageEvent>(eId1, dealer2);
                ecm.add<DamageEvent>(eId2, dealer1);
            });
    });
}

inline auto update(ECM &ecm)
{
    handleCollisions(ecm);

    return cleanup;
};
}; // namespace Systems::Collision
