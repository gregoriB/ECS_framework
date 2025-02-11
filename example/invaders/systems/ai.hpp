#pragma once

#include "../components.hpp"
#include "../core.hpp"
#include "../entities.hpp"
#include "../utilities.hpp"
#include <cstdlib>

namespace Systems::AI
{
inline void cleanup(ECM &ecm)
{
    Utilties::cleanupEffect<AITimeoutEffect, UFOTimeoutEffect>(ecm);
}

inline void updateOutsideHiveAliens(ECM &ecm, EId hiveId, const HiveComponent &hiveComp)
{
    auto &ids = ecm.getEntityIds<HiveAIComponent>();
    if (ids.empty())
    {
        ecm.add<GameEvent>(hiveId, GameEvents::NEXT_STAGE);
        return;
    }

    auto [x, y, w, h] = hiveComp.bounds.box();
    for (const auto &eId : ids)
    {
        auto [positionComps] = ecm.get<PositionComponent>(eId);
        auto [aiX, aiY, aiW, aiH] = positionComps.peek(&PositionComponent::bounds).box();
        if (aiX <= x)
            ecm.add<LeftAlienComponent>(eId);
        if (aiW >= w)
            ecm.add<RightAlienComponent>(eId);
    };
}

inline void updateHiveBounds(ECM &ecm, EId hiveId)
{
    auto [hiveComps] = ecm.get<HiveComponent>(hiveId);
    hiveComps.mutate([&](HiveComponent &hiveComp) {
        constexpr float MIN_FLOAT = std::numeric_limits<float>::min();
        constexpr float MAX_FLOAT = std::numeric_limits<float>::max();

        Vector2 topLeft{MAX_FLOAT, MAX_FLOAT};
        Vector2 bottomRight{MIN_FLOAT, MIN_FLOAT};

        ecm.getGroup<HiveAIComponent, PositionComponent>().each([&](EId eId, auto &_, auto &positionComps) {
            positionComps.inspect([&](const PositionComponent &posComp) {
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
        default:
            break;
        }
    });
}

template <typename Movement>
inline Vector2 calculateSpeed(ECM &ecm, const Vector2 &speed, const Movement &movement)
{
    auto [_, gameComps] = ecm.getUnique<GameComponent>();
    float modifier = gameComps.peek(&GameComponent::currentStage) / 2.0f;
    if (modifier < 1)
        modifier = 1;

    Vector2 calculatedSpeed{0, 0};

    switch (movement)
    {
    case Movement::LEFT:
        calculatedSpeed.x = -1 * speed.x * modifier;
        break;
    case Movement::RIGHT:
        calculatedSpeed.x = speed.x * modifier;
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
    auto [gameId, gameComps] = ecm.getUnique<GameComponent>();
    auto [movementComps] = ecm.get<MovementComponent>(hiveId);
    auto &hiveSpeeds = movementComps.peek(&MovementComponent::speeds);

    auto &hiveAiIds = movement == Movement::LEFT ? ecm.getEntityIds<LeftAlienComponent>()
                                                 : ecm.getEntityIds<RightAlienComponent>();
    if (!hiveAiIds.size())
        return false;

    auto [posComps] = ecm.get<PositionComponent>(hiveAiIds[0]);
    return !!posComps.find([&](const PositionComponent &positionComp) {
        auto [x, y] = calculateSpeed(ecm, hiveSpeeds, movement);
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
    if (!ecm.exists<LeftAlienComponent>() || !ecm.exists<RightAlienComponent>())
        updateHiveBounds(ecm, hiveId);

    auto movement = hiveMovementEffects.peek(&HiveMovementEffect::movement);
    using Movement = decltype(movement);

    switch (movement)
    {
    case Movement::LEFT:
    case Movement::RIGHT:
        return checkIsOutOfBounds(ecm, hiveId, movement);
    default:
        break;
    }

    return false;
}

inline void moveHiveAI(ECM &ecm, EId hiveId, auto &hiveMovementEffects)
{
    auto movement = hiveMovementEffects.peek(&HiveMovementEffect::movement);
    auto [movementComps] = ecm.get<MovementComponent>(hiveId);
    auto &speeds = movementComps.peek(&MovementComponent::speeds);

    auto &allHiveAiIds = ecm.getEntityIds<HiveAIComponent>();
    if (!allHiveAiIds.size())
        return;

    auto newSpeed = calculateSpeed(ecm, speeds, movement);
    if (!newSpeed.x && !newSpeed.y)
        return;

    for (const auto &eId : ecm.getEntityIds<HiveAIComponent>())
        ecm.add<MovementEvent>(eId, std::move(newSpeed));
}

inline void updateHiveMovement(ECM &ecm, EId hiveId, auto &hiveMovementEffects)
{
    hiveMovementEffects.mutate([&](HiveMovementEffect &hiveMovementEffect) {
        auto &allHiveAiIds = ecm.getEntityIds<HiveAIComponent>();
        auto hiveAICount = allHiveAiIds.size();
        if (!hiveAICount)
            return;

        using Movement = decltype(HiveMovementEffect::movement);
        if (hiveMovementEffect.movement == Movement::DOWN)
            hiveMovementEffect.movement = hiveMovementEffect.nextMove;

        float hiveTotal = 55.0f;
        float diff = hiveTotal - hiveAICount;
        diff = diff > 0 ? diff : 1.0f;
        float interval = 0.5f / (diff / 2);

        hiveMovementEffect.timer->update(interval);
    });
}

inline bool checkShouldHiveAIMove(Components<HiveMovementEffect> &hiveMovementEffects)
{
    return !!hiveMovementEffects.find(
        [&](const HiveMovementEffect &hiveMovementEffect) { return hiveMovementEffect.timer->hasElapsed(); });
}

inline void updateHive(ECM &ecm)
{
    auto [hiveMovementSet] = ecm.getAll<HiveMovementEffect>();
    hiveMovementSet.each([&](EId hiveId, auto &hiveMovementEffects) {
        if (checkHiveOutOfBounds(ecm, hiveId, hiveMovementEffects))
            handleHiveShift(ecm, hiveMovementEffects);

        if (checkShouldHiveAIMove(hiveMovementEffects))
        {
            moveHiveAI(ecm, hiveId, hiveMovementEffects);
            updateHiveMovement(ecm, hiveId, hiveMovementEffects);
        }
    });
}

inline bool containsId(const auto &vec, EntityId id)
{
    for (const auto &vecId : vec)
        if (vecId == id)
            return true;

    return false;
}

inline void handleHiveAttack(ECM &ecm)
{
    auto [hiveId, hiveComps] = ecm.getUnique<HiveComponent>();
    if (!hiveId)
        return;

    auto [aiTimeoutEffects] = ecm.get<AITimeoutEffect>(hiveId);
    if (aiTimeoutEffects)
    {
        auto elapsedEffect = aiTimeoutEffects.find(
            [&](const AITimeoutEffect &AITimeoutEffect) { return AITimeoutEffect.timer->hasElapsed(); });

        if (!elapsedEffect)
            return;

        ecm.remove<AITimeoutEffect>(hiveId);
    }

    auto &attackingAIs = ecm.getEntityIds<HiveAIComponent, AttackEffect>();
    if (attackingAIs.size() >= 3)
        return;

    auto hiveAiIds = ecm.getEntityIds<HiveAIComponent>();
    if (hiveAiIds.empty())
        return;

    for (auto iter = hiveAiIds.begin(); iter != hiveAiIds.end();)
    {
        auto id = hiveAiIds[*iter];
        if (containsId(attackingAIs, id))
            iter = hiveAiIds.erase(iter);
        else
            ++iter;
    }

    float randomIndex = std::rand() % hiveAiIds.size();
    ecm.add<AttackEvent>(hiveAiIds[randomIndex], 0);

    float randomDelay = std::rand() % 10;
    ecm.add<AITimeoutEffect>(hiveId, randomDelay);
}

inline void updateUFO(ECM &ecm)
{
    auto [gameId, gameMetaComps] = ecm.getUnique<GameMetaComponent>();
    auto [ufoTimeoutEffect] = ecm.get<UFOTimeoutEffect>(gameId);
    if (ufoTimeoutEffect)
        return;

    auto [ufoAISet] = ecm.getAll<UFOAIComponent>();
    if (ufoAISet)
        return;

    const float &tileSize = gameMetaComps.peek(&GameMetaComponent::tileSize);
    createUfo(ecm, 10.0f, tileSize / 2);
    ecm.add<UFOTimeoutEffect>(gameId, 15);
}

inline void handleUFOAttack(ECM &ecm)
{
    auto [ufoAISet] = ecm.getAll<UFOAIComponent>();
    ufoAISet.each([&](EId eId, auto &ufoAiComps) {
        if (ecm.contains<AITimeoutEffect>(eId))
            return;

        float randomDelay = std::rand() % 5;
        ecm.add<AttackEvent>(eId, 0);
        ecm.add<AITimeoutEffect>(eId, randomDelay);
    });
}

inline auto update(ECM &ecm)
{
    updateHive(ecm);
    updateUFO(ecm);

    handleHiveAttack(ecm);
    handleUFOAttack(ecm);

    return cleanup;
};
}; // namespace Systems::AI
