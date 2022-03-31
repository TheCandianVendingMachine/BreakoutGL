#include "transformable.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

glm::mat4 transformable::transform() const
    {
        glm::mat4 transform(1.f);
        transform = glm::translate(transform, glm::vec3(position, 0.f));
        transform *= glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 0.f, 1.f));
        transform = glm::scale(transform, glm::vec3(scale, 0.f));

        return transform;
    }
