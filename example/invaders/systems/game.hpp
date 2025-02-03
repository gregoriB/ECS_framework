#pragma once

#include "../components.hpp"
#include "../core.hpp"
#include "../utilities.hpp"

namespace Systems::Game
{
inline void cleanup(ECM &ecm)
{
}

inline auto update(ECM &ecm)
{
    ecm.getAll<GameEvent>().each([&](EId eId, auto &gameEvents) {
        gameEvents.inspect([&](const GameEvent &gameEvent) {
            auto [_, gameComps] = ecm.getUnique<GameComponent>();
            gameComps.mutate([&](GameComponent &gameComp) {
                switch (gameEvent.event)
                {
                case GameEvents::QUIT: {
                    gameComps.mutate([&](GameComponent &gameComp) { gameComp.isGameOver = true; });
                    break;
                }
                case GameEvents::GAME_OVER:
                    PRINT("GAME OVER")
                    break;
                case GameEvents::NEXT_STAGE:
                    PRINT("STAGE CLEARED!!")
                    Utilties::nextStage(ecm, ++gameComp.currentStage);
                    break;
                default:
                    break;
                }
            });
        });
    });

    return cleanup;
};
}; // namespace Systems::Game
