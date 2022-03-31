#include "healthSystem.hpp"
#include <algorithm>

healthComponent &healthSystem::create(int maxHealth, const char *messageOnDeath, const char *messageOnHit)
    {
        healthComponent &health = *m_healthComponents.emplace();

        health.maxHealth = maxHealth;
        health.currentHealth = health.maxHealth;

        health.eventOnDeath = FE_STR(messageOnDeath);
        health.eventOnHit = FE_STR(messageOnHit);

        return health;
    }

void healthSystem::handleDestruction()
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
        int oldHealth = health.currentHealth;
        health.currentHealth = std::min(std::max(health.currentHealth - amount, 0), health.maxHealth);

        signal(message(health.eventOnHit, &health, oldHealth, health.currentHealth, amount));

        if (health.currentHealth <= 0)
            {
                signal(message(health.eventOnDeath, &health));
            }
    }
