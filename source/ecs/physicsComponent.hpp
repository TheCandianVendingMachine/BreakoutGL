// physicsComponent.hpp
// Defines a velocity, position of an entity
#pragma once
#include "component.hpp"
#include <glm/vec2.hpp>

struct physicsComponent : component 
    {
        virtual const char *name() const override final { return "physics"; }

        glm::vec2 velocity;
        glm::vec2 position;
    };
