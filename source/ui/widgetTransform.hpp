// widgetTransform.hpp
// The transformation for a widget. A position can be defined in window coords, percent, etc. and this class manages it
// Also handles the scale of the widget
#pragma once
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#undef ABSOLUTE

class widgetTransform
	{
		public:
			enum class type
				{
					ABSOLUTE,	// Absolute position. (0, 0) is top left of window, and position is defined as an offset from this position
					PERCENT		// Percent position. (1, 1) is the bottom right corner of window, (0, 0) is top left corner.
				};

			enum class anchor
				{
					TOP_LEFT,
					TOP_MIDDLE,
					TOP_RIGHT,
					MIDDLE_LEFT,
					MIDDLE,
					MIDDLE_RIGHT,
					BOTTOM_LEFT,
					BOTTOM_MIDDLE,
					BOTTOM_RIGHT
				};

		private:
			anchor m_anchor = anchor::MIDDLE;

			type m_sizeType = type::ABSOLUTE;
			glm::vec2 m_size = { 0, 0 };

			type m_positionType = type::ABSOLUTE;
			glm::vec2 m_position = { 0, 0 };

			glm::mat4 m_transformMatrix;

			bool m_needsUpdate = false;

		public:
			void setPosition(glm::vec2 position, type positionType);
			void setSize(glm::vec2 size, type sizeType);
			void setAnchor(anchor anchor);

			glm::vec2 getPosition(glm::vec2 windowSize) const;
			glm::vec2 getSize(glm::vec2 windowSize) const;

			glm::mat4 getMatrix(glm::vec2 windowSize);

	};
