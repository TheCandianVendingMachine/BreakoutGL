// healthSystem.hpp
// Handles updating health components and signalling when they die
#pragma once
#include <plf_colony.h>
#include "healthComponent.hpp"
#include "messaging/eventSource.hpp"

class healthSystem : public eventSource
    {
        private:
            plf::colony<healthComponent> m_healthComponents;

        public:
            healthComponent &create(int maxHealth, const char *messageOnDeath);

            void update();
            void damage(healthComponent &health, int amount);

    };
