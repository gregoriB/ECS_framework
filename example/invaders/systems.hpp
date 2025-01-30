#pragma once

#include "components.hpp"
#include "core.hpp"
#include "entities.hpp"
#include "utilities.hpp"
#include <functional>

namespace Systems
{
inline auto input(ECM &ecm)
{
    float dt = Utilties::getDeltaTime(ecm);

    ecm.getAll<PlayerInputEvent>().each([&](EId eId, auto &playerInputEvents) {
        if (ecm.get<DeactivatedComponent>(eId))
            return;

        auto &speeds = ecm.get<MovementComponent>(eId).peek(&MovementComponent::speeds);
        playerInputEvents.inspect([&](const PlayerInputEvent &inputEvent) {
            switch (inputEvent.action)
            {
            case Action::SHOOT:
                ecm.add<AttackEvent>(eId);
            case Action::QUIT: {
                auto [gameId, _] = ecm.getUniqueEntity<GameComponent>();
                ecm.add<GameEvent>(gameId, GameEvents::QUIT);
                break;
            }
            default:
                break;
            }

            switch (inputEvent.movement)
            {
            case Movement::LEFT:
                ecm.add<MovementEvent>(eId, Vector2{-speeds.x + (dt * 0.5f), 0});
                break;
            case Movement::RIGHT:
                ecm.add<MovementEvent>(eId, Vector2{speeds.x + (dt * 0.5f), 0});
                break;
            default:
                break;
            }
        });
    });

    ecm.getAll<AIInputEvent>().each([&](EId eId, auto &aiInputEvents) {
        if (ecm.get<DeactivatedComponent>(eId))
            return;

        auto &speeds = ecm.get<MovementComponent>(eId).peek(&MovementComponent::speeds);
        aiInputEvents.inspect([&](const AIInputEvent &inputEvent) {
            switch (inputEvent.action)
            {
            case Action::SHOOT:
                ecm.add<AttackEffect>(eId);
            default:
                break;
            }

            switch (inputEvent.movement)
            {
            case Movement::LEFT:
                ecm.add<MovementEvent>(eId, Vector2{-speeds.x + (dt * 0.5f), 0});
                break;
            case Movement::RIGHT:
                ecm.add<MovementEvent>(eId, Vector2{speeds.x + (dt * 0.5f), 0});
                break;
            case Movement::DOWN:
                ecm.add<MovementEvent>(eId, Vector2{speeds.y + (dt * 0.5f), 0});
                break;
            case Movement::UP:
                ecm.add<MovementEvent>(eId, Vector2{-speeds.y + (dt * 0.5f), 0});
                break;
            default:
                break;
            }
        });
    });

    return [&]() {};
};

inline auto movement(ECM &ecm)
{
    ecm.getAll<MovementEvent>().each([&](EId eId, auto &movementEvents) {
        auto [playerComps, aiComps, positionComps] =
            ecm.gather<PlayerComponent, AIComponent, PositionComponent>(eId);
        auto reducedEvent = movementEvents.reduce([&](MovementEvent acc, const MovementEvent &current) {
            acc.coords.x += current.coords.x;
            acc.coords.y += current.coords.y;
        });

        positionComps.inspect([&](const PositionComponent &positionComp) {
            auto [x, y] = positionComp.bounds.position;
            auto [w, h] = positionComp.bounds.size;
            float newX = x + reducedEvent.coords.x;
            float newY = y + reducedEvent.coords.y;
            float newW = newX + w;
            float newH = newY + h;

            auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();
            auto [gX, gY, gW, gH] = gameComps.peek(&GameComponent::bounds).box();

            bool isOutOfBounds = newX < gX || newW > gW || newY < gY || newH > gH;
            if (isOutOfBounds)
                return;

            ecm.add<PositionEvent>(eId, Vector2{newX, newY});
            ecm.add<CollisionCheckEvent>(eId, Bounds{newX, newY, w, h});
        });
    });

    return [&]() {};
};

inline auto collision(ECM &ecm)
{
    ecm.getAll<CollisionCheckEvent>().each([&](EId eId1, auto &checkEvents) {
        auto [cX, cY, cW, cH] = checkEvents.peek(&CollisionCheckEvent::bounds).box();

        ecm.getAll<PositionComponent>().each([&](EId eId2, auto &positionComps) {
            if (eId1 == eId2)
                return;

            auto [pX, pY, pH, pW] = positionComps.peek(&PositionComponent::bounds).box();
            bool isX = (cX >= pX && cX <= pW) || (cW >= pX && cW <= pW);
            bool isY = (cY >= pY && cY <= pH) || (cH >= pY && cH <= pH);
            if (!isX || !isY)
                return;

            ecm.add<DeathEvent>(eId1, eId2);
        });
    });

    return [&]() {};
};

inline auto death(ECM &ecm)
{
    ecm.getAll<DeathEvent>().each([&](EId eId, auto &deathEvents) {
        ecm.add<DeathComponent>(eId);
        ecm.add<DeactivatedComponent>(eId);

        if (ecm.get<PlayerComponent>(eId))
            ecm.add<GameEvent>(eId, GameEvents::GAME_OVER);
    });

    return [&]() {};
};

inline auto position(ECM &ecm)
{
    ecm.getAll<PositionEvent>().each([&](EId eId, auto &positionEvents) {
        positionEvents.first().inspect([&](const PositionEvent &positionEvent) {
            ecm.get<PositionComponent>(eId).mutate([&](PositionComponent &positionComp) {
                positionComp.bounds.position.x = positionEvent.coords.x;
                positionComp.bounds.position.y = positionEvent.coords.y;
            });
        });
    });

    return [&]() {};
};

inline auto attack(ECM &ecm)
{
    ecm.getAll<AttackComponent>().each([&](EId eId, auto &attackEvents) {
        if (ecm.get<AttackEffect>(eId))
            return;

        if (!ecm.get<AttackComponent>(eId))
            return;

        auto [positionComps, playerComps, aiComps] =
            ecm.gather<PositionComponent, PlayerComponent, AIComponent>(eId);
        auto &bounds = positionComps.peek(&PositionComponent::bounds);
        ecm.add<AttackEffect>(eId);
        if (playerComps)
            createUpwardProjectile(ecm, bounds);
        else
            createDownwardProjectile(ecm, bounds);
    });

    return [&]() {};
};

inline auto player(ECM &ecm)
{
    // get player events
    // check for death
    // // create game over event if no lives
    // // remove life are if lives

    return [&]() {};
};

inline auto game(ECM &ecm)
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
            default:
                break;
            }
        });
    });

    return [&]() {};
}

namespace Update
{
using CleanupFunc = std::function<void()>;

template <typename CleanupFuncs> inline void cleanup(ECM &ecm, CleanupFuncs &cleanupFuncs)
{
    for (auto &func : cleanupFuncs)
        func();

    ecm.clearByTag<Tags::Event>();
}

inline bool run(ECM &ecm)
{
    std::array<CleanupFunc, 8> cleanupFuncs{
        Systems::input(ecm),     Systems::attack(ecm), Systems::movement(ecm), Systems::position(ecm),
        Systems::collision(ecm), Systems::death(ecm),  Systems::player(ecm),   Systems::game(ecm),
    };

    cleanup(ecm, cleanupFuncs);

    auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();

    return !gameComps.peek(&GameComponent::isGameOver);
}
}; // namespace Update

}; // namespace Systems
