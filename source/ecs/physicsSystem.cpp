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
