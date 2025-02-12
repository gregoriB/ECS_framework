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
    auto [gameEventSet] = ecm.getAll<GameEvent>();
    gameEventSet.each([&](EId eId, auto &gameEvents) {
        gameEvents.inspect([&](const GameEvent &gameEvent) {
            auto [_, gameComps] = ecm.getUnique<GameComponent>();
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
                    Utilties::nextStage(ecm, -999);
                    auto [playerId, _] = ecm.getUnique<PlayerComponent>();
                    ecm.add<DeactivatedComponent>(playerId);
                    break;
                }
                case GameEvents::NEXT_STAGE: {
                    auto [startTriggerId, _] = ecm.getUnique<StartGameTriggerComponent>();
                    if (eId == startTriggerId)
                    {
                        gameComp.currentStage = 1;
                        Utilties::nextStage(ecm, 1);
                        auto entities = ecm.getEntityIds<TitleScreenComponent>();
                        ecm.remove(entities);
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
