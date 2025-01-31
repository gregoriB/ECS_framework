#pragma once

#include "components.hpp"
#include "core.hpp"
#include "entities.hpp"
#include "utilities.hpp"
#include <functional>

namespace Systems
{

inline auto ai(ECM &ecm)
{
    auto [hiveId, hiveMovementEffect] = ecm.getUniqueEntity<HiveMovementEffect>();
    auto &aiHiveComps = ecm.getAll<HiveAIComponent>();
    hiveMovementEffect.mutate([&](HiveMovementEffect &hiveMovementEffect) {
        auto elapsed = hiveMovementEffect.timer->getElapsedTime();

        ecm.getAll<HiveAIComponent>().each([&](EId eId, auto &_) {
            if (elapsed < hiveMovementEffect.moveInterval)
                return;

            ecm.add<AIInputEvent>(eId, hiveMovementEffect.movement);
        });

        if (elapsed < hiveMovementEffect.moveInterval)
            return;

        if (hiveMovementEffect.movement == Movement::DOWN)
            hiveMovementEffect.movement = Movement::LEFT;

        hiveMovementEffect.timer->restart();
    });

    return [&]() {};
}

inline auto input(ECM &ecm)
{
    float dt = Utilties::getDeltaTime(ecm);
    ecm.getAll<PlayerInputEvent>().each([&](EId eId, auto &playerInputEvents) {
        auto &speeds = ecm.get<MovementComponent>(eId).peek(&MovementComponent::speeds);
        float baseSpeed = speeds.x * dt;
        playerInputEvents.inspect([&](const PlayerInputEvent &inputEvent) {
            switch (inputEvent.action)
            {
            case Action::SHOOT:
                ecm.add<AttackEvent>(eId);
                break;
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
                ecm.add<MovementEvent>(eId, Vector2{-1 * baseSpeed, 0});
                break;
            case Movement::RIGHT:
                ecm.add<MovementEvent>(eId, Vector2{baseSpeed, 0});
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
        float baseSpeedX = speeds.x;
        float baseSpeedY = speeds.y;

        aiInputEvents.inspect([&](const AIInputEvent &inputEvent) {
            switch (inputEvent.action)
            {
            case Action::SHOOT:
                ecm.add<AttackEvent>(eId);
            default:
                break;
            }

            switch (inputEvent.movement)
            {
            case Movement::LEFT:
                ecm.add<MovementEvent>(eId, Vector2{-1 * baseSpeedX, 0});
                break;
            case Movement::RIGHT:
                ecm.add<MovementEvent>(eId, Vector2{baseSpeedX, 0});
                break;
            case Movement::DOWN:
                ecm.add<MovementEvent>(eId, Vector2{0, baseSpeedY});
                break;
            case Movement::UP:
                ecm.add<MovementEvent>(eId, Vector2{0, -1 * baseSpeedY});
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
    auto [movementEffects, movementEvents] = ecm.gatherAll<MovementEffect, MovementEvent>();

    float dt = Utilties::getDeltaTime(ecm);
    movementEffects.each([&](EId eId, auto &movementEffects) {
        movementEffects.inspect([&](const MovementEffect &movementEffect) {
            auto [movementComps, positionComps, aiComps] =
                ecm.gather<MovementComponent, PositionComponent, HiveAIComponent>(eId);
            auto &speeds = movementComps.peek(&MovementComponent::speeds);
            auto &position = positionComps.peek(&PositionComponent::bounds).position;
            auto &targetPos = movementEffect.trajectory;

            // clang-format off
            Vector2 diff{position.x - targetPos.x, position.y - targetPos.y};
            Vector2 directions{
                diff.x < 0 ? 1.0f : diff.x > 0 ? -1.0f : 0,
                diff.y < 0 ? 1.0f : diff.y > 0 ? -1.0f : 0,
            };
            // clang-format on

            auto xMove = speeds.x * dt;
            auto yMove = speeds.y * dt;
            ecm.add<MovementEvent>(eId, Vector2{xMove * directions.x, yMove * directions.y});
        });
    });

    movementEvents.each([&](EId eId, auto &movementEvents) {
        auto [positionComps, projectileComps, hiveAIComps] =
            ecm.gather<PositionComponent, ProjectileComponent, HiveAIComponent>(eId);

        auto reducedEvent = movementEvents.reduce([&](MovementEvent &acc, const MovementEvent &current) {
            acc.coords.x += current.coords.x;
            acc.coords.y += current.coords.y;
        });

        positionComps.inspect([&](const PositionComponent &positionComp) {
            auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();
            auto [gX, gY, gW, gH] = gameComps.peek(&GameComponent::bounds).box();

            auto [x, y] = positionComp.bounds.position;
            auto [w, h] = positionComp.bounds.size;

            float newX = x + reducedEvent.coords.x;
            float newY = y + reducedEvent.coords.y;
            float newW = newX + w;
            float newH = newY + h;

            bool isOutOfBounds = newX <= gX || newW >= gW || newY <= gY || newH >= gH;
            if (isOutOfBounds)
            {
                if (projectileComps)
                {
                    if (newH < gY || newY > gH)
                    {
                        ecm.add<DeathEvent>(eId, 0);
                        return;
                    }
                }

                if (hiveAIComps)
                {
                    if (newW >= gW || newX <= gX)
                    {
                        ecm.getAll<HiveMovementEffect>().each([&](EId hiveId, auto &hiveMovementEffects) {
                            hiveMovementEffects.mutate([&](HiveMovementEffect &hiveMovementEffect) {
                                switch (hiveMovementEffect.movement)
                                {
                                case Movement::LEFT:
                                case Movement::RIGHT:
                                    hiveMovementEffect.movement = Movement::DOWN;
                                    break;
                                }
                            });
                        });
                        return;
                    }
                }
            }

            ecm.add<CollisionCheckEvent>(eId, Bounds{newX, newY, w, h});
            ecm.add<PositionEvent>(eId, Vector2{newX, newY});
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

            auto [pX, pY, pW, pH] = positionComps.peek(&PositionComponent::bounds).box();
            bool isX = (cX >= pX && cX <= pW) || (cW >= pX && cW <= pW);
            bool isY = (cY >= pY && cY <= pH) || (cH >= pY && cH <= pH);
            if (!isX || !isY)
                return;

            PRINT("DEATH", eId1, eId2)
            ecm.add<DeathEvent>(eId1, eId2);
            ecm.add<DeathEvent>(eId2, eId1);
        });
    });

    return [&]() {};
};

inline auto death(ECM &ecm)
{
    auto &deathEventsSet = ecm.getAll<DeathEvent>();
    std::vector<EntityId> ids{};
    deathEventsSet.each([&](EId eId, auto &deathEvents) -> void {
        if (ecm.get<PlayerComponent>(eId))
            ecm.add<GameEvent>(eId, GameEvents::GAME_OVER);

        ids.push_back(eId);
    });

    for (auto &id : ids)
        ecm.clearAllByEntity(id);

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
    ecm.getAll<AttackEvent>().each([&](EId eId, auto &attackEvents) {
        auto &attackEffects = ecm.get<AttackEffect>(eId);
        if (attackEffects)
        {
            attackEffects.mutate([&](AttackEffect &attackEffect) {
                if (!ecm.get<ProjectileComponent>(attackEffect.attackId))
                    attackEffect.cleanup = true;
            });

            return;
        }

        if (!ecm.get<AttackComponent>(eId))
            return;

        auto [positionComps, playerComps, aiComps] =
            ecm.gather<PositionComponent, PlayerComponent, AIComponent>(eId);
        auto &bounds = positionComps.peek(&PositionComponent::bounds);
        EntityId projectileId;
        if (playerComps)
            projectileId = createUpwardProjectile(ecm, bounds);
        else
            projectileId = createDownwardProjectile(ecm, bounds);

        ecm.add<AttackEffect>(eId, projectileId);
    });

    return [&]() {
        ecm.getAll<AttackEffect>().each([&](EId eId, auto &attackEffects) {
            attackEffects.remove([&](const AttackEffect &attackEffect) { return attackEffect.cleanup; });
        });

        ecm.prune<AttackEffect>();
    };
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
                PRINT("GAME OVER")
                break;
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
    // clang-format off
    std::array<CleanupFunc, 9> cleanupFuncs{
        Systems::ai(ecm),
        Systems::input(ecm),
        Systems::attack(ecm),
        Systems::movement(ecm),
        Systems::position(ecm),
        Systems::collision(ecm),
        Systems::death(ecm),
        Systems::player(ecm),
        Systems::game(ecm),
    };

    // clang-format on
    cleanup(ecm, cleanupFuncs);

    auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();

    return !gameComps.peek(&GameComponent::isGameOver);
}
}; // namespace Update

}; // namespace Systems
