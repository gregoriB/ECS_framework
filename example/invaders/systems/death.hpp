#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Death
{
inline void cleanup(ECM &ecm)
{
    auto deadIds = ecm.getEntityIds<DeathComponent>();
    for (const auto &id : deadIds)
        ecm.clearEntity(id);
}

inline auto update(ECM &ecm)
{
    std::vector<EntityId> ids{};
    ecm.getAll<DeathEvent>().each([&](EId eId, auto &deathEvents) {
        auto [playerId, _] = ecm.get<PlayerComponent>();
        if (eId == playerId)
        {
            ecm.add<PlayerEvent>(eId, PlayerEvents::DEATH);
            return;
        }

        deathEvents.inspect([&](const DeathEvent &deathEvent) {
            auto &pointsComps = ecm.get<PointsComponent>(eId);
            if (!pointsComps)
                return;

            ecm.add<ScoreEvent>(deathEvent.killedBy, eId);
        });

        ecm.add<DeathComponent>(eId);
    });

    return cleanup;
};
}; // namespace Systems::Death
