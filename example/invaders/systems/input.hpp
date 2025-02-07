#pragma once

#include "../components.hpp"
#include "../core.hpp"
#include "../utilities.hpp"

namespace Systems::Input
{
inline void cleanup(ECM &ecm)
{
}

inline void movePlayer(ECM &ecm)
{
    float dt = Utilties::getDeltaTime(ecm);
    ecm.getAll<PlayerInputEvent>().each([&](EId eId, auto &playerInputEvents) {
        auto &speeds = ecm.get<MovementComponent>(eId).peek(&MovementComponent::speeds);
        float baseSpeed = speeds.x * dt;
        playerInputEvents.inspect([&](const PlayerInputEvent &inputEvent) {
            using Actions = decltype(PlayerInputEvent::action);
            switch (inputEvent.action)
            {
            case Actions::SHOOT:
                ecm.add<AttackEvent>(eId, 3);
                break;
            case Actions::QUIT: {
                auto [gameId, _] = ecm.get<GameComponent>();
                ecm.add<GameEvent>(gameId, GameEvents::QUIT);
                break;
            }
            default:
                break;
            }

            using Movements = decltype(PlayerInputEvent::movement);
            switch (inputEvent.movement)
            {
            case Movements::LEFT:
                ecm.add<MovementEvent>(eId, Vector2{-1 * baseSpeed, 0});
                break;
            case Movements::RIGHT:
                ecm.add<MovementEvent>(eId, Vector2{baseSpeed, 0});
                break;
            default:
                break;
            }
        });
    });
}

inline auto update(ECM &ecm)
{
    movePlayer(ecm);

    return cleanup;
};
}; // namespace Systems::Input
