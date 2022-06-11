#include "fontEngine.hpp"
#include "graphics/image.hpp"
#include <spdlog/spdlog.h>
#include <glm/common.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

FT_Face fontEngine::loadFontFile(const char *font)
    {
        if (m_loadedFreeTypeFonts.find(font) != m_loadedFreeTypeFonts.end())
            {
                return m_loadedFreeTypeFonts.at(font);
            }

        FT_Face newFace;
        if (FT_New_Face(m_library, font, 0, &newFace))
            {
                spdlog::error("Failed to load font file {}", font);
                return nullptr;
            }

        m_loadedFreeTypeFonts.insert({ font, newFace });
        return newFace;
    }

fontEngine::fontEngine()
    {
        if (FT_Init_FreeType(&m_library))
            {
                spdlog::error("Failed to load FreeType");
            }
    }

fontEngine::fontReference fontEngine::load(const char *font, const char *name)
    {
        std::vector<characterType> glyphs(128);
        for (char c = 0; c < 127; c++)
            {
                glyphs[c] = c;
            }

        return load(font, name, glyphs);
    }

fontEngine::fontReference fontEngine::load(const char *font, const char *name, const std::vector<characterType> &glyphs)
    {
        FT_Face face = loadFontFile(font);
        FT_Set_Pixel_Sizes(face, 0, 48);

        ::font newFont;
        newFont.characters.resize(glyphs.size());

        glm::ivec2 maxSize{0, 0};
        glm::ivec2 averageSize{0, 0};

        std::vector<stbrp_rect> renderedChars;

        for (auto c : glyphs)
            {
                if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                    {
                        spdlog::warn("Failed to load glyph '{}'", c);
                        continue;
                    }

                character newChar;
                newChar.advance = glm::ivec2(face->glyph->advance.x, face->glyph->advance.y);
                newChar.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
                newChar.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);

                stbrp_rect renderedCharData;
                renderedCharData.id = c;
                renderedCharData.w = newChar.size.x;
                renderedCharData.h = newChar.size.y;

                renderedChars.push_back(renderedCharData);

                maxSize = glm::max(maxSize, newChar.size);
                averageSize += newChar.size;

                newFont.characters[c] = newChar;
            }

        // generate bitmaps and pack into VBO
        std::vector<stbrp_node> packedNodes;

        averageSize /= static_cast<int>(glyphs.size());
        glm::ivec2 contextSize = glm::ivec2(std::max(averageSize.x, averageSize.y)) * static_cast<int>(std::ceil(std::sqrt(static_cast<double>(glyphs.size()))));

        packedNodes.resize(contextSize.x * 2);

        stbrp_context context;
        stbrp_init_target(&context, contextSize.x, contextSize.y, packedNodes.data(), packedNodes.size());
        int isPacked = stbrp_pack_rects(&context, renderedChars.data(), renderedChars.size());

        packedFontTexture textureData;
        if (!isPacked)
            {
                spdlog::error("Failed to pack {} ({}) into rectangle", font, name);
            }
        else
            {
                image packedFont(contextSize.x, contextSize.y, 4);
                packedFont.clear(0, 0, 0, 0);

                for (auto &packed : renderedChars)
                    {
                        characterType c = packed.id;
                        textureData.characterOffsets[c] = { packed.x, packed.y };

                        FT_UInt glyphIndex = FT_Get_Char_Index(face, c);

                        FT_Error error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_MONOCHROME);
                        if (error) 
                            {
                                spdlog::warn("Failed to load glyph {}", packed.id);
                                continue;
                            }

                        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
                        if (error) 
                            {
                                spdlog::warn("Failed to render glyph {}", packed.id);
                                continue;
                            }

                        FT_Bitmap bitmap = face->glyph->bitmap;
                        unsigned char *texel = bitmap.buffer;
                        if (texel)
                            {
                                for (int y = 0; y < bitmap.rows; y++)
                                    {
                                        for (int x = 0; x < bitmap.width; x++)
                                            {
                                                unsigned char colour = *(texel + x + y * bitmap.pitch);
                                                bool isTexel = colour != 0;

                                                if (isTexel)
                                                    {
                                                        packedFont.set(packed.x + x, packed.y + y, colour, colour, colour, 255);
                                                    }
                                                else
                                                    {
                                                        packedFont.set(packed.x + x, packed.y + y, 0, 0, 0, 0);
                                                    }
                                            }
                                    }
                            }
                    }

                textureData.texture.loadFromMemory(reinterpret_cast<const unsigned char*>(packedFont.getPixelData().data()), packedFont.getBufferSize(), false);
            }

        m_fontIndexLookup[m_lastUID] = m_allFonts.size();
        m_allFonts.push_back(std::move(newFont));
        m_allFontTextureData.push_back(textureData);

        m_fontNameLookup[name] = m_lastUID;

        return fontReference(this, m_lastUID++);
    }

fontEngine::fontReference fontEngine::get(const char *name)
    {
        return fontReference(this, m_fontNameLookup.at(name));
    }
