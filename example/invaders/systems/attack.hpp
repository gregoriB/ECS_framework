#pragma once

#include "../components.hpp"
#include "../core.hpp"
#include "../entities.hpp"
#include "../utilities.hpp"

namespace Systems::Attack
{
inline void cleanup(ECM &ecm)
{
    Utilties::cleanupEffect<AttackEffect>(ecm);
}

inline void updateAttackEffect(ECM &ecm)
{
    auto [attackEffectSet] = ecm.getAll<AttackEffect>();
    attackEffectSet.each([&](EId eId, auto &attackEffects) {
        // clang-format off
        attackEffects
            .filter([&](const AttackEffect &effect) { 
                auto [projectileComps] = ecm.get<ProjectileComponent>(effect.attackId);
                return !projectileComps || effect.timer->hasElapsed();
            })
            .mutate([&](auto &effect) { effect.cleanup = true; });
        // clang-format on
    });
}

inline void processAttacks(ECM &ecm)
{
    auto [attackEventSet] = ecm.getAll<AttackEvent>();
    attackEventSet.each([&](EId eId, auto &attackEvents) {
        attackEvents.inspect([&](const AttackEvent &attackEvent) {
            auto [attackEffects] = ecm.get<AttackEffect>(eId);
            if (attackEffects)
                return;

            auto [positionComps, attackComps] = ecm.get<PositionComponent, AttackComponent>(eId);
            auto &bounds = positionComps.peek(&PositionComponent::bounds);
            auto direction = attackComps.peek(&AttackComponent::direction);

            using Movements = decltype(direction);
            EntityId projectileId;
            switch (direction)
            {
            case (Movements::UP):
                projectileId = createUpwardProjectile(ecm, eId, bounds);
                break;
            case (Movements::DOWN):
                projectileId = createDownwardProjectile(ecm, eId, bounds);
                break;
            default:
                return;
            }

            ecm.add<AttackEffect>(eId, projectileId, attackEvent.timeout);
        });
    });
}

inline auto update(ECM &ecm)
{
    processAttacks(ecm);
    updateAttackEffect(ecm);

    return cleanup;
};
} // namespace Systems::Attack
