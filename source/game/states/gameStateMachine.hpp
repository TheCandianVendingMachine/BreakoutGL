// gameStateMachine.hpp
// The basic state machine responsible for managing current game state
#pragma once
#include "baseGameState.hpp"
#include <vector>
#include <memory>
#include <stack>
#include <queue>
#include <concepts>

class graphicsEngine;
class gameStateMachine
    {
        private:
            std::stack<std::unique_ptr<baseGameState>> m_currentStates;
            std::queue<std::unique_ptr<baseGameState>> m_queuedStates;
            int m_popsQueued = 0;

            baseGameState *m_currentState = nullptr;

        public:
            template<std::derived_from<baseGameState> TState>
            void push();
            void pop();

            void preUpdate();
            void update();
            void postUpdate();
            void fixedUpdate(float dt);

            void preDraw();
            void draw(graphicsEngine &graphics);
            void postDraw();
    };

template<std::derived_from<baseGameState> TState>
inline void gameStateMachine::push()
    {
        baseGameState *state = &(*m_queuedStates.emplace(new TState()));
        state->m_stateMachine = this;
    }
