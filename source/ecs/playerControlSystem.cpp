#include "playerControlSystem.hpp"
#include "ecs/entity.hpp"
#include "graphicsComponent.hpp"
#include "collisionComponent.hpp"

playerControlSystem::playerControlSystem(fe::clock &clock) :
    m_clock(clock)
    {
    }

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

                if (controller.state != playerControlComponent::paddleState::NORMAL && m_clock.getTime() - controller.timeStateEntered >= fe::seconds(10))
                    {
                        changePaddleState(controller, playerControlComponent::paddleState::NORMAL);
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

void playerControlSystem::changePaddleState(playerControlComponent &controller, playerControlComponent::paddleState state)
    {
        controller.state = state;
        controller.timeStateEntered = m_clock.getTime();

        collisionComponent *collision = controller.entity->getComponent<collisionComponent>("collision");
        graphicsComponent *graphics = controller.entity->getComponent<graphicsComponent>("graphics");

        switch (state)
            {
                case playerControlComponent::paddleState::NORMAL:
                    collision->collider.box.extents = { 100.f, 30.f };
                    graphics->transform.scale = { 100.f, 30.f };

                    graphics->texture.loadFromFile("paddle.png", false);
                    break;
                case playerControlComponent::paddleState::SHORT:
                    collision->collider.box.extents = { 70.f, 30.f };
                    graphics->transform.scale = { 70.f, 30.f };

                    graphics->texture.loadFromFile("narrow-paddle.png", false);
                    break;
                case playerControlComponent::paddleState::WIDE:
                    collision->collider.box.extents = { 130.f, 30.f };
                    graphics->transform.scale = { 130.f, 30.f };

                    graphics->texture.loadFromFile("wide-paddle.png", false);
                    break;
                default:
                    break;
            }
    }
