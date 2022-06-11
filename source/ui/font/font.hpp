// font.hpp
// meta information regarding fonts. Used to create text
#pragma once
#include <vector>
#include "character.hpp"
#include "stb_rect_pack.h"

struct font
    {
        std::vector<character> characters;
    };


