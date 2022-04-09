#include "breakout.hpp"
#include "ecs/graphicsSystem.hpp"
#include "graphics/graphicsEngine.hpp"
#include "inputHandler.hpp"
#include "random.hpp"
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

        collisionComponent &collider = ball.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::CIRCLE, "on ball collision", ""));
        collider.collider.circle.radius = 10.f;
        collider.position = spawn;
    }

void breakout::createPowerup(glm::vec2 brickCenter)
    {
        enum class powerups
            {
                MULTIBALL,
                SHORTEN,
                HEALTH,
                WIDEN,
                COUNT
            };

        entity &powerup = *m_powerups.emplace();
        powerup.setTag("powerup");

        graphicsComponent &graphics = powerup.addComponent<graphicsComponent>(globals::g_graphicsSystem->create());
        graphics.transform.scale = { 50, 30 };
        graphics.transform.position = brickCenter - graphics.transform.scale * 0.5f;

        physicsComponent &physics = powerup.addComponent<physicsComponent>(m_physics.create());
        physics.position = brickCenter - graphics.transform.scale * 0.5f;
        physics.velocity = { 0.f, 50.f };

        collisionComponent &collision = powerup.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::BOX, "powerup gained", ""));
        collision.position = brickCenter - graphics.transform.scale * 0.5f;
        collision.collider.box.extents = { 50.f, 30.f };

        powerups thisPowerup = static_cast<powerups>(fe::random(0, static_cast<int>(powerups::COUNT) - 1));
        switch (thisPowerup)
            {
                case powerups::MULTIBALL:
                    graphics.texture.loadFromFile("power-multiball.png", false);
                    powerup.setTag("multiball");
                    break;
                case powerups::SHORTEN:
                    graphics.texture.loadFromFile("power-shorten.png", false);
                    powerup.setTag("shorten");
                    break;
                case powerups::HEALTH:
                    graphics.texture.loadFromFile("powerup-health.png", false);
                    powerup.setTag("health");
                    break;
                case powerups::WIDEN:
                    graphics.texture.loadFromFile("power-widen.png", false);
                    powerup.setTag("widen");
                    break;
                default:
                    break;
            }
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

void breakout::resetState()
    {
        if (m_gameClock.getTime() - m_currentStateEnter >= fe::seconds(0.7))
            {
                setGameState(state::GAMEPLAY);
            }
    }

void breakout::respawnState()
    {
        if (m_gameClock.getTime() - m_currentStateEnter >= fe::seconds(0.65))
            {
                m_balls.clear();
                m_powerups.clear();

                createBall(m_ballSpawn, { 0.f, c_ballSpeed });

                m_player.getComponent<graphicsComponent>("graphics")->transform.position.x = 200.f;

                setGameState(state::RESET);
            }
    }

void breakout::gameplayState()
    {
        if (m_bricks.empty())
            {
                setGameState(state::LEVEL_COMPLETE);
            }
    }

void breakout::levelCompleteState()
    {
    }

