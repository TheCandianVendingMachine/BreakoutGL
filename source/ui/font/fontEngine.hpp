// fontEngine.hpp
// Handles the loading, packing, and creating of fonts
#pragma once
#include "font.hpp"
#include "graphics/texture.hpp"
#include <vector>
#include <robin_hood.h>
#include <string>

// forward FT defines
typedef struct FT_LibraryRec_   *FT_Library;
typedef struct FT_FaceRec_      *FT_Face;

/*
    Todo:
        - Add a "Text" class that creates a VBO with a string of whatever length with appropiate textures
        - Add a way to render all text in one pass
*/

class fontEngine
    {
        public:
            using fontID = int;
            using characterType = char;

            struct fontReference
                {
                    private:
                        fontEngine *m_engineReference = nullptr;
                        fontID m_fontReference = 0;

                        fontReference(fontEngine *engine, fontID font) :
                            m_engineReference(engine),
                            m_fontReference(font)
                        {
                        }

                        friend class fontEngine;

                    public:
                        font &font() const
                            {
                                return m_engineReference->m_allFonts.at(m_engineReference->m_fontIndexLookup.at(m_fontReference));
                            }

                        operator ::font&() const
                            {
                                return font();
                            }
                };

        private:
            friend struct fontReference;

            struct packedFontTexture
                {
                    texture texture;
                    robin_hood::unordered_map<characterType, glm::ivec2> characterOffsets; // the offsets of each character in the packed texture
                };

            FT_Library m_library = nullptr;

            fontID m_lastUID = 1;

            robin_hood::unordered_map<std::string, fontID> m_fontNameLookup;
            robin_hood::unordered_map<fontID, unsigned int> m_fontIndexLookup;
            std::vector<font> m_allFonts;
            std::vector<packedFontTexture> m_allFontTextureData;

            robin_hood::unordered_map<std::string, FT_Face> m_loadedFreeTypeFonts;

            FT_Face loadFontFile(const char *font);

        public:
            fontEngine();

            fontReference load(const char *font, const char *name);
            fontReference load(const char *font, const char *name, const std::vector<characterType> &glyphs);

            fontReference get(const char *name);
    };
