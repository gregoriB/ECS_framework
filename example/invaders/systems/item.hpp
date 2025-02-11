#pragma once

#include "../components.hpp"
#include "../core.hpp"
#include "../entities.hpp"
#include "../utilities.hpp"
#include <cstdlib>

namespace Systems::Item
{
inline void cleanup(ECM &ecm)
{
    Utilties::cleanupEffect<PowerupTimeoutEffect, PowerupEffect>(ecm);
}

inline void spawnPowerup(ECM &ecm)
{
    auto [gameId, gameMetaComps] = ecm.getUnique<GameMetaComponent>();
    if (ecm.contains<PowerupTimeoutEffect>(gameId))
        return;

    auto [playerId, _] = ecm.getUnique<PlayerComponent>();
    if (ecm.contains<PowerupEffect>(playerId))
        return;

    auto [positionComps] = ecm.get<PositionComponent>(playerId);
    auto &playerPos = positionComps.peek(&PositionComponent::bounds);

    auto &screenSize = gameMetaComps.peek(&GameMetaComponent::screen);
    float tileSize = gameMetaComps.peek(&GameMetaComponent::tileSize);

    float randomX = std::rand() % static_cast<int>(screenSize.x - tileSize);
    createPowerup(ecm, Bounds{randomX + tileSize, playerPos.position.y, tileSize, tileSize});
    ecm.add<PowerupTimeoutEffect>(gameId);
}

inline void processEvents(ECM &ecm)
{
    auto [powerupEventSet] = ecm.getAll<PowerupEvent>();
    powerupEventSet.each([&](EId eId, auto &powerupEvents) {
        powerupEvents.inspect([&](const PowerupEvent &event) { ecm.add<PowerupEffect>(eId); });
    });
}

inline auto update(ECM &ecm)
{
    processEvents(ecm);
    spawnPowerup(ecm);

    return cleanup;
}
}; // namespace Systems::Item
