// physicsSystem.hpp
// Updates physics components
#pragma once
#include <plf_colony.h>
#include "physicsComponent.hpp"

class physicsSystem
    {
        private:
            plf::colony<physicsComponent> m_physicsComponents;

        public:
            physicsComponent &create();

            void update(float dt);
            void handleDestruction();

    };
