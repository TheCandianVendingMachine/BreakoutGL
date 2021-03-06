// transformable.hpp
// base class that allows for a object to be transformed through 3d space
#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class transformable
    {
        public:
            glm::vec2 position = { 0, 0  };
            glm::vec2 scale = { 1, 1 };
            float angle = 0.f;

            glm::mat4 transform() const;
    };
