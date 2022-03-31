#include "breakout.hpp"
#include "ecs/graphicsSystem.hpp"
#include "graphics/graphicsEngine.hpp"
#include "inputHandler.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void breakout::createBall(glm::vec2 spawn, glm::vec2 velocity)
    {
        entity &ball = *m_balls.emplace();
        ball.setTag("ball");

        graphicsComponent &ballGraphics = ball.addComponent<graphicsComponent>(globals::g_graphicsSystem->create());
        ballGraphics.transform.scale = { 10.f, 10.f };
        ballGraphics.texture.loadFromFile("ball.png", false);
        ballGraphics.colour = { 0.f, 0.f, 1.f };

        physicsComponent &physics = ball.addComponent<physicsComponent>(m_physics.create());
        physics.position = spawn;
        physics.velocity = velocity;

        collisionComponent &collider = ball.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::CIRCLE, "", ""));
        collider.collider.circle.radius = 10.f;
        collider.position = spawn;
    }

void breakout::setGameState(state newState)
    {
        m_nextGameState = newState;
        m_newState = true;
    }

void breakout::firstSpawnState()
    {
        if (m_gameClock.getTime() - m_currentStateEnter >= fe::seconds(1))
            {
                setGameState(state::GAMEPLAY);
            }
    }

void breakout::respawnState()
    {
        if (m_gameClock.getTime() - m_currentStateEnter >= fe::seconds(1))
            {
                m_balls.clear();
                createBall(m_ballSpawn, { 0.f, c_ballSpeed });

                setGameState(state::GAMEPLAY);
            }
    }

void breakout::gameplayState()
    {
    }

void breakout::gameOverState()
    {
    }

void breakout::init()
    {
        m_gameCamera.position = { 0, 0, 0 };
        m_gameCamera.left = { 0.f, 0.f };

        constexpr int scale = 1000;
        m_gameCamera.right = { -0.5f * scale, 1.f * scale };
        m_gameCamera.zNear = -1.f;
        m_gameCamera.zFar = 1.f;

        graphicsComponent &playerGraphics = m_player.addComponent<graphicsComponent>(globals::g_graphicsSystem->create());
        playerGraphics.transform.scale = { 100.f, 30.f };
        playerGraphics.transform.position = { 200.f, 900.f };
        playerGraphics.texture.loadFromFile("paddle.png", false);

        m_player.addComponent(m_playerControlSystem.create(
            globals::g_inputs->addDefaultInput("Gameplay", "Move Left", GLFW_KEY_LEFT),
            globals::g_inputs->addDefaultInput("Gameplay", "Move Right", GLFW_KEY_RIGHT),
            300.f,
            500.f,
            100.f
        ));

        m_player.addComponent(m_healthSystem.create(3, "playerKilled", "playerHit"));

        m_ballSpawn = { 240.f, 800.f };
        createBall(m_ballSpawn, { 0.f, c_ballSpeed });

        m_scoreBox.setTag("score box");
        collisionComponent &scoreBoxCollider = m_scoreBox.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::BOX, "score box hit", ""));
        scoreBoxCollider.collider.box.extents = glm::vec2{ -m_gameCamera.right.x, 50.f };
        scoreBoxCollider.position = glm::vec2{ 0.f, static_cast<float>(scale) - scoreBoxCollider.collider.box.extents.y };

        m_healthSystem.subscribe("playerHit", [this] (message &m) {
            spdlog::debug("Player hit");
            setGameState(state::RESPAWN);
        });

        m_healthSystem.subscribe("playerKilled", [this] (message &m) {
            spdlog::debug("Player killed");
            setGameState(state::GAME_OVER);
        });

        m_collisionSystem.subscribe("score box hit", [this] (message &m) {
            spdlog::debug("score box hit");
            void *otherVoid = nullptr;
            m.arguments[1].cast(otherVoid);
            collisionComponent *other = static_cast<collisionComponent*>(otherVoid);
            if (other->entity->hasTag("ball")) 
                {
                    m_healthSystem.damage(*m_player.getComponent<healthComponent>("health"), 1);
                }
        });
    }

void breakout::update()
    {
        switch (m_currentGameState)
            {
                case breakout::state::FIRST_SPAWN:
                    firstSpawnState();
                    break;
                case breakout::state::RESPAWN:
                    respawnState();
                    break;
                case breakout::state::GAMEPLAY:
                    gameplayState();
                    break;
                case breakout::state::GAME_OVER:
                    gameOverState();
                    break;
                default:
                    break;
            }
    }

void breakout::fixedUpdate(float dt)
    {
        if (m_currentGameState == breakout::state::GAMEPLAY)
            {
                m_playerControlSystem.update(dt);
                m_physics.update(dt);
            }

        for (auto &ball : m_balls)
            {
                if (ball.hasComponent("physics") && ball.hasComponent("graphics"))
                    {
                        glm::vec2 physicsPosition = ball.getComponent<physicsComponent>("physics")->position;
                        ball.getComponent<graphicsComponent>("graphics")->transform.position = physicsPosition;

                        collisionComponent *collider = ball.getComponent<collisionComponent>("collision");
                        collider->position = physicsPosition + glm::vec2(collider->collider.circle.radius);
                    }
            }

        m_collisionSystem.update();
    }

void breakout::preUpdate()
    {
        if (m_newState)
            {
                spdlog::debug("setting game state to {} from {} at {}", m_nextGameState, m_currentGameState, m_gameClock.getTime().asSeconds());
                m_currentStateEnter = m_gameClock.getTime();
                m_currentGameState = m_nextGameState;
                m_newState = false;
            }

        m_playerControlSystem.handleDestruction();
        m_physics.handleDestruction();
        m_healthSystem.handleDestruction();
        m_collisionSystem.handleDestruction();
    }

void breakout::draw(graphicsEngine &graphics)
    {
        graphics.draw(m_gameCamera);
    }
