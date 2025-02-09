#pragma once

#include "core.hpp"
#include "renderer.hpp"
#include <cstdint>

using NoStack = Tags::NoStack;
using Stack = Tags::Stack;
using Event = Tags::Event;
using Effect = Tags::Effect;
using Required = Tags::Required;
using Unique = Tags::Unique;
using Transform = Tags::Transform;

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

enum class PlayerEvents
{
    NONE = 0,
    NEXT_STAGE,
    DEATH,
};

struct PlayerEvent : Event
{
    PlayerEvents event;

    PlayerEvent(PlayerEvents _event) : event(_event)
    {
    }
};

struct PlayerComponent : Unique
{
};

struct ScoreComponent
{
    int score{};
};

struct LivesComponent
{
    int count{};

    LivesComponent(int _count) : count(_count)
    {
    }
};

struct PlayerScoreCardComponent : Unique
{
};

struct PlayerLifeCardComponent : Unique
{
};

struct ScoreEvent : Event
{
    EntityId pointsId;

    ScoreEvent(EntityId _pointsId) : pointsId(_pointsId)
    {
    }
};

struct AIComponent
{
};

struct LeftAlienComponent : Stack
{
};

struct RightAlienComponent : Stack
{
};

struct HiveComponent : Unique
{
    Bounds bounds{};
};

struct UFOAIComponent
{
};

struct HiveAIComponent
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

struct HiveMovementEffect : Effect, NoStack
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

struct MovementComponent : Transform
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

struct CollidableComponent
{
};

struct PositionComponent
{
    Bounds bounds;

    PositionComponent(Bounds _bounds) : bounds(_bounds)
    {
    }
};

struct PositionEvent : Event, NoStack
{
    Vector2 coords;

    PositionEvent(Vector2 _coords) : coords(_coords)
    {
    }
};

struct HealthEvent : Event
{
    EntityId dealerId;
    int32_t amount{};

    HealthEvent(int32_t _amount, EntityId _dealerId = 0) : amount(_amount), dealerId(_dealerId)
    {
    }
};

struct HealthComponent
{
    int32_t total{};
    int32_t current{};

    HealthComponent(int32_t _total) : total(_total), current(_total)
    {
    }
};

struct CollisionCheckEvent : Event, NoStack
{
    Bounds bounds;

    CollisionCheckEvent(Bounds _bounds) : bounds(_bounds)
    {
    }
};

struct DamageEvent : Event
{
    EntityId dealerId;

    DamageEvent(EntityId _dealerId) : dealerId(_dealerId)
    {
    }
};

struct DeathEvent : Event
{
    EntityId killedBy;

    DeathEvent(EntityId _killedBy = 0) : killedBy(_killedBy)
    {
    }
};

struct DeathComponent
{
};

struct DeactivatedComponent
{
};

struct DamageComponent
{
    float amount;

    DamageComponent(float _amount) : amount(_amount)
    {
    }
};

struct AttackComponent
{
    Movements direction;

    AttackComponent(Movements _direction) : direction(_direction)
    {
    }
};

struct AttackEvent : Event
{
    float timeout;

    AttackEvent(float _timeout = 0) : timeout(_timeout)
    {
    }
};

struct AttackEffect : Effect, Stack
{
    EntityId attackId;

    AttackEffect(EntityId _attackId, float _timeout = 0) : attackId(_attackId), Effect(_timeout)
    {
    }
};

struct AITimeoutEffect : Effect, Stack
{
    AITimeoutEffect(float _duration) : Effect(_duration)
    {
    }
};

struct UFOTimeoutEffect : Effect, Stack
{
    UFOTimeoutEffect(float _duration) : Effect(_duration)
    {
    }
};

struct StartGameTriggerComponent : Required, Unique
{
};

struct TitleScreenComponent
{
};

struct GameComponent : Required, Unique
{
    Bounds bounds;
    bool isGameOver{};
    int currentStage{1};

    GameComponent(Bounds _bounds) : bounds(_bounds)
    {
    }
};

struct GameMetaComponent : Required, Unique
{
    Vector2 screen;
    int tileSize{};
    float deltaTime{};

    GameMetaComponent(Vector2 _screen, int _tileSize) : screen(_screen), tileSize(_tileSize)
    {
    }
};

enum class GameEvents
{
    NONE = 0,
    NEXT_STAGE,
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

struct SpriteComponent
{
    Renderer::RGBA rgba;

    SpriteComponent(Renderer::RGBA _rgba) : rgba(_rgba)
    {
    }
};

struct UIComponent
{
};

enum class UIEvents
{
    NONE = 0,
    UPDATE_SCORE,
    UPDATE_LIVES,
};

struct UIEvent : Event
{
    UIEvents event;

    UIEvent(UIEvents _event) : event(_event)
    {
    }
};

struct TextComponent
{
    std::string text{};
    Renderer::RGBA color;

    TextComponent(std::string _text, Renderer::RGBA _color = Renderer::RGBA{255, 255, 255, 255})
        : text(_text), color(_color)
    {
    }
};

struct ObstacleComponent
{
};

struct ProjectileComponent
{
    EntityId shooterId;
    Movements movement;

    ProjectileComponent(EntityId _shooterId, Movements _movement) : shooterId(_shooterId), movement(_movement)
    {
    }
};

struct PointsComponent
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

struct PowerupEvent : Event
{
};

struct PowerupComponent
{
};

struct PowerupEffect : Effect
{
    PowerupEffect() : Effect(10)
    {
    }
};

struct PowerupTimeoutEffect : Effect
{
    PowerupTimeoutEffect() : Effect(30)
    {
    }
};
