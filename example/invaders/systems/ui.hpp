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
    auto [uiEventSet] = ecm.getAll<UIEvent>();
    uiEventSet.each([&](EId eId, auto &uiEvents) {
        uiEvents.inspect([&](const UIEvent &uiEvent) {
            auto [playerId, playerComps] = ecm.getUnique<PlayerComponent>();
            using Event = decltype(uiEvent.event);
            switch (uiEvent.event)
            {
            case Event::UPDATE_SCORE: {
                auto [scoreComps] = ecm.get<ScoreComponent>(playerId);
                auto &score = scoreComps.peek(&ScoreComponent::score);
                auto [playerScoreId, _] = ecm.getUnique<PlayerScoreCardComponent>();
                auto [textComps] = ecm.get<TextComponent>(playerScoreId);
                textComps.mutate(
                    [&](TextComponent &textComp) { textComp.text = "SCORE: " + std::to_string(score); });
                break;
            }
            case Event::UPDATE_LIVES: {
                auto [livesComps] = ecm.get<LivesComponent>(playerId);
                auto &lives = livesComps.peek(&LivesComponent::count);
                auto [playerLifeCardId, _] = ecm.getUnique<PlayerLifeCardComponent>();
                auto [textComps] = ecm.get<TextComponent>(playerLifeCardId);
                textComps.mutate(
                    [&](TextComponent &textComp) { textComp.text = "LIVES: " + std::to_string(lives); });
                break;
            }
            }
        });
    });

    return cleanup;
}
}; // namespace Systems::UI
