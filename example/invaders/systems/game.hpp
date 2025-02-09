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
            auto [_, gameComps] = ecm.get<GameComponent>();
            gameComps.mutate([&](GameComponent &gameComp) {
                switch (gameEvent.event)
                {
                case GameEvents::QUIT: {
                    PRINT("GAME QUIT")
                    gameComps.mutate([&](GameComponent &gameComp) { gameComp.isGameOver = true; });
                    break;
                }
                case GameEvents::GAME_OVER: {
                    PRINT("GAME OVER")
                    Utilties::nextStage(ecm, -1);
                    auto [playerId, _] = ecm.get<PlayerComponent>();
                    ecm.add<DeactivatedComponent>(playerId);
                    break;
                }
                case GameEvents::NEXT_STAGE: {
                    auto [startTriggerId, _] = ecm.get<StartGameTriggerComponent>();
                    if (eId == startTriggerId)
                    {
                        gameComp.currentStage = 1;
                        Utilties::nextStage(ecm, 1);
                        ecm.clearEntities<TitleScreenComponent>();
                    }
                    else
                    {
                        PRINT("STAGE CLEARED!!")
                        Utilties::nextStage(ecm, ++gameComp.currentStage);
                    }
                    break;
                }
                default:
                    break;
                }
            });
        });
    });

    return cleanup;
};
}; // namespace Systems::Game
