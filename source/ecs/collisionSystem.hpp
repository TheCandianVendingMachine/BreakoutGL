// collisionSystem.hpp
// Handles collisions between colliders
#pragma once
#include "collisionComponent.hpp"
#include "messaging/eventSource.hpp"
#include <plf_colony.h>
#include <robin_hood.h>

class collisionSystem : public eventSource
    {
        private:
            robin_hood::unordered_map<unsigned int, robin_hood::unordered_set<unsigned int>> m_collisionPairsThisCheck;
            robin_hood::unordered_map<unsigned int, robin_hood::unordered_set<unsigned int>> m_collisionPairsLastCheck;
            unsigned int m_currentUID = 1;

            plf::colony<collisionComponent> m_colliders;

            void collision(collisionComponent &a, collisionComponent &b);

            void testCircleCollision(collisionComponent &circle, collisionComponent &b);
            void testBoxCollision(collisionComponent &box, collisionComponent &b);

            bool circleCircleTest(collisionComponent &a, collisionComponent &b) const;
            bool boxBoxTest(collisionComponent &a, collisionComponent &b) const;
            bool circleBoxTest(collisionComponent &circle, collisionComponent &box) const;

        public:
            collisionComponent &create(collisionComponent::type type, const char *eventOnCollisionStart, const char *eventOnCollision);

            void update();
            void handleDestruction();

    };
