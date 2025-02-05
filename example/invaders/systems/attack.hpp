#pragma once

#include "../components.hpp"
#include "../core.hpp"
#include "../entities.hpp"

namespace Systems::Attack
{
inline void cleanup(ECM &ecm)
{
    ecm.getAll<AttackEffect>().each([&](EId eId, auto &attackEffects) {
        attackEffects.remove([&](const AttackEffect &attackEffect) { return attackEffect.cleanup; });
    });

    ecm.prune<AttackEffect>();
}

inline void updateAttackEffect(ECM &ecm)
{
    ecm.getAll<AttackEffect>().each([&](EId eId, auto &attackEffects) {
        // clang-format off
        attackEffects
            .filter([&](const auto &effect) { return !ecm.get<ProjectileComponent>(effect.attackId); })
            .mutate([&](auto &effect) { effect.cleanup = true; });
        // clang-format on
    });
}

inline void processAttacks(ECM &ecm)
{
    ecm.getAll<AttackEvent>().each([&](EId eId, auto &attackEvents) {
        auto &attackEffects = ecm.get<AttackEffect>(eId);
        if (attackEffects)
            return;

        auto [positionComps, attackComps] = ecm.gather<PositionComponent, AttackComponent>(eId);

        auto &bounds = positionComps.peek(&PositionComponent::bounds);
        auto direction = attackComps.peek(&AttackComponent::direction);
        using Movements = decltype(direction);
        EntityId projectileId;
        switch (direction)
        {
        case (Movements::UP):
            projectileId = createUpwardProjectile(ecm, bounds);
            break;
        case (Movements::DOWN):
            projectileId = createDownwardProjectile(ecm, bounds);
            break;
        default:
            return;
        }

        ecm.add<AttackEffect>(eId, projectileId);
    });

    updateAttackEffect(ecm);
}

inline auto update(ECM &ecm)
{
    processAttacks(ecm);

    return cleanup;
};
} // namespace Systems::Attack
