// nineBox.hpp
// Takes in a [3n, 3n] sized texture, splits it into [n, n] chunks, and generates dynamically sized elements from chunks
#pragma once
#include "graphics/texture.hpp"
#include "graphics/vertexArray.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

class nineBox
	{
		private:
			texture m_texture;
			unsigned int m_chunkSize = 0;

			vertexArray m_vertexArray;

			glm::vec2 m_size = { 0.f, 0.f };
			glm::vec4 m_colour = { 1.f, 1.f, 1.f, 1.f };

			bool m_regenerateVertices = false;

			void generateVertices();

		public:
			const glm::vec2 &size = m_size;
			const glm::vec4 &colour = m_colour;
			
			nineBox(texture texture, unsigned int chunkSize);
			nineBox(const char* texturePath, unsigned int chunkSize);
			~nineBox();

			void setSize(glm::vec2 size);
			void setColour(glm::vec3 colour);
			void setColour(glm::vec4 colour);

			const vertexArray &getVertexArray();

	};
