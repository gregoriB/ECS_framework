#pragma once

#include "components.hpp"
#include "core.hpp"
#include "renderer.hpp"

inline void createGame(ECM &ecm, Vector2 &size)
{
    EntityId gameId = ecm.createEntity();

    PRINT("CREATE GAME", gameId)
    ecm.addUnique<GameMetaComponent>(gameId, size);
    ecm.addUnique<GameComponent>(gameId, Bounds{0, 0, size.x, size.y});
}

inline EntityId hive(ECM &ecm, float x, float y, float w, float h)
{
    EntityId hiveId = ecm.createEntity();
    PRINT("CREATE HIVE", hiveId)
    ecm.clear<HiveComponent>();
    ecm.clear<HiveMovementEffect>();
    auto [_, gameMetaComps] = ecm.getUnique<GameMetaComponent>();
    auto &size = gameMetaComps.peek(&GameMetaComponent::screen);
    ecm.addUnique<HiveComponent>(hiveId);
    ecm.addUnique<HiveMovementEffect>(hiveId, Movements::RIGHT);
    ecm.add<MovementComponent>(hiveId, Vector2{size.x / 200, size.y / 50});

    return hiveId;
}

inline EntityId player(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();

    PRINT("CREATE PLAYER", id)
    ecm.addUnique<PlayerComponent>(id);
    ecm.add<PositionComponent>(id, Bounds{x - (w / 2), y + (h / 2), w + (w / 2), h - (h / 2)});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 255, 0, 1});
    ecm.add<MovementComponent>(id, Vector2{w * 10, w * 10});
    ecm.add<AttackComponent>(id, Movements::UP);

    return id;
};

inline EntityId hiveAlien(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();
    auto [hiveId, _] = ecm.getUnique<HiveComponent>();
    float diff = 7;
    ecm.add<AIComponent>(id);
    ecm.add<HiveAIComponent>(id, hiveId);
    ecm.add<PositionComponent>(id, Bounds{x - diff, y, w + diff, h});
    ecm.add<MovementComponent>(id, Vector2{w / 2, w});

    return id;
};

inline EntityId hiveAlienSmall(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = hiveAlien(ecm, x, y, w, h);
    ecm.add<PointsComponent>(id, 10);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{205, 205, 205, 1});

    return id;
}

inline EntityId hiveAlienMedium(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = hiveAlien(ecm, x, y, w, h);
    ecm.add<PointsComponent>(id, 20);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{230, 230, 230, 1});

    return id;
}

inline EntityId hiveAlienLarge(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = hiveAlien(ecm, x, y, w, h);
    ecm.add<PointsComponent>(id, 40);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 255, 255, 1});

    return id;
}

inline EntityId redBlock(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 0, 0, 1});
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});

    return id;
}

inline EntityId greenBlock(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 255, 0, 1});
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});

    return id;
}

inline EntityId createProjectile(ECM &ecm, Bounds bounds)
{
    EntityId id = ecm.createEntity();
    auto [w, h] = bounds.size;
    ecm.add<MovementComponent>(id, Vector2{0, w * 13});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 255, 255, 1});
    return id;
};

inline EntityId createUpwardProjectile(ECM &ecm, Bounds bounds)
{
    auto [x, y, w, h] = bounds.get();
    float newW = w / 5;
    float newH = h * 2;
    float newY = y - newH - 1;
    float newX = x + (w / 2) - (newW / 2);
    EntityId id = createProjectile(ecm, bounds);
    ecm.add<MovementEffect>(id, Vector2{newX, -10000});
    ecm.add<PositionComponent>(id, Bounds{newX, newY, newW, newH});
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
