#include "breakout.hpp"
#include "ecs/graphicsSystem.hpp"
#include "graphics/graphicsEngine.hpp"
#include "inputHandler.hpp"
#include <GLFW/glfw3.h>

void breakout::createBall(glm::vec2 spawn, glm::vec2 velocity)
    {
        entity &ball = *m_balls.emplace();

        graphicsComponent &ballGraphics = ball.addComponent<graphicsComponent>(globals::g_graphicsSystem->create());
        ballGraphics.transform.scale = { 30.f, 30.f };
        ballGraphics.texture.loadFromFile("ball.png", false);
        ballGraphics.colour = { 0.f, 0.f, 1.f };

        physicsComponent &physics = ball.addComponent<physicsComponent>(m_physics.create());
        physics.position = spawn;
        physics.velocity = velocity;
    }

void breakout::init()
    {
        m_gameCamera.position = { 0, 0, 0 };
        m_gameCamera.left = { 0.f, 0.f };

        constexpr int scale = 320;
        m_gameCamera.right = { -4.f * scale, 3.f * scale };
        m_gameCamera.zNear = -1.f;
        m_gameCamera.zFar = 1.f;

        graphicsComponent &playerGraphics = m_player.addComponent<graphicsComponent>(globals::g_graphicsSystem->create());
        playerGraphics.transform.scale = { 100.f, 30.f };
        playerGraphics.transform.position = { 100.f, 30.f };
        playerGraphics.texture.loadFromFile("paddle.png", false);

        m_player.addComponent(m_playerControlSystem.create(
            globals::g_inputs->addDefaultInput("Gameplay", "Move Left", GLFW_KEY_LEFT),
            globals::g_inputs->addDefaultInput("Gameplay", "Move Right", GLFW_KEY_RIGHT),
            100.f
        ));

        createBall({0.f, 0.f}, {0.f, 0.f});
    }

void breakout::fixedUpdate(float dt)
    {
        m_playerControlSystem.update(dt);
        m_physics.update(dt);

        for (auto &ball : m_balls)
            {
                if (ball.hasComponent("physics") && ball.hasComponent("graphics"))
                    {
                        glm::vec2 physicsPosition = ball.getComponent<physicsComponent>("physics")->position;
                        ball.getComponent<graphicsComponent>("graphics")->transform.position = physicsPosition;
                    }
            }
    }

void breakout::preUpdate()
    {
        m_playerControlSystem.handleDestruction();
        m_physics.handleDestruction();
        m_healthSystem.handleDestruction();
    }

void breakout::draw(graphicsEngine &graphics)
    {
        graphics.draw(m_gameCamera);
    }
