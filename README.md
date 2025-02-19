# ECS Library (pending rename)
## An opinionated header-only ECS library

## Features
- Develop with guard rails for faster and easier game development
- Utilize component tags to:
    * Enforce storage options and/or limitations
    * Easily group components
    * Enable extended component functionality
- Many convience methods for components, eg:
    * .filter(), .sort(), .remove(), .find() ...and more! [See the docs for more information][docs_components_wrapper_url]
- Data-oriented development approach for better CPU cache performance
- Automatic Component Transformation pipelines

## Examples Games
- [Block Invaders][game_url] - Space Invaders clone/approximation

## Documentation
- [Auto-generated with Doxygen][docs_url]

## Example Usage
Creating entities:
```cpp
using EntityId = int;
using ComponentManager = ECS::Manager<EntityId>;

struct PlayerComponent : ECS::Tags::Unique {};
struct PositionComponent : ECS::Tags::NoStack {
    float x, float y;
    PositionComponent(float _x, float _y) : x(_x), y(_y) {};
};
struct MovementComponent : ECS::Tags::NoStack {
    float vX, float vY;
    MovementComponent(float _vX, float _vY) : vX(_vX), vY(_vY) {};
};

void createPlayer(ComponentManager &cm, float x, float y) {
    EntityId id = cm.createEntity();
    cm.add<PlayerComponent>(id);
    cm.add<PositionComponent>(id, x, y);
    cm.add<MovementComponent>(id, 16.0f, 16.0f);
}

void createEnemy(ComponentManager &cm, float x, float y) {
    EntityId id = cm.createEntity();
    cm.add<PositionComponent>(id, x, y);
    cm.add<MovementComponent>(id, 16.0f, 16.0f);
}
```
Some system performing updates on entities
```cpp
using EntityId = int;
using ComponentManager = ECS::Manager<EntityId>;

void update(ComponentManager &cm)
{
    auto group = cm.getGroup<MovementComponent, PositionComponent>();

    // Basic example for how to access and mutate components.
    // In this case, each entity can only have a single instance of each components
    // However, in the case where these components were given the "Stack" tag, thes
    // .inspect and .mutate methods would iterate over every instance and perform the operation

    group.each([&](EntityId eId, auto MovementComponent &moveComps, auto PositionComponent &posComps)
        moveComps.inspect([&](const MovementComponent &moveComp) {
            posComps.mutate([&](PositionComponent &posComp) {
                posComp.x += moveComp.vX;
                posComp.y += moveComp.vY;
            });
        });
    });
    
    // Alternatively since the movement component is not stacked, 
    // the peek method can be used to access the values
    group.each([&](EntityId eId, auto MovementComponent &moveComps, auto PositionComponent &posComps)
        auto [vX, vY] = moveComps.peek(&MovementComponent::vX, &MovementComponent::vY);
        posComps.mutate([&](PositionComponent &posComp) {
            posComp.x += moveComp.vX;
            posComp.y += moveComp.vY;
        });
    });
    
    // Perhaps some special operation needs to happen for the unique player
    auto [playerId, playerComponent] = cm.getUnique<PlayerComponent>();
    group.each([&](EntityId eId, auto MovementComponent &moveComps, auto PositionComponent &posComps)
        // Do movement update stuff from before
        if (eId == playerId)
            // Do something special, possibly using the movement/position components
    });
}
```

## Installation
Run the build script to run the automated tests:
```sh
$ bash build_and_test.sh
```
Tested on Linux. Cannot make any guarantees about anything Windows or Mac related at this time.

## Dependencies
- [CMAKE][cmake_url]

## Development
At this time, I am open to suggestions but am not approving any pull requests.  But if anyone wants to use the library to develop example games, I'll be happy to add them to the example game list if they seem reasonable.

Regarding library development, once I reach a point where the library has the features I've wanted to implement and the user experience has been solidified, I will open it for contributions if there is any interest in that.

## License

MIT

[//]: # ()

   [docs_url]: <https://ecslibarydocs.netlify.app>
   [docs_components_wrapper_url]: <https://ecslibarydocs.netlify.app/classecs_1_1internal_1_1componentswrapper>
   [game_url]: <https://github.com/gregoriB/block_invaders-ecs_library_example_game>
   [cmake_url]: <https://cmake.org>
