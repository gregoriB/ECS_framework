#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Death
{
inline void cleanup(ECM &ecm)
{
}

inline auto update(ECM &ecm)
{
    std::vector<EntityId> ids{};
    ecm.getAll<DeathEvent>().each([&](EId eId, auto &deathEvents) {
        if (ecm.get<PlayerComponent>(eId))
            ecm.add<GameEvent>(eId, GameEvents::GAME_OVER);

        ids.push_back(eId);
    });

    for (auto &id : ids)
        ecm.clearAllByEntity(id);

    return cleanup;
};
}; // namespace Systems::Death
