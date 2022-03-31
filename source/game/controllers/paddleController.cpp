#include "paddleController.hpp"

glm::vec2 paddleController::getPositionDelta(direction direction) const
    {
        return glm::vec2(static_cast<float>(direction) * speed, 0.f);
    }
