// font.hpp
// meta information regarding fonts. Used to create text
#pragma once
#include <robin_hood.h>
#include "character.hpp"
#include "graphics/texture.hpp"
#include "stb_rect_pack.h"
#include "fontSizeType.hpp"

struct font
    {
        using characterType = char;

        robin_hood::unordered_map<characterType, character> characters;
        robin_hood::unordered_map<characterType, glm::ivec2> characterOffsets;
        texture texture;
        fontSizeType sizeType;
        float size = 0.f;
    };


