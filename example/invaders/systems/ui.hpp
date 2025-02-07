#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::UI
{
inline void cleanup(ECM &ecm)
{
}

inline auto update(ECM &ecm)
{
    ecm.getAll<UIEvent>().each([&](EId eId, auto &uiEvents) {
        uiEvents.inspect([&](const UIEvent &uiEvent) {
            auto [playerId, playerComps] = ecm.get<PlayerComponent>();
            using Event = decltype(uiEvent.event);
            switch (uiEvent.event)
            {
            case Event::UPDATE_SCORE: {
                auto &score = ecm.get<ScoreComponent>(playerId).peek(&ScoreComponent::score);
                auto [playerScoreId, _] = ecm.get<PlayerScoreCardComponent>();
                ecm.get<TextComponent>(playerScoreId).mutate([&](TextComponent &textComp) {
                    textComp.text = "SCORE: " + std::to_string(score);
                });
                break;
            }
            case Event::UPDATE_LIVES: {
                auto &lives = ecm.get<LivesComponent>(playerId).peek(&LivesComponent::count);
                auto [playerLifeCardId, _] = ecm.get<PlayerLifeCardComponent>();
                ecm.get<TextComponent>(playerLifeCardId).mutate([&](TextComponent &textComp) {
                    textComp.text = "LIVES: " + std::to_string(lives);
                });
                break;
            }
            }
        });
    });

    return cleanup;
}
}; // namespace Systems::UI
