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
                ecm.add<AttackEvent>(eId);
                break;
            case Actions::QUIT: {
                auto [gameId, _] = ecm.getUniqueEntity<GameComponent>();
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

inline void moveAI(ECM &ecm)
{
    ecm.getAll<AIInputEvent>().each([&](EId eId, auto &aiInputEvents) {
        if (ecm.get<DeactivatedComponent>(eId))
            return;

        auto &speeds = ecm.get<MovementComponent>(eId).peek(&MovementComponent::speeds);
        float baseSpeedX = speeds.x;
        float baseSpeedY = speeds.y;

        aiInputEvents.inspect([&](const AIInputEvent &inputEvent) {
            using Actions = decltype(AIInputEvent::action);
            switch (inputEvent.action)
            {
            case Actions::SHOOT:
                ecm.add<AttackEvent>(eId);
            default:
                break;
            }

            using Movements = decltype(AIInputEvent::movement);
            switch (inputEvent.movement)
            {
            case Movements::LEFT:
                ecm.add<MovementEvent>(eId, Vector2{-1 * baseSpeedX, 0});
                break;
            case Movements::RIGHT:
                ecm.add<MovementEvent>(eId, Vector2{baseSpeedX, 0});
                break;
            case Movements::DOWN:
                ecm.add<MovementEvent>(eId, Vector2{0, baseSpeedY});
                break;
            case Movements::UP:
                ecm.add<MovementEvent>(eId, Vector2{0, -1 * baseSpeedY});
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
    moveAI(ecm);

    return cleanup;
};
}; // namespace Systems::Input
