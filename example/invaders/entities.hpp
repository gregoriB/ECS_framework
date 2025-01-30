#pragma once

#include "components.hpp"
#include "core.hpp"
#include "renderer.hpp"

inline void createGame(ECM &ecm, ScreenConfig &screen)
{
    EntityId gameId = ecm.createEntity();
    ecm.add<GameMetaComponent>(gameId);
    ecm.add<GameComponent>(gameId,
                           Bounds{0, 0, static_cast<float>(screen.width), static_cast<float>(screen.height)});
    ecm.lockSet<GameMetaComponent>();
    ecm.lockSet<GameComponent>();
}

inline void player(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();

    ecm.add<PlayerComponent>(id);
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 0, 255, 1});
    ecm.add<MovementComponent>(id, Vector2{1, 1});
};

inline void alien(ECM &ecm, float x, float y, float w, float h)
{
    EntityId id = ecm.createEntity();

    ecm.add<AIComponent>(id);
    ecm.add<PositionComponent>(id, Bounds{x, y, w, h});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{255, 0, 0, 1});
    ecm.add<MovementComponent>(id, Vector2{1, 1});
};

inline void createUpwardProjectile(ECM &ecm, Bounds bounds)
{
    EntityId id = ecm.createEntity();
    auto [x, y, w, h] = bounds.get();
    float newW = w / 5;
    ecm.add<PositionComponent>(id, Bounds{x + newW, y, x + w - newW, h});
    ecm.add<MovementEffect>(id, Vector2{-x + newW, -10000});
    ecm.add<MovementComponent>(id, Vector2{1, 1});
};

inline void createDownwardProjectile(ECM &ecm, Bounds bounds)
{
    EntityId id = ecm.createEntity();
    auto [x, y, w, h] = bounds.get();
    float newW = w / 5;
    ecm.add<PositionComponent>(id, Bounds{x + newW, y, x + w - newW, h});
    ecm.add<MovementEffect>(id, Vector2{-x + newW, 10000});
    ecm.add<MovementComponent>(id, Vector2{1, 1});
    ecm.add<SpriteComponent>(id, Renderer::RGBA{0, 255, 0, 1});
};
