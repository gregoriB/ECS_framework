#pragma once

#include "components.hpp"
#include "core.hpp"
#include "renderer.hpp"

inline void createGame(ECM &ecm, Vector2 &size, int tileSize)
{
    EntityId gameId = ecm.createEntity();

    PRINT("CREATE GAME", gameId)
    ecm.add<GameMetaComponent>(gameId, size, tileSize);
    ecm.add<GameComponent>(gameId, Bounds{0, 0, size.x, size.y});
    ecm.add<UFOTimeoutEffect>(gameId, 12);
    ecm.add<PowerupTimeoutEffect>(gameId);
}

inline EntityId hive(ECM &ecm, float x, float y, float w, float h)
{
    EntityId hiveId = ecm.createEntity();
    PRINT("CREATE HIVE", hiveId)
    ecm.clear<HiveComponent, HiveMovementEffect>();
    auto [_, gameMetaComps] = ecm.getUnique<GameMetaComponent>();
    auto &size = gameMetaComps.peek(&GameMetaComponent::screen);
    ecm.add<HiveComponent>(hiveId);
    ecm.add<HiveMovementEffect>(hiveId, Movements::RIGHT);
    ecm.add<MovementComponent>(hiveId, Vector2{size.x / 200, size.y / 50});
    ecm.add<AttackEffect>(hiveId, 0, 3);

    return hiveId;
}

inline EntityId player(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();

    PRINT("CREATE PLAYER", id)
    ecm.add<CollidableComponent>(id);
    ecm.add<PlayerComponent>(id);
    ecm.add<PositionComponent>(id, Bounds{x - (w / 4), y + (h / 2), w * 1.5f, h - (h / 2)});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 255, 0, 255});
    ecm.add<MovementComponent>(id, Vector2{w * 10, w * 10});
    ecm.add<AttackComponent>(id, Movements::UP);
    ecm.add<HealthComponent>(id, 10);
    ecm.add<DamageComponent>(id, 25.0f);
    ecm.add<ScoreComponent>(id, 0);
    ecm.add<LivesComponent>(id, 3);

    return id;
};

inline EntityId playerScore(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();

    PRINT("CREATE PLAYER SCORE", id)
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 0, 0, 0});
    ecm.add<UIComponent>(id);
    ecm.add<TextComponent>(id, "SCORE: 0");
    ecm.add<PlayerScoreCardComponent>(id);

    return id;
};

inline EntityId playerLives(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();

    PRINT("CREATE PLAYER LIVES", id)
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 0, 0, 0});
    ecm.add<UIComponent>(id);
    ecm.add<TextComponent>(id, "LIVES: 3");
    ecm.add<PlayerLifeCardComponent>(id);

    return id;
};

inline EntityId createUfo(ECM &ecm, float x, float y)
{
    EntityId id = ecm.createEntity();
    PRINT("UFO SPAWNED", id)
    ecm.add<UFOAIComponent>(id);
    auto [_, gameMetaComps] = ecm.getUnique<GameMetaComponent>();
    auto &size = gameMetaComps.peek(&GameMetaComponent::screen);
    const float &tileSize = gameMetaComps.peek(&GameMetaComponent::tileSize);
    float diff = 15;
    float newW = tileSize + diff;
    float newX = x - newW;
    ecm.add<CollidableComponent>(id);
    ecm.add<PositionComponent>(id, Bounds{newX, y, newW, tileSize});
    ecm.add<AttackComponent>(id, Movements::DOWN);
    ecm.add<HealthComponent>(id, 10);
    ecm.add<DamageComponent>(id, 100);
    ecm.add<PointsComponent>(id, 150);
    ecm.add<MovementComponent>(id, Vector2{tileSize * 4, tileSize * 4});
    ecm.add<MovementEffect>(id, Vector2{tileSize * size.x, tileSize / 2});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 0, 0, 255});
    float randomDelay = std::rand() % 5;
    ecm.add<AttackEffect>(id, randomDelay);

    return id;
};

