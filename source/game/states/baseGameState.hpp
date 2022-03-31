// baseGameState.hpp
// The base class for all game states
#pragma once

class gameStateMachine;
class graphicsEngine;
class baseGameState
    {
        private:
            friend class gameStateMachine;

        protected:
            gameStateMachine *m_stateMachine = nullptr;

        public:
            virtual void init() {}
            virtual void deinit() {}
            virtual void onEnter() {}
            virtual void onLeave() {}

            virtual void update() {}
            virtual void fixedUpdate(float dt) {}

            virtual void preUpdate() {}
            virtual void postUpdate() {}

            virtual void preDraw() {}
            virtual void draw(graphicsEngine &graphics) {}
            virtual void postDraw() {}
    };
