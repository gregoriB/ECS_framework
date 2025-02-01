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
    ecm.getAll<PositionEvent>().each([&](EId eId, auto &positionEvents) {
        positionEvents.first().inspect([&](const PositionEvent &positionEvent) {
            ecm.get<PositionComponent>(eId).mutate([&](PositionComponent &positionComp) {
                positionComp.bounds.position.x = positionEvent.coords.x;
                positionComp.bounds.position.y = positionEvent.coords.y;
            });
        });
    });

    return cleanup;
};
}; // namespace Systems::Position
