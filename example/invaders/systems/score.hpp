#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Score
{
inline void cleanup(ECM &ecm)
{
}

inline auto update(ECM &ecm)
{
    auto [scoreEventSet] = ecm.getAll<ScoreEvent>();
    scoreEventSet.each([&](EId eId, auto &scoreEvents) {
        scoreEvents.inspect([&](const ScoreEvent &scoreEvent) {
            auto [pointsComps] = ecm.get<PointsComponent>(scoreEvent.pointsId);
            if (!pointsComps)
                return;

            auto [points, multiplier] =
                pointsComps.peek(&PointsComponent::points, &PointsComponent::multiplier);
            auto [scoreComps] = ecm.get<ScoreComponent>(eId);
            scoreComps.mutate([&](ScoreComponent &scoreComp) { scoreComp.score += (points * multiplier); });

            auto [playerId, _] = ecm.getUnique<PlayerComponent>();
            if (eId == playerId)
                ecm.add<UIEvent>(eId, UIEvents::UPDATE_SCORE);
        });
    });

    return cleanup;
}
}; // namespace Systems::Score
