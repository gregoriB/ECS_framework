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
    ecm.getAll<ScoreEvent>().each([&](EId eId, auto &scoreEvents) {
        scoreEvents.inspect([&](const ScoreEvent &scoreEvent) {
            auto &pointsComps = ecm.get<PointsComponent>(scoreEvent.pointsId);
            if (!pointsComps)
                return;

            auto [points, multiplier] =
                pointsComps.peek(&PointsComponent::points, &PointsComponent::multiplier);
            ecm.get<ScoreComponent>(eId).mutate(
                [&](ScoreComponent &scoreComp) { scoreComp.score += (points * multiplier); });

            auto [playerId, _] = ecm.get<PlayerComponent>();
            if (eId == playerId)
                ecm.add<UIEvent>(eId, UIEvents::UPDATE_SCORE);
        });
    });

    return cleanup;
}
}; // namespace Systems::Score
