#include "breakout.hpp"
#include "ecs/graphicsSystem.hpp"
#include "graphics/graphicsEngine.hpp"
#include "inputHandler.hpp"
#include <GLFW/glfw3.h>

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

        graphicsComponent &ballGraphics = m_ball.addComponent<graphicsComponent>(globals::g_graphicsSystem->create());
        ballGraphics.transform.scale = { 30.f, 30.f };
        ballGraphics.texture.loadFromFile("ball.png", false);
        ballGraphics.colour = { 0.f, 0.f, 0.f };
    }

void breakout::fixedUpdate(float dt)
    {
        m_playerControlSystem.update(dt);
    }

void breakout::draw(graphicsEngine &graphics)
    {
        graphics.draw(m_gameCamera);
    }
