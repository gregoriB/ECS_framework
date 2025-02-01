#pragma once

#include "../components.hpp"
#include "../core.hpp"
#include "../utilities.hpp"

namespace Systems::Movement
{
inline void cleanup(ECM &ecm)
{
}

inline void handleHiveShift(ECM &ecm, auto &hiveMovementEffects)
{
    hiveMovementEffects.mutate([&](HiveMovementEffect &hiveMovementEffect) {
        using Movements = decltype(HiveMovementEffect::movement);
        switch (hiveMovementEffect.movement)
        {
        case Movements::LEFT:
            hiveMovementEffect.movement = Movements::DOWN;
            hiveMovementEffect.nextMove = Movements::RIGHT;
            break;
        case Movements::RIGHT:
            hiveMovementEffect.movement = Movements::DOWN;
            hiveMovementEffect.nextMove = Movements::LEFT;
            break;
        }

        hiveMovementEffect.timer->stop();
    });
}

inline auto checkOutOfBounds(const Bounds &countainer, const Bounds &subject)
{
    auto [cX, cY, cW, cH] = countainer.box();
    auto [sX, sY, sW, sH] = subject.box();

    return sX <= cX || sY <= cY || sW >= cW || sH >= cH;
}

inline Bounds calculateNewBounds(auto &movementEvents, const PositionComponent &positionComp)
{
    MovementEvent reduced = movementEvents.reduce([&](MovementEvent &acc, const MovementEvent &current) {
        acc.coords.x += current.coords.x;
        acc.coords.y += current.coords.y;
    });

    auto [rX, rY] = reduced.coords;
    auto [pX, pY, pW, pH] = positionComp.bounds.get();

    return Bounds{pX + rX, pY + rY, pW, pH};
}

inline void updateHiveAIMovement(ECM &ecm, auto &hiveAiCompSet)
{
    bool isInBounds{true};

    auto [hiveId, hiveMovementEffects] = ecm.getUniqueEntity<HiveMovementEffect>();
    hiveAiCompSet.each([&](EId eId, auto &_) {
        auto [movementEvents, positionComps] = ecm.gather<MovementEvent, PositionComponent>(eId);

        bool isOutOfBounds = positionComps.check([&](const PositionComponent &positionComp) {
            auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();
            auto &gameBounds = gameComps.peek(&GameComponent::bounds);
            auto newBounds = calculateNewBounds(movementEvents, positionComp);

            return checkOutOfBounds(gameBounds, newBounds);
        });

        isInBounds = !isOutOfBounds;

        return isOutOfBounds ? BREAK : CONTINUE;
    });

    if (!isInBounds)
    {
        handleHiveShift(ecm, hiveMovementEffects);
        return;
    }

    auto movement = hiveMovementEffects.peek(&HiveMovementEffect::movement);
    ecm.getAll<HiveAIComponent>().each([&](EId eId, auto &_) {
        auto [movementEvents, positionComps] = ecm.gather<MovementEvent, PositionComponent>(eId);
        if (!movementEvents)
            return;

        positionComps.inspect([&](const PositionComponent &positionComp) {
            auto [newX, newY, w, h] = calculateNewBounds(movementEvents, positionComp).get();

            ecm.add<CollisionCheckEvent>(eId, Bounds{newX, newY, w, h});
            ecm.add<PositionEvent>(eId, Vector2{newX, newY});
        });
    });
}

inline void applyMovementEffects(ECM &ecm)
{
    auto dt = Utilties::getDeltaTime(ecm);
    ecm.getAll<MovementEffect>().each([&](EId eId, auto &movementEffects) {
        movementEffects.inspect([&](const MovementEffect &movementEffect) {
            auto [movementComps, positionComps] = ecm.gather<MovementComponent, PositionComponent>(eId);
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
}

inline void updateHiveMovement(ECM &ecm)
{
    auto [hiveId, hiveMovementEffects] = ecm.getUniqueEntity<HiveMovementEffect>();
    auto [leftAlienComps, rightAlienComps] = ecm.gatherAll<LeftAlienComponent, RightAlienComponent>();
    if (!leftAlienComps || !rightAlienComps)
        Utilties::updateHiveBounds(ecm, hiveId);

    auto &currentMovement = hiveMovementEffects.peek(&HiveMovementEffect::movement);
    using Movements = decltype(HiveMovementEffect::movement);
    switch (currentMovement)
    {
    case Movements::LEFT:
        updateHiveAIMovement(ecm, ecm.getAll<LeftAlienComponent>());
        break;
    case Movements::RIGHT:
        updateHiveAIMovement(ecm, ecm.getAll<RightAlienComponent>());
        break;
    }
}

inline void updateOtherMovement(ECM &ecm)
{
    ecm.getAll<MovementEvent>().each([&](EId eId, auto &movementEvents) {
        auto [positionComps, projectileComps, playerComps, hiveAIComps] =
            ecm.gather<PositionComponent, ProjectileComponent, PlayerComponent, HiveAIComponent>(eId);

        if (hiveAIComps)
            return;

        positionComps.inspect([&](const PositionComponent &positionComp) {
            auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();
            auto &gameBounds = gameComps.peek(&GameComponent::bounds);
            auto [gX, gY, gW, gH] = gameBounds.box();

            auto newBounds = calculateNewBounds(movementEvents, positionComp);
            auto [newX, newY, newW, newH] = newBounds.box();
            auto [w, h] = positionComp.bounds.size;

            bool isOutOfBounds = checkOutOfBounds(gameBounds, newBounds);
            if (isOutOfBounds)
            {
                if (playerComps)
                    return;

                if (projectileComps && (newH < gY || newY > gH))
                {
                    ecm.add<DeathEvent>(eId, 0);
                    return;
                }
            }

            ecm.add<CollisionCheckEvent>(eId, Bounds{newX, newY, w, h});
            ecm.add<PositionEvent>(eId, Vector2{newX, newY});
        });
    });
}

inline auto update(ECM &ecm)
{
    applyMovementEffects(ecm);
    updateHiveMovement(ecm);
    updateOtherMovement(ecm);

    return cleanup;
};
}; // namespace Systems::Movement
