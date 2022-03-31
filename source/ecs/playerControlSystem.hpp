// playerControlSystem.hpp
// Handles the updating of the player controller and handling the transforms of all related components
#pragma once
#include <plf_colony.h>
#include "playerControlComponent.hpp"

class playerControlSystem
    {
        private:
            plf::colony<playerControlComponent> m_playerControllers;

        public:
            playerControlComponent &create(inputHandler::input left, inputHandler::input right, float speed);

            void update(float dt);

    };
