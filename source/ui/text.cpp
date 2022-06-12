#include "text.hpp"
#include <glm/common.hpp>

text &text::operator=(const text &rhs)
    {
        if (this != &rhs)
            {
                m_string = rhs.m_string;
                m_font = rhs.m_font;
                m_textVertexArray = rhs.m_textVertexArray;
            }
        return *this;
    }

text::text(fontReference font) :
    m_font(font)
    {
    }

text::text(const char *text, fontReference font) :
    m_font(font)
    {
        setText(text);
    }

void text::setText(const char *text)
    {
        m_string = text;
        m_needsRebuild = true;
    }

void text::build(glm::vec2 size)
    {
        // parse through our string and render each character
        std::vector<vertex> vertices;
        std::vector<unsigned int> indices;

        glm::vec2 normalModifier = 12.f / size;

        glm::vec2 min = {};
        glm::vec2 max = {};

        glm::vec2 pen = { 0, 0 };
        for (auto c : m_string)
            {
                if (c == '\n') { continue; } // todo
                ::font &f = m_font.font();
                character info = f.characters[c];

                glm::vec2 drawPos = pen + glm::vec2(info.bearing.x, -info.bearing.y);
                glm::vec2 charTextureOffset = f.characterOffsets[c];
                glm::vec2 charTextureSize = f.characterSizes[c];

                min = glm::min(min, pen - glm::vec2(0.f, info.bearing.y));
                max = glm::max(max, pen + glm::vec2(info.advance) + glm::vec2(0.f, info.size.y - drawPos.y));

                pen += info.advance;

                //drawPos *= normalModifier;
                glm::vec2 size = glm::vec2(info.size);// * normalModifier;

                vertex v0{
                    .position = { drawPos.x, drawPos.y, 0.f},
                    .textureCoordinate = { charTextureOffset.x, charTextureOffset.y },
                    .colour = m_colour
                };

                vertex v1{
                    .position = { drawPos.x + size.x, drawPos.y, 0.f },
                    .textureCoordinate = { charTextureOffset.x + charTextureSize.x, charTextureOffset.y },
                    .colour = m_colour
                };

                vertex v2{
                    .position = { drawPos.x + size.x, drawPos.y + size.y, 0.f},
                    .textureCoordinate = { charTextureOffset.x + charTextureSize.x, charTextureOffset.y + charTextureSize.y },
                    .colour = m_colour
                };

                vertex v3{
                    .position = { drawPos.x, drawPos.y + size.y, 0.f },
                    .textureCoordinate = { charTextureOffset.x, charTextureOffset.y + charTextureSize.y },
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

        m_textVertexArray.bindVertices(vertices);
        m_textVertexArray.bindIndices(indices);
        m_textVertexArray.use(vertex::attributes::COLOUR | vertex::attributes::POSITION | vertex::attributes::TEXTURE);

        m_size = max - min;

        m_needsRebuild = false;
    }

void text::setFont(fontReference font)
    {
        m_font = font;
        m_needsRebuild = true;
    }
