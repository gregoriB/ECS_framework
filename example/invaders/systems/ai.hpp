#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::AI
{
inline void cleanup(ECM &ecm)
{
}

inline void updateOutsideHiveAliens(ECM &ecm, EId hiveId, const HiveComponent &hiveComp)
{
    auto [x, y, w, h] = hiveComp.bounds.box();
    auto &ids = ecm.getEntityIds<HiveAIComponent>();
    if (ids.empty())
    {
        ecm.clearByEntity<HiveMovementEffect>(hiveId);
        ecm.add<GameEvent>(hiveId, GameEvents::NEXT_STAGE);
        return;
    }

    for (const auto &eId : ids)
    {
        auto [aiX, aiY, aiW, aiH] = ecm.get<PositionComponent>(eId).peek(&PositionComponent::bounds).box();
        if (aiX <= x)
            ecm.add<LeftAlienComponent>(eId);
        if (aiW >= w)
            ecm.add<RightAlienComponent>(eId);
    };
}

inline void updateHiveBounds(ECM &ecm, EId hiveId)
{
    ecm.get<HiveComponent>(hiveId).mutate([&](HiveComponent &hiveComp) {
        constexpr int MIN_INT = std::numeric_limits<int>::min();
        constexpr int MAX_INT = std::numeric_limits<int>::max();

        Vector2 topLeft{MAX_INT, MAX_INT};
        Vector2 bottomRight{MIN_INT, MIN_INT};

        ecm.getAll<HiveAIComponent>().each([&](EId eId, auto &_) {
            ecm.get<PositionComponent>(eId).inspect([&](const PositionComponent &posComp) {
                auto [x, y, w, h] = posComp.bounds.box();
                if (x < topLeft.x)
                    topLeft.x = x;
                if (y < topLeft.y)
                    topLeft.y = y;
                if (w > bottomRight.x)
                    bottomRight.x = w;
                if (h > bottomRight.y)
                    bottomRight.y = h;
            });
        });

        hiveComp.bounds = Bounds{topLeft, Vector2{bottomRight.x - topLeft.x, bottomRight.y - topLeft.y}};
        updateOutsideHiveAliens(ecm, hiveId, hiveComp);
    });
}

inline void handleHiveShift(ECM &ecm, auto &hiveMovementEffects)
{
    hiveMovementEffects.mutate([&](HiveMovementEffect &hiveMovementEffect) {
        using Movement = decltype(HiveMovementEffect::movement);
        switch (hiveMovementEffect.movement)
        {
        case Movement::LEFT:
            hiveMovementEffect.movement = Movement::DOWN;
            hiveMovementEffect.nextMove = Movement::RIGHT;
            break;
        case Movement::RIGHT:
            hiveMovementEffect.movement = Movement::DOWN;
            hiveMovementEffect.nextMove = Movement::LEFT;
            break;
        }
    });
}

template <typename Movement> inline Vector2 calculateSpeed(const Vector2 &speed, const Movement &movement)
{
    Vector2 calculatedSpeed{0, 0};
    switch (movement)
    {
    case Movement::LEFT:
        calculatedSpeed.x = -1 * speed.x;
        break;
    case Movement::RIGHT:
        calculatedSpeed.x = speed.x;
        break;
    case Movement::DOWN:
        calculatedSpeed.y = speed.y;
        break;
    case Movement::UP:
        calculatedSpeed.y = -1 * speed.y;
        break;
    default:
        break;
    }

    return calculatedSpeed;
}

template <typename Movement> inline bool checkIsOutOfBounds(ECM &ecm, EId hiveId, Movement &movement)
{
    auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();
    auto &hiveSpeeds = ecm.get<MovementComponent>(hiveId).peek(&MovementComponent::speeds);

    auto &hiveAiIds = movement == Movement::LEFT ? ecm.getEntityIds<LeftAlienComponent>()
                                                 : ecm.getEntityIds<RightAlienComponent>();
    if (!hiveAiIds.size())
        return false;

    auto &posComps = ecm.get<PositionComponent>(hiveAiIds[0]);
    return posComps.check([&](const PositionComponent &positionComp) {
        auto [x, y] = calculateSpeed(hiveSpeeds, movement);
        Bounds newBounds{
            positionComp.bounds.position.x + x,
            positionComp.bounds.position.y + y,
            positionComp.bounds.size.x + x,
            positionComp.bounds.size.y + y,
        };

        auto [gX, gY, gW, gH] = gameComps.peek(&GameComponent::bounds).box();
        auto [nX, nY, nW, nH] = newBounds.box();

        return nX <= gX || nY <= gY || nW >= gW || nH >= gH;
    });
}

inline bool checkHiveOutOfBounds(ECM &ecm, EId hiveId, auto &hiveMovementEffects)
{
    auto [leftAlienComps, rightAlienComps] = ecm.gatherAll<LeftAlienComponent, RightAlienComponent>();
    if (!leftAlienComps || !rightAlienComps)
        updateHiveBounds(ecm, hiveId);

    auto movement = hiveMovementEffects.peek(&HiveMovementEffect::movement);
    using Movement = decltype(movement);

    switch (movement)
    {
    case Movement::LEFT:
    case Movement::RIGHT:
        return checkIsOutOfBounds(ecm, hiveId, movement);
    }

    return false;
}

inline void moveHiveAI(ECM &ecm, EId hiveId, auto &hiveMovementEffects)
{
    auto movement = hiveMovementEffects.peek(&HiveMovementEffect::movement);
    auto &speeds = ecm.get<MovementComponent>(hiveId).peek(&MovementComponent::speeds);
    auto newSpeed = calculateSpeed(speeds, movement);
    if (!newSpeed.x && !newSpeed.y)
        return;

    for (const auto &eId : ecm.getEntityIds<HiveAIComponent>())
        ecm.add<MovementEvent>(eId, std::move(newSpeed));
}

inline void updateHiveMovement(ECM &ecm, EId hiveId, auto &hiveMovementEffects)
{
    hiveMovementEffects.mutate([&](HiveMovementEffect &hiveMovementEffect) {
        using Movement = decltype(HiveMovementEffect::movement);
        if (hiveMovementEffect.movement == Movement::DOWN)
            hiveMovementEffect.movement = hiveMovementEffect.nextMove;

        hiveMovementEffect.timer->restart();
    });
}

inline bool checkShouldHiveAIMove(Components<HiveMovementEffect> &hiveMovementEffects)
{
    return hiveMovementEffects.check(
        [&](const HiveMovementEffect &hiveMovementEffect) { return hiveMovementEffect.timer->hasElapsed(); });
}

inline void updateHive(ECM &ecm)
{
    auto [hiveId, hiveMovementEffects] = ecm.getUniqueEntity<HiveMovementEffect>();
    if (checkHiveOutOfBounds(ecm, hiveId, hiveMovementEffects))
        handleHiveShift(ecm, hiveMovementEffects);

    if (checkShouldHiveAIMove(hiveMovementEffects))
    {
        moveHiveAI(ecm, hiveId, hiveMovementEffects);
        updateHiveMovement(ecm, hiveId, hiveMovementEffects);
    }
}

inline auto update(ECM &ecm)
{
    updateHive(ecm);

    return cleanup;
};
}; // namespace Systems::AI
