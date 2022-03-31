#include "gameStateMachine.hpp"

void gameStateMachine::pop()
    {
        m_popsQueued++;
    }

void gameStateMachine::preUpdate()
    {
        while (m_popsQueued > 0)
            {
                m_currentState = nullptr;

                m_currentStates.top()->onLeave();
                m_currentStates.top()->deinit();
                m_currentStates.pop();
                if (!m_currentStates.empty()) 
                    {
                        m_currentStates.top()->onEnter();
                    }
                m_popsQueued--;
            }

        while (!m_queuedStates.empty())
            {
                m_currentStates.push(std::move(m_queuedStates.front()));
                m_currentStates.top()->init();
                m_currentStates.top()->onEnter();
                m_queuedStates.pop();
            }

        if (!m_currentStates.empty())
            {
                m_currentState = m_currentStates.top().get();
                m_currentState->preUpdate();
            }
        else
            {
                m_currentState = nullptr;
            }
    }

void gameStateMachine::update()
    {
        if (m_currentState)
            {
                m_currentState->update();
            }
    }

void gameStateMachine::postUpdate()
    {
        if (m_currentState)
            {
                m_currentState->postUpdate();
            }
    }

void gameStateMachine::fixedUpdate(float dt)
    {
        if (m_currentState)
            {
                m_currentState->fixedUpdate(dt);
            }
    }

void gameStateMachine::preDraw()
    {
        if (m_currentState)
            {
                m_currentState->preDraw();
            }
    }

void gameStateMachine::draw(graphicsEngine &graphics)
    {
        if (m_currentState)
            {
                m_currentState->draw(graphics);
            }
    }

void gameStateMachine::postDraw()
    {
        if (m_currentState)
            {
                m_currentState->postDraw();
            }
    }

