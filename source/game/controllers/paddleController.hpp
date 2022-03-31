// paddleController.hpp
// Moves the paddle based on some input
#pragma once
#include <glm/vec2.hpp>

class paddleController
    {
        public:
            enum class direction : char
                {
                    LEFT = -1, 
                    NONE = 0,
                    RIGHT = 1
                };
            friend direction operator+(direction lhs, direction rhs) { return static_cast<direction>(static_cast<char>(lhs) + static_cast<char>(rhs)); }
            friend direction operator+=(direction &lhs, direction rhs) { lhs = lhs + rhs; return lhs; }

            float speed = 0.f;

            glm::vec2 getPositionDelta(direction direction) const;
    };
