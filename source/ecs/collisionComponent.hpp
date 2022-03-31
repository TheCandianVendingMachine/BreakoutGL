// collisionComponent.hpp
// Defines a collider. Can be circle or box collider
#pragma once
#include "component.hpp"
#include <glm/vec2.hpp>

struct collisionComponent : component
    {
        virtual const char *name() const override final { return "collision"; }

        enum class type
            {
                NONE,
                CIRCLE,
                BOX
            };

        struct box
            {
                glm::vec2 extents;
            };

        struct circle
            {
                float radius;
            };

        union {
            box box;
            circle circle;
        } collider;

        unsigned int uid = 0;

        type shape = type::NONE;
        glm::vec2 position;

        fe::str eventOnCollisionStart = 0;
        fe::str eventOnCollision = 0;

    };
