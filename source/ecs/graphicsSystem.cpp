#include "graphicsSystem.hpp"
#include <algorithm>

graphicsSystem *globals::g_graphicsSystem = nullptr;

graphicsSystem::graphicsSystem()
    {
        vertex v0{
            .position = { 0.f, 0.f, 0.f },
            .textureCoordinate = { 0.f, 1.f },
            .colour = { 1.f, 1.f, 1.f, 1.f }
        };
        vertex v1{
            .position = { 1.f, 0.f, 0.f },
            .textureCoordinate = { 1.f, 1.f },
            .colour = { 1.f, 1.f, 1.f, 1.f }
        };
        vertex v2{
            .position = { 1.f, 1.f, 0.f },
            .textureCoordinate = { 1.f, 0.f },
            .colour = { 1.f, 1.f, 1.f, 1.f }
        };
        vertex v3{
            .position = { 0.f, 1.f, 0.f },
            .textureCoordinate = { 0.f, 0.f },
            .colour = { 1.f, 1.f, 1.f, 1.f }
        };

        m_spriteQuad.bindVertices({ v0, v1, v2, v3 });
        m_spriteQuad.bindIndices({ 0, 1, 2, 2, 3, 0 });
        m_spriteQuad.use(vertex::attributes::POSITION | vertex::attributes::TEXTURE | vertex::attributes::COLOUR);
    }

graphicsComponent &graphicsSystem::create()
    {
        return *m_graphicComponents.emplace();
    }

void graphicsSystem::handleDestruction()
    {
        for (auto it = m_graphicComponents.begin(); it != m_graphicComponents.end();)
            {
                if (it->destroy)
                    {
                        it = m_graphicComponents.erase(it);
                    }
                else
                    {
                        it++;
                    }
            }
    }

void graphicsSystem::setZLayer(graphicsComponent& component, int zLayer)
    {
        component.zOrder = zLayer;
        m_maxZLayer = std::max(m_maxZLayer, zLayer);
    }

int graphicsSystem::maxZLayer() const
    {
        return m_maxZLayer;
    }
