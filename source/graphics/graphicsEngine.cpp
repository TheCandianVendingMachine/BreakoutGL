#include "graphicsEngine.hpp"
#include "vertexArray.hpp"
#include "transformable.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include "window.hpp"
#include "str.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#include "ecs/graphicsSystem.hpp"
#include "ui/widgetManager.hpp"

graphicsEngine *globals::g_graphicsEngine = nullptr;

void graphicsEngine::createFramebuffers()
    {
    }

graphicsEngine::graphicsEngine(window &app, graphicsSystem &graphicsSystem, particleRenderer &particleSystem, widgetManager &widgetManager) :
    m_window(app),
    m_graphicsSystem(graphicsSystem),
    m_particleSystem(particleSystem),
    m_widgetManager(widgetManager),
    m_2dShader("shaders/basic2d.vs", "shaders/basic2d.fs"),
    m_tilemapShader("shaders/basic2d.vs", "shaders/basic2d.fs")
    {
        m_2dShader.use();
        m_2dShader.setInt("sprite", 0);

        m_screenWidth = app.width;
        m_screenHeight = app.height;

        createFramebuffers();
        app.subscribe(FE_STR("framebufferResize"), [this] (message &event, int) {
            m_screenWidth = event.arguments[0].variable.INT;
            m_screenHeight = event.arguments[1].variable.INT;

            createFramebuffers();
        });

        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST); // rendering in order of appearance for 2d

        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);
        glClearColor(0.f, 0.f, 0.f, 1.f);
    }

void graphicsEngine::draw(const camera &camera, unsigned int texture, drawFlags flags) const
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_2dShader.use();
        m_2dShader.setMat4("view", camera.view());
        m_2dShader.setMat4("projection", camera.projection());

        glBindVertexArray(m_graphicsSystem.m_spriteQuad.vao);

        int maxZ = m_graphicsSystem.maxZLayer();
        std::vector<std::vector<const graphicsComponent*>> zOrderedGraphics(static_cast<std::size_t>(maxZ + 1));
        for (const auto &graphicComponent : m_graphicsSystem.m_graphicComponents) 
            {
                zOrderedGraphics[maxZ - graphicComponent.zOrder].push_back(&graphicComponent);
            }

        for (auto &layer : zOrderedGraphics)
            {
                for (const auto& graphicComponent : layer) 
                    {
                        m_2dShader.setVec3("DefinedColour", graphicComponent->colour);
                        m_2dShader.setMat4("model", graphicComponent->transform.transform());
                        graphicComponent->texture.bind(GL_TEXTURE0);
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                    }
            }

        m_particleSystem.render(camera, texture);
        m_widgetManager.draw(camera, texture);

        glBindVertexArray(0);

        m_window.display();
    }

void graphicsEngine::setTilemap(tilemapSystem *tilemap)
    {
        m_tilemapSystem = tilemap;
    }


