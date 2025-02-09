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
    auto [gameId, gameMetaComps] = ecm.get<GameMetaComponent>();
    if (ecm.get<PowerupTimeoutEffect>(gameId))
        return;

    auto [playerId, _] = ecm.get<PlayerComponent>();
    if (ecm.get<PowerupEffect>(playerId))
        return;

    auto &playerPos = ecm.get<PositionComponent>(playerId).peek(&PositionComponent::bounds);

    auto &screenSize = gameMetaComps.peek(&GameMetaComponent::screen);
    float tileSize = gameMetaComps.peek(&GameMetaComponent::tileSize);

    float randomX = std::rand() % static_cast<int>(screenSize.x - tileSize);
    createPowerup(ecm, Bounds{randomX + tileSize, playerPos.position.y, tileSize, tileSize});
    ecm.add<PowerupTimeoutEffect>(gameId);
}

inline void processEvents(ECM &ecm)
{
    ecm.getAll<PowerupEvent>().each([&](EId eId, auto &powerupEvents) {
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
