// breakout.hpp
// main game state for breakout
#pragma once
#include "baseGameState.hpp"
#include "graphics/camera.hpp"
#include "ecs/entity.hpp"
#include "ecs/playerControlSystem.hpp"
#include "ecs/physicsSystem.hpp"
#include "ecs/healthSystem.hpp"
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
                    GAMEPLAY,
                    GAME_OVER
                };

            fe::clock m_gameClock;
            fe::time m_currentStateEnter;
            state m_currentGameState = state::FIRST_SPAWN;
            bool m_firstStateIteration = true;

            playerControlSystem m_playerControlSystem;
            physicsSystem m_physics;
            healthSystem m_healthSystem;

            orthographicCamera m_gameCamera;

            entity m_player;
            plf::colony<entity> m_balls;
            plf::colony<entity> m_bricks;

            glm::vec2 m_ballSpawn;
            const float c_ballSpeed = 200.f;

            void createBall(glm::vec2 spawn, glm::vec2 velocity);

            void setGameState(state newState);

            void firstSpawnState();
            void respawnState();
            void gameplayState();
            void gameOverState();

        public:
            virtual void init() override final;
            virtual void deinit() override final {}
            virtual void onEnter() override final {}
            virtual void onLeave() override final {}

            virtual void update() override final;
            virtual void fixedUpdate(float dt) override final;

            virtual void preUpdate() override final;
            virtual void postUpdate() override final;

            virtual void preDraw() override final {}
            virtual void draw(graphicsEngine &graphics) override final;
            virtual void postDraw() override final {}
    };
