// playerControlComponent.hpp
// Allows the entity to be controlled by the player
#pragma once
#include "component.hpp"
#include "game/controllers/paddleController.hpp"
#include "inputHandler.hpp"

struct playerControlComponent : component
    {
        virtual const char *name() const override final { return "playerControlComponent"; }

        playerControlComponent(inputHandler::input left, inputHandler::input right, float speed) :
            leftInput(left),
            rightInput(right),
            controller{.speed = speed}
        {}

        inputHandler::input leftInput;
        inputHandler::input rightInput;
        paddleController controller;
    };
