#include "playerControlSystem.hpp"
#include "ecs/entity.hpp"
#include "graphicsComponent.hpp"

playerControlComponent &playerControlSystem::create(inputHandler::input left, inputHandler::input right, float speed)
    {
        playerControlComponent &controller = *m_playerControllers.emplace(left, right, speed);

        return controller;
    }

void playerControlSystem::update(float dt)
    {
        for (auto &controller : m_playerControllers)
            {
                paddleController::direction direction = paddleController::direction::NONE;
                if (globals::g_inputs->keyState(controller.leftInput) == inputHandler::inputState::PRESS)
                    {
                        direction += paddleController::direction::LEFT;
                    }
                if (globals::g_inputs->keyState(controller.rightInput) == inputHandler::inputState::PRESS)
                    {
                        direction += paddleController::direction::RIGHT;
                    }

                glm::vec2 delta = controller.controller.getPositionDelta(direction);

                entity *parent = controller.entity;
                if (parent->hasComponent("graphics"))
                    {
                        parent->getComponent<graphicsComponent>("graphics")->transform.position += delta *dt;
                    }
            }
    }

void playerControlSystem::handleDestruction()
    {
        for (auto it = m_playerControllers.begin(); it != m_playerControllers.end();)
            {
                if (it->destroy)
                    {
                        it = m_playerControllers.erase(it);
                    }
                else
                    {
                        it++;
                    }
            }
    }
