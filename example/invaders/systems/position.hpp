#pragma once

#include "../components.hpp"
#include "../core.hpp"

namespace Systems::Position
{
inline void cleanup(ECM &ecm)
{
}

inline auto update(ECM &ecm)
{
    auto [positionEventSet] = ecm.getAll<PositionEvent>();
    positionEventSet.each([&](EId eId, auto &positionEvents) {
        positionEvents.first().inspect([&](const PositionEvent &positionEvent) {
            auto [positionComps] = ecm.get<PositionComponent>(eId);
            positionComps.mutate([&](PositionComponent &positionComp) {
                positionComp.bounds.position.x = positionEvent.coords.x;
                positionComp.bounds.position.y = positionEvent.coords.y;
            });
        });
    });

    return cleanup;
};
}; // namespace Systems::Position