inline EntityId hiveAlien(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();
    auto [hiveId, _] = ecm.getUnique<HiveComponent>();
    float diff = 7;
    ecm.add<CollidableComponent>(id);
    ecm.add<AIComponent>(id);
    ecm.add<HiveAIComponent>(id, hiveId);
    ecm.add<PositionComponent>(id, Bounds{x - diff, y, w + diff, h});
    ecm.add<MovementComponent>(id, Vector2{w / 2, w});
    ecm.add<AttackComponent>(id, Movements::DOWN);
    ecm.add<HealthComponent>(id, 10);
    ecm.add<DamageComponent>(id, 25.0f);

    return id;
};

inline EntityId hiveAlienSmall(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = hiveAlien(ecm, x, y, w, h);
    ecm.add<PointsComponent>(id, 10);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{205, 205, 205, 255});

    return id;
}

inline EntityId hiveAlienMedium(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = hiveAlien(ecm, x, y, w, h);
    ecm.add<PointsComponent>(id, 20);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{230, 230, 230, 255});

    return id;
}

inline EntityId hiveAlienLarge(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = hiveAlien(ecm, x, y, w, h);
    ecm.add<PointsComponent>(id, 40);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 255, 255, 255});

    return id;
}

inline EntityId collidableObstacleBlock(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();
    ecm.add<ObstacleComponent>(id);
    ecm.add<CollidableComponent>(id);
    ecm.add<DamageComponent>(id, 1);

    return id;
}

inline EntityId titleBlockSm(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = collidableObstacleBlock(ecm, x, y, w, h);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 255, 0, 255});
    ecm.add<PositionComponent>(id, Bounds{x, y, w - 5, h});
    ecm.add<HealthComponent>(id, 50);
    ecm.add<TitleScreenComponent>(id);

    return id;
}

inline EntityId titleBlock(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = collidableObstacleBlock(ecm, x, y, w, h);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 255, 0, 255});
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<HealthComponent>(id, 50);
    ecm.add<TitleScreenComponent>(id);

    return id;
}

inline EntityId redBlock(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = collidableObstacleBlock(ecm, x, y, w, h);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 0, 0, 255});
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<HealthComponent>(id, 1);
    ecm.add<PointsComponent>(id, 1);

    return id;
}

inline EntityId startBlock(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = collidableObstacleBlock(ecm, x, y, w, h);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{67, 189, 255, 255});
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<HealthComponent>(id, 1);
    ecm.add<StartGameTriggerComponent>(id);
    ecm.add<TitleScreenComponent>(id);

    return id;
}

inline EntityId greenBlock(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = collidableObstacleBlock(ecm, x, y, w, h);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 255, 0, 255});
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<HealthComponent>(id, 100);

    return id;
}

inline EntityId createProjectile(ECM &ecm, Bounds bounds)
{
    EntityId id = ecm.createEntity();
    auto [w, h] = bounds.size;
    ecm.add<CollidableComponent>(id);
    ecm.add<MovementComponent>(id, Vector2{0, w * 10});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 255, 255, 255});
    ecm.add<HealthComponent>(id, 1);
    return id;
};

inline EntityId createUpwardProjectile(ECM &ecm, EntityId shooterId, Bounds bounds)
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
    ecm.add<ProjectileComponent>(id, shooterId, Movements::UP);

    return id;
}

inline EntityId createDownwardProjectile(ECM &ecm, EntityId shooterId, Bounds bounds)
{
    auto [x, y, w, h] = bounds.get();
    float newW = w / 5;
    float newH = h;
    float newY = y + newH;
    float newX = x + (w / 2) - (newW / 2);
    EntityId id = createProjectile(ecm, bounds);
    ecm.add<MovementEffect>(id, Vector2{newX, 10000});
    ecm.add<PositionComponent>(id, Bounds{newX, newY + 1, newW, newH});
    using Movements = decltype(ProjectileComponent::movement);
    ecm.add<ProjectileComponent>(id, shooterId, Movements::DOWN);
    ecm.add<PointsComponent>(id, 10);

    return id;
}

inline EntityId createPowerup(ECM &ecm, Bounds bounds)
{
    EntityId id = ecm.createEntity();
    PRINT("POWERUP SPAWNED", id)
    ecm.add<CollidableComponent>(id);
    ecm.add<HealthComponent>(id, 1);
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 255, 0, 255});
    ecm.add<PositionComponent>(id, bounds);
    ecm.add<PowerupComponent>(id);

    return id;
}
