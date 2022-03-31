#include "breakout.hpp"
#include "ecs/graphicsSystem.hpp"
#include "graphics/graphicsEngine.hpp"
#include "inputHandler.hpp"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

void breakout::createBall(glm::vec2 spawn, glm::vec2 velocity)
    {
        entity &ball = *m_balls.emplace();

        graphicsComponent &ballGraphics = ball.addComponent<graphicsComponent>(globals::g_graphicsSystem->create());
        ballGraphics.transform.scale = { 10.f, 10.f };
        ballGraphics.texture.loadFromFile("ball.png", false);
        ballGraphics.colour = { 0.f, 0.f, 1.f };

        physicsComponent &physics = ball.addComponent<physicsComponent>(m_physics.create());
        physics.position = spawn;
        physics.velocity = velocity;
    }

void breakout::setGameState(state newState)
    {
        spdlog::debug("setting game state to {} from {} at {}", newState, m_currentGameState, m_gameClock.getTime().asSeconds());
        m_currentGameState = newState;
        m_currentStateEnter = m_gameClock.getTime();

        m_firstStateIteration = true;
    }

void breakout::firstSpawnState()
    {
        if (m_firstStateIteration) 
            {
                createBall(m_ballSpawn, { 0.f, c_ballSpeed });
            }

        if (m_gameClock.getTime() - m_currentStateEnter >= fe::seconds(1))
            {
                setGameState(state::GAMEPLAY);
            }
    }

void breakout::respawnState()
    {
        if (m_gameClock.getTime() - m_currentStateEnter >= fe::seconds(1))
            {
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

        m_healthSystem.subscribe("playerHit", [this] (message &m) {
            spdlog::debug("Player hit");
            setGameState(state::RESPAWN);
        });

        m_healthSystem.subscribe("playerKilled", [this] (message &m) {
            spdlog::debug("Player killed");
            setGameState(state::GAME_OVER);
        });

        m_ballSpawn = { 240.f, 800.f };
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
                    }
            }
    }

void breakout::preUpdate()
    {
        m_playerControlSystem.handleDestruction();
        m_physics.handleDestruction();
        m_healthSystem.handleDestruction();
    }

void breakout::postUpdate()
    {
        m_firstStateIteration = false;
    }

void breakout::draw(graphicsEngine &graphics)
    {
        graphics.draw(m_gameCamera);
    }
