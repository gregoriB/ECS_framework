#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Player
{
inline void cleanup(ECM &ecm)
{
}

inline auto update(ECM &ecm)
{
    auto [playerId, playerComps] = ecm.get<PlayerComponent>();

    ecm.getAll<PlayerEvent>().each([&](EId eId, auto &playerEvents) {
        playerEvents.inspect([&](const PlayerEvent &playerEvent) {
            using Event = decltype(playerEvent.event);
            switch (playerEvent.event)
            {
            case Event::DEATH: {
                auto &livesComps = ecm.get<LivesComponent>(playerId);
                livesComps.mutate([&](LivesComponent &livesComp) { --livesComp.count; });
                auto &lifeCount = livesComps.peek(&LivesComponent::count);
                if (lifeCount <= 0)
                    ecm.add<GameEvent>(eId, GameEvents::GAME_OVER);
                else
                    ecm.add<UIEvent>(eId, UIEvents::UPDATE_LIVES);

                break;
            }
            default:
                break;
            }
        });
    });

    return cleanup;
};
}; // namespace Systems::Player
