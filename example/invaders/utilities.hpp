#pragma once

#include "components.hpp"
#include "core.hpp"
#include "entities.hpp"
#include "renderer.hpp"
#include "stages.hpp"
#include "ui.hpp"
#include <functional>
#include <string_view>
#include <tuple>

namespace Utilties
{
inline auto &first(auto value)
{
    return std::get<0>(value);
}

inline void registerTransformations(ECM &ecm)
{
    ecm.registerTransformation<MovementComponent>([&](auto eId, MovementComponent comp) {
        auto &projectile = first(ecm.get<ProjectileComponent>(eId));
        if (!projectile)
            return comp;

        auto [playerId, _] = ecm.getUnique<PlayerComponent>();
        auto &shooterId = projectile.peek(&ProjectileComponent::shooterId);
        if (shooterId != playerId || !first(ecm.get<PowerupEffect>(playerId)))
            return comp;

        comp.speeds.y += 1000;
        return comp;
    });
}

inline void stageBuilder(ECM &ecm, const std::vector<std::string_view> &stage, int tileSize)
{
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

inline void uiBuilder(ECM &ecm, const std::vector<std::string_view> &ui, int tileSize)
{
    for (int row = 0; row < ui.size(); ++row)
    {
        for (int col = 0; col < ui[row].size(); ++col)
        {
            auto constructor = UI::getEntityConstructor(ui[row][col]);
            if (!constructor)
                continue;

            constructor(ecm, col * tileSize, row * tileSize, tileSize, tileSize);
        }
    }
};

inline void stageBuilder(ECM &ecm, const std::vector<std::string_view> &stage)
{
    auto [_, gameMetaComps] = ecm.getUnique<GameMetaComponent>();
    auto &size = gameMetaComps.peek(&GameMetaComponent::screen);
    return stageBuilder(ecm, stage, size.x / stage[0].size());
}

inline void setup(ECM &ecm, ScreenConfig &screen)
{
    PRINT("STARTING GAME")
    auto stage = Stages::getStage(-1);
    int tileSize = screen.width / stage[0].size();
    Vector2 size{static_cast<float>(screen.width), static_cast<float>(screen.height)};

    createGame(ecm, size, tileSize);
    registerTransformations(ecm);
    stageBuilder(ecm, stage, tileSize);
    auto ui = UI::getUI(1);
    uiBuilder(ecm, ui, tileSize);
};

inline void nextStage(ECM &ecm, int stage)
{
    PRINT("STAGE:", stage, "LOADED")
    ecm.clear<HiveMovementEffect>();
    ecm.remove(ecm.getEntityIds<HiveAIComponent>());
    stageBuilder(ecm, Stages::getStage(stage));
};

inline void updateDeltaTime(ECM &ecm, float delta)
{
    auto [gameId, gameMetaComps] = ecm.getUnique<GameMetaComponent>();
    gameMetaComps.mutate([&](GameMetaComponent &gameMetaComp) { gameMetaComp.deltaTime = delta; });
};

inline float getDeltaTime(ECM &ecm)
{
    auto [gameId, gameMetaComps] = ecm.getUnique<GameMetaComponent>();
    return gameMetaComps.peek(&GameMetaComponent::deltaTime);
};

inline void registerPlayerInputs(ECM &ecm, std::vector<Inputs> &inputs)
{
    auto [playerId, _] = ecm.getUnique<PlayerComponent>();
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
    auto [gameId, gameComps] = ecm.getUnique<GameComponent>();
    return gameComps.peek(&GameComponent::isGameOver);
};

inline std::vector<Renderer::RenderableElement> getRenderableElements(ECM &ecm)
{
    std::vector<Renderer::RenderableElement> worldElements{};
    std::vector<Renderer::RenderableElement> uiElements{};

    ecm.getGroup<SpriteComponent, PositionComponent>().each(
        [&](EId eId, auto &spriteComps, auto &positionComps) {
            auto &rgba = spriteComps.peek(&SpriteComponent::rgba);
            auto [x, y, w, h] = positionComps.peek(&PositionComponent::bounds).get();
            Renderer::RenderableElement renderEl{x, y, w, h, rgba};
            auto &uiComps = first(ecm.get<UIComponent>(eId));
            auto &vec = !!uiComps ? uiElements : worldElements;
            if (uiComps)
            {
                auto &textComps = first(ecm.get<TextComponent>(eId));
                textComps.inspect([&](const TextComponent &textComp) {
                    renderEl.text = textComp.text;
                    renderEl.rgba = Renderer::RGBA{255, 255, 255, 255};
                });
            }

            vec.push_back(std::move(renderEl));
        });

    worldElements.insert(worldElements.end(), uiElements.begin(), uiElements.end());
    return worldElements;
};

inline template <typename... Ts> void cleanupEffect(ECM &ecm)
{
    std::apply(
        [&](auto &...set) {
            (set.each([&](EId eId, auto &effects) {
                effects.remove([&](auto &effect) {
                    if (effect.timer.has_value() && effect.timer->hasElapsed())
                        return true;

                    return effect.cleanup;
                });
            }),
             ...);
        },
        ecm.getAll<Ts...>());
}

}; // namespace Utilties
