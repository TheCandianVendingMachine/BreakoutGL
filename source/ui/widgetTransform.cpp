#include "widgetTransform.hpp"
#include <glm/gtc/matrix_transform.hpp>

void widgetTransform::setPosition(glm::vec2 position, type positionType)
	{
		m_position = position;
		m_positionType = positionType;
		m_needsUpdate = true;
	}

void widgetTransform::setSize(glm::vec2 size, type sizeType)
	{
		m_size = size;
		m_sizeType = sizeType;
		m_needsUpdate = true;
	}

void widgetTransform::setAnchor(anchor anchor)
	{
		m_anchor = anchor;
		m_needsUpdate = true;
	}

glm::vec2 widgetTransform::getPosition(glm::vec2 windowSize) const
	{
		switch (m_positionType)
			{
				case widgetTransform::type::ABSOLUTE:
					return m_position;
					break;
				case widgetTransform::type::PERCENT:
					return m_position * windowSize;
					break;
				default:
					break;
			}
		return glm::vec2(0.f);
	}

glm::vec2 widgetTransform::getSize(glm::vec2 windowSize) const
	{
		switch (m_sizeType)
			{
				case widgetTransform::type::ABSOLUTE:
					return m_size;
					break;
				case widgetTransform::type::PERCENT:
					return m_size * windowSize;
					break;
				default:
					break;
			}
		return glm::vec2(0.f);
	}

glm::mat4 widgetTransform::getMatrix(glm::vec2 windowSize)
	{
		if (m_needsUpdate)
			{
				glm::vec2 position{};

				glm::vec2 truePosition = getPosition(windowSize);
				glm::vec2 trueSize = getSize(windowSize);

				switch (m_anchor)
					{
						case widgetTransform::anchor::TOP_LEFT:
							position = truePosition;
							break;
						case widgetTransform::anchor::TOP_MIDDLE:
							position = truePosition - glm::vec2(trueSize.x * 0.5f, 0.f);
							break;
						case widgetTransform::anchor::TOP_RIGHT:
							position = truePosition - glm::vec2(trueSize.x, 0.f);
							break;
						case widgetTransform::anchor::MIDDLE_LEFT:
							position = truePosition - glm::vec2(0.f, trueSize.y * 0.5f);
							break;
						case widgetTransform::anchor::MIDDLE:
							position = truePosition - trueSize * 0.5f;
							break;
						case widgetTransform::anchor::MIDDLE_RIGHT:
							position = truePosition - glm::vec2(trueSize.x, trueSize.y * 0.5f);
							break;
						case widgetTransform::anchor::BOTTOM_LEFT:
							position = truePosition - glm::vec2(0.f, trueSize.y);
							break;
						case widgetTransform::anchor::BOTTOM_MIDDLE:
							position = truePosition - glm::vec2(trueSize.x * 0.5f, trueSize.y);
							break;
						case widgetTransform::anchor::BOTTOM_RIGHT:
							position = truePosition - trueSize;
							break;
						default:
							break;
					}

				m_transformMatrix = glm::translate(glm::mat4(1.f), glm::vec3(position, 0.f));
				m_transformMatrix *= glm::scale(glm::mat4(1.f), glm::vec3(trueSize, 0.f));

				m_needsUpdate = false;
			}
		return m_transformMatrix;
	}


