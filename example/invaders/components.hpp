#pragma once

#include "core.hpp"
#include "renderer.hpp"

using Unique = Tags::Unique;
using Stacked = Tags::Component;
using Event = Tags::Event;
using Effect = Tags::Effect;

enum class Movement
{
    NONE = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum class Action
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

struct HiveComponent : Unique
{
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
    Movement movement = Movement::NONE;
    Action action = Action::NONE;

    PlayerInputEvent(Movement _movement, Action _action) : movement(_movement), action(_action)
    {
    }
    PlayerInputEvent(Movement _movement) : movement(_movement)
    {
    }
    PlayerInputEvent(Action _action) : action(_action)
    {
    }
};

struct AIInputEvent : Event
{
    Movement movement = Movement::NONE;
    Action action = Action::NONE;

    AIInputEvent(Movement _movement, Action _action) : movement(_movement), action(_action)
    {
    }
    AIInputEvent(Movement _movement) : movement(_movement)
    {
    }
    AIInputEvent(Action _action) : action(_action)
    {
    }
};

struct AIMovementEffect : Effect
{
};

struct HiveMovementEffect : Effect
{
    Movement movement;
    float moveInterval{0.5f};

    HiveMovementEffect(Movement _movement) : movement(_movement), Effect(0)
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

struct DeactivatedComponent : Unique
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
    Movement movement;

    ProjectileComponent(Movement _movement) : movement(_movement)
    {
    }
};
