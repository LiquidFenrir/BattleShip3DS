#pragma once

#include "common.h"

C2D_Text* makeText(const char* string);
inline C2D_Text* makeText(const std::string& string)
{
    return makeText(string.c_str());
}

void drawCentered(int center, C2D_Text* text, bool atBaseline, float y, float z, float scaleX, float scaleY, u32 color);
