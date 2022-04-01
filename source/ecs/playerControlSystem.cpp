#include "playerControlSystem.hpp"
#include "ecs/entity.hpp"
#include "graphicsComponent.hpp"
#include "collisionComponent.hpp"

playerControlComponent &playerControlSystem::create(inputHandler::input left, inputHandler::input right, float speed, float maxRight, float width)
    {
        playerControlComponent &controller = *m_playerControllers.emplace(left, right, speed, maxRight, width);

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
                        graphicsComponent *graphics = parent->getComponent<graphicsComponent>("graphics");
                        glm::vec2 pos = graphics->transform.position;
                        pos += delta * dt;

                        pos.x = std::min(pos.x, controller.maxRightExtent - controller.width);
                        pos.x = std::max(0.f, pos.x);

                        graphics->transform.position = pos;

                        if (parent->hasComponent("collision"))
                            {
                                collisionComponent *collision = parent->getComponent<collisionComponent>("collision");
                                collision->position = pos;
                            }
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
