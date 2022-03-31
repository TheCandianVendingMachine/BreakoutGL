// graphicsSystem.hpp
// Handles the scene: traverses Z buffer tree and draws as needed
#pragma once
#include "plf_colony.h"
#include "graphicsComponent.hpp"
#include "graphics/vertexArray.hpp"

class graphicsSystem
    {
        private:
            plf::colony<graphicsComponent> m_graphicComponents;
            vertexArray m_spriteQuad;

            friend class graphicsEngine;

        public:
            graphicsSystem();

            graphicsComponent &create();
            void handleDestruction();

    };

namespace globals
    {
        extern graphicsSystem *g_graphicsSystem;
    }
