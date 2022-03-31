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

graphicsEngine *globals::g_graphicsEngine = nullptr;

void graphicsEngine::createFramebuffers()
    {
    }

graphicsEngine::graphicsEngine(window &app, graphicsSystem &graphicsSystem) :
    m_window(app),
    m_graphicsSystem(graphicsSystem),
    m_2dShader("shaders/basic2d.vs", "shaders/basic2d.fs"),
    m_tilemapShader("shaders/basic2d.vs", "shaders/basic2d.fs")
    {
        m_2dShader.use();
        m_2dShader.setInt("sprite", 0);

        m_screenWidth = app.width;
        m_screenHeight = app.height;

        createFramebuffers();
        app.subscribe(FE_STR("framebufferResize"), [this] (message &event) {
            m_screenWidth = event.arguments[0].variable.INT;
            m_screenHeight = event.arguments[1].variable.INT;

            createFramebuffers();
        });

        glDisable(GL_CULL_FACE);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
    }

void graphicsEngine::draw(const camera &camera, unsigned int texture, drawFlags flags) const
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        m_2dShader.use();
        m_2dShader.setMat4("view", camera.view());
        m_2dShader.setMat4("projection", camera.projection());

        glBindVertexArray(m_graphicsSystem.m_spriteQuad.vao);
        for (const auto &graphicComponent : m_graphicsSystem.m_graphicComponents) 
            {
                m_2dShader.setMat4("model", graphicComponent.transform.transform());
                graphicComponent.texture.bind(GL_TEXTURE0);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }

        glBindVertexArray(0);

        m_window.display();
    }

void graphicsEngine::setTilemap(tilemapSystem *tilemap)
    {
        m_tilemapSystem = tilemap;
    }


