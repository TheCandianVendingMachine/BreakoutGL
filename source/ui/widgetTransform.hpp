// widgetTransform.hpp
// The transformation for a widget. A position can be defined in window coords, percent, etc. and this class manages it
// Also handles the scale of the widget
#pragma once
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include "widgetTransformEnums.hpp"
#undef ABSOLUTE

class widgetTransform
    {
        private:
            widgetTransformEnums::anchor m_anchor = widgetTransformEnums::anchor::MIDDLE;

            widgetTransformEnums::type m_sizeType = widgetTransformEnums::type::ABSOLUTE;
            glm::vec2 m_size = { 0, 0 };

            widgetTransformEnums::type m_positionType = widgetTransformEnums::type::ABSOLUTE;
            glm::vec2 m_position = { 0, 0 };

            glm::mat4 m_transformMatrix;

            bool m_needsUpdate = false;

        public:
            void setPosition(glm::vec2 position, widgetTransformEnums::type positionType);
            void setSize(glm::vec2 size, widgetTransformEnums::type sizeType);
            void setAnchor(widgetTransformEnums::anchor anchor);

            glm::vec2 getPosition(glm::vec2 windowSize) const;
            glm::vec2 getSize(glm::vec2 windowSize) const;

            glm::mat4 getMatrix(glm::vec2 windowSize);

    };
