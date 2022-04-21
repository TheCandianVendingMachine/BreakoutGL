#include "widgetTransform.hpp"

void widgetTransform::setPosition(glm::vec2 position, type positionType)
	{
		m_position = position;
		m_positionType = positionType;
	}

void widgetTransform::setSize(glm::vec2 size, type sizeType)
	{
		m_size = size;
		m_sizeType = sizeType;
	}

void widgetTransform::setAnchor(anchor anchor)
	{
		m_anchor = anchor;
	}
