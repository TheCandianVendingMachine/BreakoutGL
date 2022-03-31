// breakout.hpp
// main game state for breakout
#pragma once
#include "baseGameState.hpp"
#include "graphics/camera.hpp"
#include "ecs/entity.hpp"
#include "ecs/playerControlSystem.hpp"
#include <plf_colony.h>

class breakout : public baseGameState
    {
        private:
            playerControlSystem m_playerControlSystem;

            orthographicCamera m_gameCamera;
            entity m_player;
            entity m_ball;
            plf::colony<entity> m_bricks;

        public:
            virtual void init() override final;
            virtual void deinit() override final {}
            virtual void onEnter() override final {}
            virtual void onLeave() override final {}

            virtual void update() override final {}
            virtual void fixedUpdate(float dt) override final;

            virtual void preUpdate() override final {}
            virtual void postUpdate() override final {}

            virtual void preDraw() override final {}
            virtual void draw(graphicsEngine &graphics) override final;
            virtual void postDraw() override final {}
    };
