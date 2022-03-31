#include "physicsSystem.hpp"

physicsComponent &physicsSystem::create()
    {   
        return *m_physicsComponents.emplace();
    }

void physicsSystem::update(float dt)
    {
        for (auto &physics : m_physicsComponents)
            {
                physics.position += physics.velocity * dt;
            }
    }

void physicsSystem::handleDestruction()
    {
        for (auto it = m_physicsComponents.begin(); it != m_physicsComponents.end())
            {
                if (it->destroy)
                    {
                        it = m_physicsComponents.erase(it);
                    }
                else
                    {
                        it++;
                    }
            }
    }
