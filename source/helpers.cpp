#include "helpers.h"

C2D_Text* makeText(const char* string)
{
    static C2D_Text c2dText;
    C2D_TextParse(&c2dText, dynamicBuf, string);
    C2D_TextOptimize(&c2dText);
    return &c2dText;
}

void drawCentered(int center, C2D_Text* text, bool atBaseline, float y, float z, float scaleX, float scaleY, u32 color)
{
    float width = 0;
    C2D_TextGetDimensions(text, scaleX, scaleY, &width, NULL);
    float offset = center - width/2;
    C2D_DrawText(text, C2D_WithColor | (atBaseline ? C2D_AtBaseline : 0), offset, y, z, scaleX, scaleY, color);
}