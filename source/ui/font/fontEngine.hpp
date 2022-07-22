// fontEngine.hpp
// Handles the loading, packing, and creating of fonts
#pragma once
#include "font.hpp"
#include "graphics/texture.hpp"
#include "fontReference.hpp"
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
        private:
            friend class fontReference;

            FT_Library m_library = nullptr;

            fontReference::fontID m_lastUID = 1;

            robin_hood::unordered_map<std::string, fontReference::fontID> m_fontNameLookup;
            robin_hood::unordered_map<fontReference::fontID, std::string> m_fontIDLookup;
            robin_hood::unordered_map<fontReference::fontID, unsigned int> m_fontIndexLookup;
            std::vector<font> m_allFonts;

            robin_hood::unordered_map<std::string, FT_Face> m_loadedFreeTypeFonts;

            FT_Face loadFontFile(const char *font);

        public:
            fontEngine();

            fontReference load(const char *font, const char *name, unsigned int pixelHeight);
            fontReference load(const char *font, const char *name, const std::vector<font::characterType> &glyphs, unsigned int pixelHeight);

            fontReference get(const char *name);
            bool exists(const char *name) const;
    };
