#include "nineBox.hpp"
#include "graphics/vertex.hpp"
#include <glm/vec2.hpp>
#include <glm/common.hpp>
#include <glad/glad.h>
#include <vector>

void nineBox::generateVertices()
	{
		std::vector<vertex> vertices;
		std::vector<unsigned int> indices;

		const glm::vec2 normalChunkSize(static_cast<float>(m_chunkSize) / m_size);
		constexpr float third = 1.f / 3.f;

		const float initialX[3] = { 0.f, normalChunkSize.x, 1.f - normalChunkSize.x };
		const float initialY[3] = { 0.f, normalChunkSize.y, 1.f - normalChunkSize.y };

		const float offsetX[3] = { normalChunkSize.x, 1.f - 2.f * normalChunkSize.x, normalChunkSize.x };
		const float offsetY[3] = { normalChunkSize.y, 1.f - 2.f * normalChunkSize.y, normalChunkSize.y };

		for (int y = 0; y < 3; y++)
			{
				for (int x = 0; x < 3; x++)
					{
						vertex v0{
							.position = { initialX[x], initialY[y], 0.f},
							.textureCoordinate = { x * third, 1.f - y * third },
							.colour = m_colour
						};

						vertex v1{
							.position = { initialX[x] + offsetX[x], initialY[y], 0.f },
							.textureCoordinate = { x * third + third, 1.f - y * third },
							.colour = m_colour
						};

						vertex v2{
							.position = { initialX[x] + offsetX[x], initialY[y] + offsetY[y], 0.f},
							.textureCoordinate = { x * third + third, 1.f - (y * third + third) },
							.colour = m_colour
						};

						vertex v3{
							.position = { initialX[x], initialY[y] + offsetY[y], 0.f },
							.textureCoordinate = { x * third, 1.f - (y * third + third) },
							.colour = m_colour
						};

						unsigned int indexOffset = vertices.size();
						vertices.push_back(v0);
						vertices.push_back(v1);
						vertices.push_back(v2);
						vertices.push_back(v3);

						indices.push_back(indexOffset + 0);
						indices.push_back(indexOffset + 1);
						indices.push_back(indexOffset + 2);
						indices.push_back(indexOffset + 2);
						indices.push_back(indexOffset + 3);
						indices.push_back(indexOffset + 0);
					}
			}

		m_vertexArray.bindVertices(vertices);
		m_vertexArray.bindIndices(indices);
		m_vertexArray.use(vertex::attributes::COLOUR | vertex::attributes::POSITION | vertex::attributes::TEXTURE);

		m_regenerateVertices = false;
	}

nineBox::nineBox(texture texture, unsigned int chunkSize) :
	m_chunkSize(chunkSize),
	m_texture(texture)
	{
	}

nineBox::nineBox(const char *texturePath, unsigned int chunkSize) :
	m_texture(texturePath, false),
	m_chunkSize(chunkSize)
	{
	}

nineBox::~nineBox()
	{
	}

void nineBox::setSize(glm::vec2 size)
	{
		m_size = glm::max(size, glm::vec2(2.f * m_chunkSize));
		m_regenerateVertices = true;
	}

void nineBox::setColour(glm::vec3 colour)
	{
		m_colour = glm::vec4(colour, 1.f);
		m_regenerateVertices = true;
	}

void nineBox::setColour(glm::vec4 colour)
	{
		m_colour = colour;
		m_regenerateVertices = true;
	}

const vertexArray &nineBox::getVertexArray()
	{
		if (m_regenerateVertices)
			{
				generateVertices();
			}
		return m_vertexArray;
	}
