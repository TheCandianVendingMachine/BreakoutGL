// graphicsComponent.hpp
// Holds information related to rendering an object
#pragma once
#include "component.hpp"
#include "graphics/texture.hpp"
#include "graphics/transformable.hpp"

struct graphicsComponent : component
    {
        virtual const char *name() const override final { return "graphics"; }

        transformable transform;
        texture texture;
    };
