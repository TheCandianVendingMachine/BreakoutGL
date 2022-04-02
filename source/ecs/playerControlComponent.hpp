// playerControlComponent.hpp
// Allows the entity to be controlled by the player
#pragma once
#include "component.hpp"
#include "game/controllers/paddleController.hpp"
#include "inputHandler.hpp"
#include "time.hpp"

struct playerControlComponent : component
    {
        virtual const char *name() const override final { return "playerControlComponent"; }

        playerControlComponent(inputHandler::input left, inputHandler::input right, float speed, float maxRight, float playerWidth) :
            leftInput(left),
            rightInput(right),
            controller{.speed = speed},
            maxRightExtent(maxRight),
            width(playerWidth)
        {}

        inputHandler::input leftInput;
        inputHandler::input rightInput;
        paddleController controller;
        float maxRightExtent = 0.f;
        float width = 0.f;

        enum class paddleState
            {
                NORMAL,
                SHORT,
                WIDE
            };
        paddleState state = paddleState::NORMAL;
        fe::time timeStateEntered = 0;
    };
