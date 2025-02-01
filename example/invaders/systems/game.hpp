#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Game
{
inline void cleanup(ECM &ecm)
{
}

inline auto update(ECM &ecm)
{
    ecm.getAll<GameEvent>().each([&](EId eId, auto &gameEvents) {
        gameEvents.inspect([&](const GameEvent &gameEvent) {
            switch (gameEvent.event)
            {
            case GameEvents::QUIT: {
                auto [_, gameComps] = ecm.getUniqueEntity<GameComponent>();
                gameComps.mutate([&](GameComponent &gameComp) { gameComp.isGameOver = true; });
                break;
            }
            case GameEvents::GAME_OVER:
                PRINT("GAME OVER")
                break;
            default:
                break;
            }
        });
    });

    return cleanup;
};
}; // namespace Systems::Game
