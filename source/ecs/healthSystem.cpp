#include "healthSystem.hpp"
#include <algorithm>

healthComponent &healthSystem::create(int maxHealth, const char *messageOnDeath)
    {
        healthComponent &health = *m_healthComponents.emplace();

        health.maxHealth = maxHealth;
        health.currentHealth = health.maxHealth;

        health.eventOnDeath = FE_STR(messageOnDeath);

        return health;
    }

void healthSystem::update()
    {
        for (auto it = m_healthComponents.begin(); it != m_healthComponents.end();)
            {
                if (it->destroy)
                    {
                        it = m_healthComponents.erase(it);
                    }
                else
                    {
                        ++it;
                    }
            }
    }

void healthSystem::damage(healthComponent &health, int amount)
    {
        health.currentHealth = std::min(std::max(health.currentHealth - amount, 0), health.maxHealth);
        if (health.currentHealth <= 0)
            {
                signal(message(health.eventOnDeath, &health));
            }
    }
