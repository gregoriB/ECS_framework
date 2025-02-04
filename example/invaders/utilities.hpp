#pragma once

#include "components.hpp"
#include "core.hpp"
#include "entities.hpp"
#include "renderer.hpp"
#include "stages.hpp"
#include <functional>
#include <string_view>

namespace Utilties
{
inline void registerTransformations(ECM &ecm)
{
    ecm.registerTransformation<PositionComponent>([&](auto eId, PositionComponent comp) {
        if (ecm.get<ProjectileComponent>(eId))
        {
            comp.bounds.size.x += 100;
        }

        return comp;
    });
}

inline void stageBuilder(ECM &ecm, const std::vector<std::string_view> &stage)
{
    auto [_, gameMetaComps] = ecm.get<GameMetaComponent>();
    auto &screen = gameMetaComps.peek(&GameMetaComponent::screen);
    int tileSize = screen.x / stage[0].size();

    for (int row = 0; row < stage.size(); ++row)
    {
        for (int col = 0; col < stage[row].size(); ++col)
        {
            auto constructor = Stages::getEntityConstructor(stage[row][col]);
            if (!constructor)
                continue;

            constructor(ecm, col * tileSize, row * tileSize, tileSize, tileSize);
        }
    }
};

inline void setup(ECM &ecm, ScreenConfig &screen)
{
    Vector2 size{static_cast<float>(screen.width), static_cast<float>(screen.height)};
    createGame(ecm, size);
    registerTransformations(ecm);
    stageBuilder(ecm, Stages::getStage(1));
};

inline void nextStage(ECM &ecm, int stage)
{
    ecm.clear<HiveMovementEffect>();
    stageBuilder(ecm, Stages::getStage(stage));
};

inline void updateDeltaTime(ECM &ecm, float delta)
{
    auto [gameId, gameMetaComps] = ecm.get<GameMetaComponent>();
    gameMetaComps.mutate([&](GameMetaComponent &gameMetaComp) { gameMetaComp.deltaTime = delta; });
};

inline float getDeltaTime(ECM &ecm)
{
    auto [gameId, gameMetaComps] = ecm.get<GameMetaComponent>();
    return gameMetaComps.peek(&GameMetaComponent::deltaTime);
};

inline void registerPlayerInputs(ECM &ecm, std::vector<Inputs> &inputs)
{
    auto [playerId, _] = ecm.get<PlayerComponent>();
    using Movements = decltype(PlayerInputEvent::movement);
    using Actions = decltype(PlayerInputEvent::action);
    for (const auto &input : inputs)
    {
        switch (input)
        {
        case Inputs::SHOOT:
            ecm.add<PlayerInputEvent>(playerId, Actions::SHOOT);
            break;
        case Inputs::LEFT:
            ecm.add<PlayerInputEvent>(playerId, Movements::LEFT);
            break;
        case Inputs::RIGHT:
            ecm.add<PlayerInputEvent>(playerId, Movements::RIGHT);
            break;
        case Inputs::QUIT:
            ecm.add<PlayerInputEvent>(playerId, Actions::QUIT);
            break;
        case Inputs::UP:
        case Inputs::DOWN:
        case Inputs::MENU:
        default:
            break;
        }
    }
};

inline void registerAIInputs(ECM &ecm, EId eId, std::vector<Inputs> &inputs)
{
    using Movements = decltype(AIInputEvent::movement);
    using Actions = decltype(AIInputEvent::action);
    for (const auto &input : inputs)
        switch (input)
        {
        case Inputs::SHOOT:
            ecm.add<AIInputEvent>(eId, Actions::SHOOT);
            break;
        case Inputs::LEFT:
            ecm.add<AIInputEvent>(eId, Movements::LEFT);
            break;
        case Inputs::RIGHT:
            ecm.add<AIInputEvent>(eId, Movements::RIGHT);
            break;
        case Inputs::UP:
            ecm.add<AIInputEvent>(eId, Movements::UP);
            break;
        case Inputs::DOWN:
            ecm.add<AIInputEvent>(eId, Movements::DOWN);
            break;
        case Inputs::MENU:
        case Inputs::QUIT:
        default:
            break;
        }
};

inline bool getGameoverState(ECM &ecm)
{
    auto [gameId, gameComps] = ecm.get<GameComponent>();
    return gameComps.peek(&GameComponent::isGameOver);
};

inline std::vector<Renderer::RenderableElement> getRenderableElements(ECM &ecm)
{
    std::vector<Renderer::RenderableElement> elements{};
    ecm.getAll<SpriteComponent>().each([&](EId eId, auto &spriteComps) {
        auto &rgba = spriteComps.peek(&SpriteComponent::rgba);
        auto [x, y, w, h] = ecm.get<PositionComponent>(eId).peek(&PositionComponent::bounds).get();
        elements.emplace_back(x, y, w, h, rgba);
    });

    return elements;
};
}; // namespace Utilties
