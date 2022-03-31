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

        m_playerController.speed = 100.f;

        globals::g_inputs->addDefaultInput("Gameplay", "Move Left", GLFW_KEY_LEFT);
        globals::g_inputs->addDefaultInput("Gameplay", "Move Right", GLFW_KEY_RIGHT);
    }

void breakout::fixedUpdate(float dt)
    {
        paddleController::direction direction = paddleController::direction::NONE;
        if (globals::g_inputs->keyState("Gameplay", "Move Left") == inputHandler::inputState::PRESS)
            {
                direction += paddleController::direction::LEFT;
            }
        if (globals::g_inputs->keyState("Gameplay", "Move Right") == inputHandler::inputState::PRESS)
            {
                direction += paddleController::direction::RIGHT;
            }

        m_player.getComponent<graphicsComponent>("graphics")->transform.position += m_playerController.getPositionDelta(direction) * dt;
    }

void breakout::draw(graphicsEngine &graphics)
    {
        graphics.draw(m_gameCamera);
    }
