// healthComponent.hpp
// Contains information about the health of an entity
#pragma once
#include "component.hpp"

struct healthComponent : component
    {
        virtual const char *name() const override final { return "health"; }

        int maxHealth = 0;
        int currentHealth = 0;

        fe::str eventOnDeath = 0;
        fe::str eventOnHit = 0;
    };
