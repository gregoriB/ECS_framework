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
inline void updateOutsideHiveAliens(ECM &ecm, const HiveComponent &hiveComp)
{
    auto [x, y, w, h] = hiveComp.bounds.box();
    ecm.getAll<HiveAIComponent>().each([&](EId eId, auto &_) {
        auto [aiX, aiY, aiW, aiH] = ecm.get<PositionComponent>(eId).peek(&PositionComponent::bounds).box();
        if (aiX <= x)
            ecm.add<LeftAlienComponent>(eId);
        if (aiW >= w)
            ecm.add<RightAlienComponent>(eId);
    });
}

inline void updateOutsideHiveAliens(ECM &ecm, EId hiveId)
{
    ecm.get<HiveComponent>(hiveId).inspect(
        [&](const HiveComponent &hiveComp) { updateOutsideHiveAliens(ecm, hiveComp); });
}

inline void updateHiveBounds(ECM &ecm, HiveComponent &hiveComp)
{
    PRINT("UPDATING HIVE BOUNDS")
    constexpr int MIN_INT = std::numeric_limits<int>::min();
    constexpr int MAX_INT = std::numeric_limits<int>::max();

    Vector2 topLeft{MAX_INT, MAX_INT};
    Vector2 bottomRight{MIN_INT, MIN_INT};

    ecm.getAll<HiveAIComponent>().each([&](EId eId, auto &_) {
        ecm.get<PositionComponent>(eId).inspect([&](const PositionComponent &posComp) {
            auto [x, y, w, h] = posComp.bounds.box();
            if (x < topLeft.x)
                topLeft.x = x;
            if (y < topLeft.y)
                topLeft.y = y;
            if (w > bottomRight.x)
                bottomRight.x = w;
            if (h > bottomRight.y)
                bottomRight.y = h;
        });
    });

    hiveComp.bounds = Bounds{topLeft, Vector2{bottomRight.x - topLeft.x, bottomRight.y - topLeft.y}};
    updateOutsideHiveAliens(ecm, hiveComp);
}

inline void updateHiveBounds(ECM &ecm, EId hiveId)
{
    ecm.get<HiveComponent>(hiveId).inspect(
        [&](const HiveComponent &hiveComp) { updateOutsideHiveAliens(ecm, hiveComp); });
}

inline void initHiveAI(ECM &ecm)
{
    auto [hiveId, hiveComps] = ecm.getUniqueEntity<HiveComponent>();
    hiveComps.mutate([&](HiveComponent &hiveComp) { updateHiveBounds(ecm, hiveComp); });
    using Movements = decltype(HiveMovementEffect::movement);
    ecm.add<HiveMovementEffect>(hiveId, Movements::RIGHT);
};

inline void stageBuilder(ECM &ecm, const std::vector<std::string_view> &stage, ScreenConfig &screen)
{
    int tileSize = screen.width / stage[0].size();

    for (int row = 0; row < stage.size(); ++row)
    {
        for (int col = 0; col < stage[row].size(); ++col)
        {
            auto constructor = Stage1::getEntityConstructor(stage[row][col]);
            if (!constructor)
                continue;

            constructor(ecm, col * tileSize, row * tileSize, tileSize, tileSize);
        }
    }
};

inline void setup(ECM &ecm, ScreenConfig &screen)
{
    createGame(ecm, screen);
    createHive(ecm);
    stageBuilder(ecm, Stage1::stage, screen);
    initHiveAI(ecm);
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
    auto [gameId, gameComps] = ecm.getUniqueEntity<GameComponent>();
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
