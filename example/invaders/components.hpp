#pragma once

#include "core.hpp"
#include "renderer.hpp"

using Unique = Tags::Unique;
using Stacked = Tags::Component;
using Event = Tags::Event;
using Effect = Tags::Effect;

enum class Movements
{
    NONE = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum class Actions
{
    NONE = 0,
    SHOOT,
    QUIT,
};

struct PlayerComponent : Unique
{
};

struct AIComponent : Unique
{
};

struct LeftAlienComponent : Unique
{
};
struct RightAlienComponent : Unique
{
};

struct HiveComponent : Unique
{
    Bounds bounds{};
};

struct HiveAIComponent : Unique
{
    EntityId hiveId;

    HiveAIComponent(EntityId _hiveId) : hiveId(_hiveId)
    {
    }
};

struct PlayerInputEvent : Event
{
    Movements movement = Movements::NONE;
    Actions action = Actions::NONE;

    PlayerInputEvent(Movements _movement, Actions _action) : movement(_movement), action(_action)
    {
    }
    PlayerInputEvent(Movements _movement) : movement(_movement)
    {
    }
    PlayerInputEvent(Actions _action) : action(_action)
    {
    }
};

struct AIInputEvent : Event
{
    Movements movement = Movements::NONE;
    Actions action = Actions::NONE;

    AIInputEvent(Movements _movement, Actions _action) : movement(_movement), action(_action)
    {
    }
    AIInputEvent(Movements _movement) : movement(_movement)
    {
    }
    AIInputEvent(Actions _action) : action(_action)
    {
    }
};

struct AIMovementEffect : Effect
{
};

struct HiveMovementEffect : Effect
{
    float moveInterval{0.5f};
    Movements movement;
    Movements nextMove;

    HiveMovementEffect(Movements _movement) : movement(_movement), Effect(0.5f)
    {
    }
};

struct MovementEffect : Effect
{
    Vector2 trajectory;

    MovementEffect(Vector2 _trajectory) : trajectory(_trajectory)
    {
    }
};

struct MovementComponent : Unique
{
    Vector2 speeds;

    MovementComponent()
    {
    }
    MovementComponent(Vector2 _speeds) : speeds(_speeds)
    {
    }
};

struct MovementEvent : Event
{
    Vector2 coords;

    MovementEvent()
    {
    }
    MovementEvent(Vector2 _coords) : coords(_coords)
    {
    }
};

struct PositionComponent : Unique
{
    Bounds bounds;

    PositionComponent(Bounds _bounds) : bounds(_bounds)
    {
    }
};

struct PositionEvent : Event
{
    Vector2 coords;

    PositionEvent(Vector2 _coords) : coords(_coords)
    {
    }
};

struct CollisionCheckEvent : Event
{
    Bounds bounds;

    CollisionCheckEvent(Bounds _bounds) : bounds(_bounds)
    {
    }
};

struct DeathEvent : Event
{
    EntityId dealerId;

    DeathEvent(EntityId _dealerId) : dealerId(_dealerId)
    {
    }
};

struct DeathComponent : Unique
{
};

struct DamageComponent : Unique
{
    float amount;

    DamageComponent(float _amount) : amount(_amount)
    {
    }
};

struct AttackComponent : Unique
{
    Movements direction;

    AttackComponent(Movements _direction) : direction(_direction)
    {
    }
};

struct AttackEvent : Event
{
};

struct AttackEffect : Effect
{
    EntityId attackId;

    AttackEffect(EntityId _attackId) : attackId(_attackId)
    {
    }
};

struct GameComponent : Unique
{
    Bounds bounds;
    bool isGameOver{};

    GameComponent(Bounds _bounds) : bounds(_bounds)
    {
    }
};

struct GameMetaComponent : Unique
{
    float deltaTime{};
};

enum class GameEvents
{
    GAME_OVER,
    QUIT,
};

struct GameEvent : Event
{
    GameEvents event;
    GameEvent(GameEvents _event) : event(_event)
    {
    }
};

struct SpriteComponent : Unique
{
    Renderer::RGBA rgba;

    SpriteComponent(Renderer::RGBA _rgba) : rgba(_rgba)
    {
    }
};

struct ProjectileComponent : Unique
{
    Movements movement;

    ProjectileComponent(Movements _movement) : movement(_movement)
    {
    }
};

struct PointsComponent : Unique
{
    int points;
    int multiplier{1};

    PointsComponent(int _points) : points(_points)
    {
    }
    PointsComponent(int _points, int _multiplier) : points(_points), multiplier(_multiplier)
    {
    }
};
