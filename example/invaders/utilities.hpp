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
inline void stageBuilder(ECM &ecm, const std::vector<std::string_view> &stage, ScreenConfig &screen)
{
    int tileSize = screen.width / stage[0].size();
    PRINT(screen.width, screen.height, tileSize, stage[0].size(), stage.size())

    for (int row = 0; row < stage.size(); ++row)
    {
        for (int col = 0; col < stage[0].size(); ++col)
        {
            auto constructor = Stage1::getEntityConstructor(stage[row][col]);
            if (constructor)
                constructor(ecm, col * tileSize, row * tileSize, 1, 1);
        }
    }
};

inline void setup(ECM &ecm, ScreenConfig &screen)
{
    createGame(ecm, screen);
    stageBuilder(ecm, Stage1::stage, screen);
};

inline void updateDeltaTime(ECM &ecm, float delta)
{
    auto [gameId, gameMetaComps] = ecm.getUniqueEntity<GameMetaComponent>();
    gameMetaComps.mutate([&](GameMetaComponent &gameMetaComp) { gameMetaComp.deltaTime = delta; });
};

inline float getDeltaTime(ECM &ecm)
{
    auto [gameId, gameMetaComps] = ecm.getUniqueEntity<GameMetaComponent>();
    return gameMetaComps.peek(&GameMetaComponent::deltaTime);
};

inline void registerPlayerInputs(ECM &ecm, std::vector<Inputs> &inputs)
{
    auto [playerId, _] = ecm.getUniqueEntity<PlayerComponent>();
    for (const auto &input : inputs)
        switch (input)
        {
        case Inputs::SHOOT:
            ecm.add<PlayerInputEvent>(playerId, Action::SHOOT);
            break;
        case Inputs::LEFT:
            ecm.add<PlayerInputEvent>(playerId, Movement::LEFT);
            break;
        case Inputs::RIGHT:
            ecm.add<PlayerInputEvent>(playerId, Movement::RIGHT);
            break;
        case Inputs::QUIT:
            ecm.add<PlayerInputEvent>(playerId, Action::QUIT);
            break;
        case Inputs::UP:
        case Inputs::DOWN:
        case Inputs::MENU:
        default:
            break;
        }
};

inline void registerAIInputs(ECM &ecm, EId eId, std::vector<Inputs> &inputs)
{
    for (const auto &input : inputs)
        switch (input)
        {
        case Inputs::SHOOT:
            ecm.add<AIInputEvent>(eId, Action::SHOOT);
            break;
        case Inputs::LEFT:
            ecm.add<AIInputEvent>(eId, Movement::LEFT);
            break;
        case Inputs::RIGHT:
            ecm.add<AIInputEvent>(eId, Movement::RIGHT);
            break;
        case Inputs::UP:
            ecm.add<AIInputEvent>(eId, Movement::UP);
            break;
        case Inputs::DOWN:
            ecm.add<AIInputEvent>(eId, Movement::DOWN);
            break;
        case Inputs::MENU:
        case Inputs::QUIT:
        default:
            break;
        }
};

inline bool getGameoverState(ECM &ecm)
{
    auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();
    return gameComps.peek(&GameComponent::isGameOver);
};

inline std::vector<Renderer::RenderableElement> getRenderableElements(ECM &ecm)
{
    std::vector<Renderer::RenderableElement> elements{};
    ecm.getAll<SpriteComponent>().each([&](EId eId, auto &spriteComps) {
        auto &rgba = spriteComps.peek(&SpriteComponent::rgba);
        auto [x, y, w, h] = ecm.get<PositionComponent>(eId).peek(&PositionComponent::bounds).box();
        elements.emplace_back(x, y, x + w, y + h, rgba);
    });

    return elements;
};
}; // namespace Utilties