breakout::breakout() :
    m_playerControlSystem(m_gameClock)
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

        m_player.setTag("player");

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

        globals::g_inputs->save("inputs.ini");

        m_player.addComponent(m_healthSystem.create(3, "playerKilled", "playerHit"));

        collisionComponent &playerCollider = m_player.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::BOX, "on player collision", ""));
        playerCollider.collider.box.extents = { 100.f, 30.f };
        playerCollider.position = playerGraphics.transform.position;

        m_ballSpawn = { 240.f, 800.f };
        createBall(m_ballSpawn, { 0.f, c_ballSpeed });

        m_scoreBox.setTag("score box");
        collisionComponent &scoreBoxCollider = m_scoreBox.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::BOX, "score box hit", ""));
        scoreBoxCollider.collider.box.extents = glm::vec2{ -m_gameCamera.right.x, 5.f };
        scoreBoxCollider.position = glm::vec2{ 0.f, static_cast<float>(scale) - scoreBoxCollider.collider.box.extents.y };

        collisionComponent &leftCollider = m_leftWall.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::BOX, "", ""));
        leftCollider.collider.box.extents = { 50.f, 1000.f };
        leftCollider.position = { -50.f, 0.f };
        m_leftWall.setTag("wall");

        collisionComponent &rightCollider = m_rightWall.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::BOX, "", ""));
        rightCollider.collider.box.extents = { 50.f, 1000.f };
        rightCollider.position = { 500.f, 0.f };
        m_rightWall.setTag("wall");

        collisionComponent &topCollider = m_topWall.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::BOX, "", ""));
        topCollider.collider.box.extents = { 500.f, 50.f };
        topCollider.position = { 0.f, -50.f };
        m_topWall.setTag("wall");
        m_topWall.setTag("top");

        constexpr float xBuffer = 30.f;
        constexpr float yBuffer = 50.f;
        constexpr float blockGap = 2.f;
        constexpr int xCount = 5;
        constexpr int yCount = 10;

        constexpr float blockWidth = (500.f - 2 * xBuffer - (xCount - 1) * blockGap) / xCount;
        constexpr float blockHeight = blockWidth / 2.f;

        for (int x = 0; x < xCount; x++)
            {
                for (int y = 0; y < yCount; y++)
                    {
                        entity &brick = *m_bricks.emplace();
                        graphicsComponent &brickGraphic = brick.addComponent<graphicsComponent>(globals::g_graphicsSystem->create());
                        brickGraphic.texture.loadFromFile("brick.png", false);
                        brickGraphic.transform.scale = { blockWidth, blockHeight };
                        brickGraphic.colour = { 1.f, 0.f, 0.2f };
                        brickGraphic.transform.position = { x * blockWidth + xBuffer + blockGap * x, y * blockHeight + yBuffer + blockGap * y};

                        collisionComponent &brickCollision = brick.addComponent<collisionComponent>(m_collisionSystem.create(collisionComponent::type::BOX, "brick hit", ""));
                        brickCollision.collider.box.extents = { blockWidth, blockHeight };
                        brickCollision.position = brickGraphic.transform.position;

                        brick.addComponent(m_healthSystem.create(1, "brick killed", ""));

                        brick.setTag("brick");
                    }
            }

        m_healthSystem.subscribe("playerHit", [this] (message &m, int) {
            spdlog::debug("Player hit");
            int damageTaken = 0;
            m.arguments[3].cast(damageTaken);
            if (damageTaken > 0) 
                {
                    setGameState(state::RESPAWN);
                }
        });

        m_healthSystem.subscribe("playerKilled", [this] (message &m, int) {
            spdlog::debug("Player killed");
            setGameState(state::GAME_OVER);
        });

        m_healthSystem.subscribe("brick killed", [this] (message &m, int) {
            spdlog::debug("brick killed");
            void *otherVoid = nullptr;
            m.arguments[0].cast(otherVoid);
            healthComponent *other = static_cast<healthComponent*>(otherVoid);
            graphicsComponent *graphics = other->entity->getComponent<graphicsComponent>("graphics");
            other->entity->kill();

            if (fe::randomNormal() < 0.2f)
                {
                    spdlog::debug("Powerup");
                    createPowerup(graphics->transform.position + graphics->transform.scale / 2.f);
                }
        });

        m_collisionSystem.subscribe("score box hit", [this] (message &m, int) {
            spdlog::debug("score box hit");
            void *otherVoid = nullptr;
            m.arguments[1].cast(otherVoid);
            collisionComponent *other = static_cast<collisionComponent*>(otherVoid);
            if (other->entity->hasTag("ball")) 
                {
                    if (m_balls.size() == 1) 
                        {
                            m_healthSystem.damage(*m_player.getComponent<healthComponent>("health"), 1);
                        }
                    else
                        {
                            other->entity->kill();
                        }
                }
        });

        m_collisionSystem.subscribe("brick hit", [this] (message &m, int) {
            spdlog::debug("brick hit");
            void *thisVoid = nullptr;
            m.arguments[0].cast(thisVoid);
            collisionComponent *thisCollider = static_cast<collisionComponent*>(thisVoid);

            void *otherVoid = nullptr;
            m.arguments[1].cast(otherVoid);
            collisionComponent *otherCollider = static_cast<collisionComponent *>(otherVoid);
            if (otherCollider->entity->hasTag("ball"))
                {
                    m_healthSystem.damage(*thisCollider->entity->getComponent<healthComponent>("health"), 1);
                }
        });

        m_collisionSystem.subscribe("on ball collision", [this] (message &m, int) {
            spdlog::debug("ball collision");

            void *thisVoid = nullptr;
            m.arguments[0].cast(thisVoid);
            collisionComponent *thisCollider = static_cast<collisionComponent*>(thisVoid);

            void *otherVoid = nullptr;
            m.arguments[1].cast(otherVoid);
            collisionComponent *other = static_cast<collisionComponent*>(otherVoid);

            physicsComponent *physics = thisCollider->entity->getComponent<physicsComponent>("physics");
            if (other->entity->hasTag("player"))
                {
                    glm::vec2 direction = glm::normalize((other->position + other->collider.box.extents * 0.5f) - thisCollider->position);
                    if (direction.x == 0.f)
                        {
                            direction.x = fe::randomNormal(-0.2f, 0.2f);
                        }
                    physics->velocity = -direction * c_ballSpeed;
                }
            else if (other->entity->hasTag("wall"))
                {
                    spdlog::debug("ball-wall collision");
                    if (other->entity->hasTag("top")) 
                        {
                            physics->velocity.y *= -1.f;
                            physics->position.y = 0.f;
                        }
                    else
                        {
                            physics->velocity.x *= -1.f;

                            float wallXPos = other->entity->getComponent<collisionComponent>("collision")->position.x;
                            if (wallXPos < physics->position.x) 
                                {
                                    physics->position.x = 0.f;
                                }
                            else 
                                {
                                    physics->position.x = wallXPos - thisCollider->collider.circle.radius;
                                }
                        }
                }
            else if (other->entity->hasTag("brick"))
                {
                    spdlog::debug("ball-brick collision");
                    glm::vec2 min = other->position;
                    glm::vec2 max = min + other->collider.box.extents;

                    glm::vec2 closestPoint = glm::min(glm::max(thisCollider->position, min), max);

                    glm::vec2 directionToPoint = glm::abs(closestPoint - thisCollider->position);

                    if (directionToPoint.x > directionToPoint.y)
                        {
                            physics->velocity.x *= -1.f;
                        }
                    else
                        {
                            physics->velocity.y *= -1.f;
                        }
                }
        });

        m_collisionSystem.subscribe("on player collision", [this] (message &m, int) {
            void *thisVoid = nullptr;
            m.arguments[0].cast(thisVoid);
            collisionComponent *thisCollider = static_cast<collisionComponent*>(thisVoid);

            void *otherVoid = nullptr;
            m.arguments[1].cast(otherVoid);
            collisionComponent *other = static_cast<collisionComponent*>(otherVoid);

            if (other->entity->hasTag("powerup"))
                {
                    other->entity->kill();

                    if (other->entity->hasTag("multiball"))
                        {
                            spdlog::debug("multiball get");
                            glm::vec2 spawnPos = thisCollider->position + glm::vec2(thisCollider->collider.box.extents.x * 0.5f, -30.f);
                            createBall(spawnPos, { 0.f, -c_ballSpeed });
                            createBall(spawnPos, c_ballSpeed * glm::normalize(glm::vec2{ 0.5f, -0.5f }));
                            createBall(spawnPos, c_ballSpeed * glm::normalize(glm::vec2{ -0.5f, -0.5f }));
                        }
                    else if (other->entity->hasTag("shorten"))
                        {
                            spdlog::debug("shorten get");
                            m_playerControlSystem.changePaddleState(*m_player.getComponent<playerControlComponent>("playerControlComponent"), playerControlComponent::paddleState::SHORT);
                        }
                    else if (other->entity->hasTag("health"))
                        {
                            spdlog::debug("health get");
                            m_healthSystem.damage(*m_player.getComponent<healthComponent>("health"), -1);
                        }
                    else if (other->entity->hasTag("widen"))
                        {
                            spdlog::debug("widen get");
                            m_playerControlSystem.changePaddleState(*m_player.getComponent<playerControlComponent>("playerControlComponent"), playerControlComponent::paddleState::WIDE);
                        }
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
                case breakout::state::RESET:
                    resetState();
                    break;
                case breakout::state::RESPAWN:
                    respawnState();
                    break;
                case breakout::state::GAMEPLAY:
                    gameplayState();
                    break;
                case breakout::state::LEVEL_COMPLETE:
                    levelCompleteState();
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

        for (auto it = m_balls.begin(); it != m_balls.end();)
            {
                if (it->killed())
                    {
                        it = m_balls.erase(it);
                    }
                else
                    {
                        entity &ball = *it;
                        if (ball.hasComponent("physics") && ball.hasComponent("graphics"))
                            {
                                collisionComponent *collider = ball.getComponent<collisionComponent>("collision");
                                glm::vec2 physicsPosition = ball.getComponent<physicsComponent>("physics")->position;

                                ball.getComponent<graphicsComponent>("graphics")->transform.position = physicsPosition;

                                collider->position = physicsPosition + glm::vec2(collider->collider.circle.radius);
                            }
                        ++it;
                    }
            }

        for (auto it = m_powerups.begin(); it != m_powerups.end();)
            {
                if (it->killed())
                    {
                        it = m_powerups.erase(it);
                    }
                else
                    {
                        entity &powerup = *it;
                        collisionComponent *collider = powerup.getComponent<collisionComponent>("collision");
                        glm::vec2 physicsPosition = powerup.getComponent<physicsComponent>("physics")->position;

                        powerup.getComponent<graphicsComponent>("graphics")->transform.position = physicsPosition;

                        collider->position = physicsPosition;
                        ++it;
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

        for (auto it = m_bricks.begin(); it != m_bricks.end();)
            {
                if (it->killed())
                    {
                        it = m_bricks.erase(it);
                    }
                else
                    {
                        ++it;
                    }
            }

        for (auto it = m_powerups.begin(); it != m_powerups.end();)
            {
                if (it->killed())
                    {
                        it = m_powerups.erase(it);
                    }
                else
                    {
                        ++it;
                    }
            }
    }

void breakout::draw(graphicsEngine &graphics)
    {
        graphics.draw(m_gameCamera);
    }

