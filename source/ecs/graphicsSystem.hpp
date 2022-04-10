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

            int m_maxZLayer = 0;

            friend class graphicsEngine;

        public:
            graphicsSystem();

            graphicsComponent &create();
            void handleDestruction();

            void setZLayer(graphicsComponent& component, int zLayer);
            int maxZLayer() const;

    };

namespace globals
    {
        extern graphicsSystem *g_graphicsSystem;
    }
