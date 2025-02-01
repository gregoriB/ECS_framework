#pragma once

#include "components.hpp"
#include "core.hpp"
#include "renderer.hpp"

inline void createGame(ECM &ecm, ScreenConfig &screen)
{
    EntityId gameId = ecm.createEntity();

    PRINT("CREATE GAME", gameId)
    ecm.add<GameMetaComponent>(gameId);
    ecm.add<GameComponent>(gameId,
                           Bounds{0, 0, static_cast<float>(screen.width), static_cast<float>(screen.height)});
    ecm.lockSet<GameMetaComponent>();
    ecm.lockSet<GameComponent>();
}

inline EntityId createHive(ECM &ecm)
{
    EntityId hiveId = ecm.createEntity();
    ecm.add<HiveComponent>(hiveId);

    return hiveId;
}

inline EntityId player(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();

    PRINT("CREATE PLAYER", id)
    PRINT(x, y, w, h)
    ecm.add<PlayerComponent>(id);
    ecm.add<PositionComponent>(id, Bounds{x - (w / 2), y + (h / 2), w + (w / 2), h - (h / 2)});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 255, 0, 1});
    ecm.add<MovementComponent>(id, Vector2{w * 10, w * 10});
    ecm.add<AttackComponent>(id, Movements::UP);

    return id;
};

inline EntityId alien(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();
    auto [hiveId, _] = ecm.getUniqueEntity<HiveComponent>();
    ecm.add<AIComponent>(id);
    ecm.add<HiveAIComponent>(id, hiveId);
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 255, 255, 1});
    ecm.add<MovementComponent>(id, Vector2{w / 2, w});

    return id;
};

inline EntityId leftAlien(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = alien(ecm, x, y, w, h);
    ecm.add<LeftAlienComponent>(id);

    return id;
}

inline EntityId rightAlien(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = alien(ecm, x, y, w, h);
    ecm.add<RightAlienComponent>(id);

    return id;
}

inline EntityId createProjectile(ECM &ecm, Bounds bounds)
{
    EntityId id = ecm.createEntity();
    auto [w, h] = bounds.size;
    ecm.add<MovementComponent>(id, Vector2{0, w * 10});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 255, 255, 1});
    return id;
};

inline EntityId createUpwardProjectile(ECM &ecm, Bounds bounds)
{
    auto [x, y, w, h] = bounds.get();
    float newW = w / 5;
    float newH = h * 2;
    float newX = x + (w / 2) - (newW / 2);
    EntityId id = createProjectile(ecm, bounds);
    ecm.add<MovementEffect>(id, Vector2{newX, -10000});
    ecm.add<PositionComponent>(id, Bounds{newX, y - newH, newW, newH});
    using Movements = decltype(ProjectileComponent::movement);
    ecm.add<ProjectileComponent>(id, Movements::UP);

    return id;
}

inline EntityId createDownwardProjectile(ECM &ecm, Bounds bounds)
{
    auto [x, y, w, h] = bounds.get();
    float newW = w / 5;
    float newH = h * 2;
    float newX = x + (w / 2) - (newW / 2);
    EntityId id = createProjectile(ecm, bounds);
    ecm.add<MovementEffect>(id, Vector2{newX, 10000});
    ecm.add<PositionComponent>(id, Bounds{newX, y - newH, newW, newH});
    using Movements = decltype(ProjectileComponent::movement);
    ecm.add<ProjectileComponent>(id, Movements::DOWN);

    return id;
};
