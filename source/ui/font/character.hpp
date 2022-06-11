// character.hpp
// defines information for a specific character within a font
#pragma once
#include <glm/vec2.hpp>

struct character
    {
        // reference for all glyph metrics:
        // https://freetype.org/freetype2/docs/glyphs/metrics.png
        glm::ivec2 offset = { 0, 0 };   // the offset of the character in the packed texture
        glm::ivec2 size = { 0, 0 };     // the actual size of the character
        glm::ivec2 bearing = { 0, 0 };  // offset from origin for the top-left of the character
        glm::ivec2 advance = { 0, 0 };  // how much spacing is put after this character
    };
