// breakout.hpp
// main game state for breakout
#pragma once
#include "baseGameState.hpp"
#include "graphics/camera.hpp"
#include "ecs/entity.hpp"
#include "ecs/playerControlSystem.hpp"
#include "ecs/physicsSystem.hpp"
#include "ecs/healthSystem.hpp"
#include "ecs/collisionSystem.hpp"
#include "time.hpp"
#include "clock.hpp"
#include <plf_colony.h>
#include <glm/vec2.hpp>

class breakout : public baseGameState
    {
        private:
            enum class state
                {
                    FIRST_SPAWN,
                    RESPAWN,
                    RESET,
                    GAMEPLAY,
                    LEVEL_COMPLETE,
                    GAME_OVER
                };

            fe::clock m_gameClock;
            fe::time m_currentStateEnter;
            state m_currentGameState = state::FIRST_SPAWN;
            state m_nextGameState;
            bool m_newState = false;

            playerControlSystem m_playerControlSystem;
            physicsSystem m_physics;
            healthSystem m_healthSystem;
            collisionSystem m_collisionSystem;

            orthographicCamera m_gameCamera;

            entity m_player;
            entity m_scoreBox;

            entity m_leftWall;
            entity m_rightWall;
            entity m_topWall;

            plf::colony<entity> m_balls;
            plf::colony<entity> m_bricks;
            plf::colony<entity> m_powerups;

            glm::vec2 m_ballSpawn;
            const float c_ballSpeed = 200.f;

            void createBall(glm::vec2 spawn, glm::vec2 velocity);
            void createPowerup(glm::vec2 brickCenter);

            void setGameState(state newState);

            void firstSpawnState();
            void respawnState();
            void resetState();
            void gameplayState();
            void gameOverState();
            void levelCompleteState();

        public:
            breakout();

            virtual void init() override final;

            virtual void update() override final;
            virtual void fixedUpdate(float dt) override final;

            virtual void preUpdate() override final;

            virtual void draw(graphicsEngine &graphics) override final;
    };
