#pragma once

#include <memory>
#include <algorithm>

#include <vector>
#include <array>

#include <cstdlib>
#include <cstdio>

#include <3ds.h>
#include <citro3d.h>
#include <citro2d.h>

// #define NODEBUG
#ifdef NODEBUG
#define DEBUG(...)  
#else
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#endif

#include "sprites.h"

#define GRID_WIDTH 10
#define GRID_HEIGHT 10

#define SHIP_PART_SIZE 20

extern bool running;
extern bool ndspInited;

extern C2D_SpriteSheet spritesheet;

extern C3D_RenderTarget *top, *bottom;
extern C2D_TextBuf staticBuf, dynamicBuf;

extern u32 kDown, kHeld, kUp;
extern touchPosition oldTouch, touch;

extern C2D_ImageTint damagedTint, hitCursorTint, enemyHitCursorTint;

static constexpr u32 whiteColor = C2D_Color32f(1, 1, 1, 1);
static constexpr u32 blackColor = C2D_Color32f(0, 0, 0, 1);
static constexpr u32 backgroundColor = C2D_Color32(55, 122, 168, 255);
static constexpr u32 damagedColor = C2D_Color32(229, 66, 66, 255);
static constexpr u32 readyColor = C2D_Color32(0, 255, 33, 255);
static constexpr u32 cursorColor = C2D_Color32(239, 220, 11, 255);
static constexpr u32 darkGrey = C2D_Color32(64, 64, 64, 255);
static constexpr u32 lightGrey = C2D_Color32(128, 128, 128, 255);
static constexpr u32 textColor = whiteColor;
static constexpr u32 selectedTextColor = blackColor;

static constexpr float textScale = 0.6f;

#include "helpers.h"
