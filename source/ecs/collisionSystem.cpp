#include "collisionSystem.hpp"
#include <glm/glm.hpp>

void collisionSystem::collision(collisionComponent &a, collisionComponent &b)
    {
        bool firstCollision = m_collisionPairsLastCheck.find(a.uid) == m_collisionPairsLastCheck.end() || m_collisionPairsLastCheck.at(a.uid).find(b.uid) == m_collisionPairsLastCheck.at(a.uid).end();

        // signalling once because we iterate through all colliders and check all pairs
        // change if better collision handling implemented
        if (firstCollision)
            {
                signal(message(a.eventOnCollisionStart, &a, &b));
            }

        signal(message(a.eventOnCollision, &a, &b));

        m_collisionPairsThisCheck[a.uid].insert(b.uid);
        m_collisionPairsThisCheck[b.uid].insert(a.uid);
    }

void collisionSystem::testCircleCollision(collisionComponent &circle, collisionComponent &b)
    {
        switch (b.shape)
            {
                case collisionComponent::type::CIRCLE:
                    if (circleCircleTest(circle, b))
                        {
                            collision(circle, b);
                        }
                    break;
                case collisionComponent::type::BOX:
                    if (circleBoxTest(circle, b))
                        {
                            collision(circle, b);
                        }
                    break;
                default:
                    break;
            }
    }

void collisionSystem::testBoxCollision(collisionComponent &box, collisionComponent &b)
    {
        switch (b.shape)
            {
                case collisionComponent::type::CIRCLE:
                    if (circleBoxTest(b, box))
                        {
                            collision(box, b);
                        }
                    break;
                case collisionComponent::type::BOX:
                    if (boxBoxTest(box, b))
                        {
                            collision(box, b);
                        }
                    break;
                default:
                    break;
            }
    }

bool collisionSystem::circleCircleTest(collisionComponent &a, collisionComponent &b) const
    {
        float distance = glm::length(a.position - b.position);
        return distance <= (a.collider.circle.radius + b.collider.circle.radius);
    }

bool collisionSystem::boxBoxTest(collisionComponent &a, collisionComponent &b) const
    {
        return  !((a.position.x + a.collider.box.extents.x < b.position.x || a.position.x > b.position.x + b.collider.box.extents.x) ||
                  (a.position.y + a.collider.box.extents.y < b.position.y || a.position.y > b.position.y + b.collider.box.extents.y));
    }

bool collisionSystem::circleBoxTest(collisionComponent &circle, collisionComponent &box) const
    {
        glm::vec2 min = box.position;
        glm::vec2 max = min + box.collider.box.extents;

        glm::vec2 closestPoint = glm::min(glm::max(circle.position, min), max);

        glm::vec2 v = closestPoint - circle.position;
        return glm::dot(v, v) <= (circle.collider.circle.radius * circle.collider.circle.radius);
    }

collisionComponent &collisionSystem::create(collisionComponent::type type, const char *eventOnCollisionStart, const char *eventOnCollision)
    {
        collisionComponent &collider = *m_colliders.emplace();
        collider.uid = m_currentUID++;
        collider.shape = type;
        collider.eventOnCollisionStart = FE_STR(eventOnCollisionStart);
        collider.eventOnCollision = FE_STR(eventOnCollision);

        m_collisionPairsLastCheck[collider.uid];

        switch (type)
            {
                case collisionComponent::type::CIRCLE:
                    break;
                case collisionComponent::type::BOX:
                    break;
                default:
                    break;
            }

        return collider;
    }

void collisionSystem::update()
    {
        for (auto &colliderA : m_colliders)
            {
                for (auto &colliderB : m_colliders)
                    {
                        if (&colliderA == &colliderB)
                            {
                                continue;
                            }

                        switch (colliderA.shape)
                            {
                                case collisionComponent::type::CIRCLE:
                                    testCircleCollision(colliderA, colliderB);
                                    break;
                                case collisionComponent::type::BOX:
                                    testBoxCollision(colliderA, colliderB);
                                    break;
                                default:
                                    break;
                            }
                    }
            }

        m_collisionPairsLastCheck = m_collisionPairsThisCheck;
        m_collisionPairsThisCheck.clear();
    }

void collisionSystem::handleDestruction()
    {
    }

