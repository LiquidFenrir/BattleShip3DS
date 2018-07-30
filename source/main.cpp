#include <memory>
#include "game/game.h"

bool running = true;
bool ndspInited = false;

C2D_SpriteSheet spritesheet;

C3D_RenderTarget *top, *bottom;
C2D_TextBuf staticBuf, dynamicBuf;

u32 kDown, kHeld, kUp;
touchPosition oldTouch, touch;

int main(int argc, char* argv[])
{
    consoleDebugInit(debugDevice_SVC);
    auto game = std::make_unique<Game>(argc, argv);

    while(aptMainLoop() && running)
        game->update();

    return 0;
}
