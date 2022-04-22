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
			glm::vec4 m_colour = { 1.f, 1.f, 1.f, 1.f };

			bool m_regenerateVertices = true;

			void generateVertices(glm::vec2 size);

		public:
			const glm::vec4 &colour = m_colour;
			const texture &texture = m_texture;
			
			nineBox() = default;
			nineBox(::texture texture, unsigned int chunkSize);
			nineBox(const char* texturePath, unsigned int chunkSize);

			nineBox(const nineBox &rhs);
			nineBox(nineBox &&rhs) noexcept;

			~nineBox();

			nineBox &operator=(const nineBox &rhs);
			nineBox &operator=(nineBox &&rhs) noexcept;

			void setColour(glm::vec3 colour);
			void setColour(glm::vec4 colour);

			const vertexArray &getVertexArray(glm::vec2 size);

	};
