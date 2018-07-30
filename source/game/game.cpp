#include "game/game.h"

#include <mpg123.h>

#include "states/roomlist.h"

void deleteAllStates(State* state)
{
    if(state)
    {
        deleteAllStates(state->nextState);  // Won't do anything if it's a null pointer
        delete state;
    }
}

Game::Game(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    DEBUG("Game::Game\n");

    APT_GetAppCpuTimeLimit(&this->old_time_limit);
    APT_SetAppCpuTimeLimit(30);

    romfsInit();
    udsInit(0x3000, NULL);
    ndspInited = R_SUCCEEDED(ndspInit());
    if(ndspInited)
    {
        int initerror = MPG123_OK;
        initerror = mpg123_init();
        if(initerror != MPG123_OK)
        {
            DEBUG("mpg123_init error: %i, %s\n", initerror, mpg123_plain_strerror(initerror));
            ndspExit();
            ndspInited = false;
        }
    }
    DEBUG("ndsp inited: %s\n", ndspInited ? "yes" : "no");

    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    bottom = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    spritesheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");

    staticBuf = C2D_TextBufNew(4096);
    dynamicBuf = C2D_TextBufNew(4096);

    C2D_PlainImageTint(&damagedTint, damagedColor, 0.75f);
    C2D_PlainImageTint(&hitCursorTint, cursorColor, 1.0f);
    C2D_PlainImageTint(&enemyHitCursorTint, damagedColor, 1.0f);

    this->state = new RoomSelectionState;
}

Game::~Game()
{
    DEBUG("Game::~Game\n");

    deleteAllStates(this->state);

    C2D_TextBufDelete(dynamicBuf);
    C2D_TextBufDelete(staticBuf);

    C2D_SpriteSheetFree(spritesheet);

    C2D_Fini();
    C3D_Fini();
    gfxExit();

    if(ndspInited)
    {
        mpg123_exit();
        ndspExit();
    }
    udsExit();
    romfsExit();

    if(this->old_time_limit != UINT32_MAX)
    {
        APT_SetAppCpuTimeLimit(this->old_time_limit);
    }
}

void Game::update()
{   
    hidScanInput();

    kDown = hidKeysDown();
    kHeld = hidKeysHeld();
    kUp = hidKeysUp();

    hidTouchRead(&touch);

    if(kDown & KEY_START)
    {
        running = false;
        return;
    }

    if(this->state)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

        C2D_TextBufClear(dynamicBuf);

        C2D_SceneBegin(top);
        C2D_TargetClear(top, backgroundColor);

        this->state->drawTop();

        C2D_SceneBegin(bottom);
        C2D_TargetClear(bottom, backgroundColor);

        this->state->drawBottom();

        C3D_FrameEnd(0);

        this->state->update();

        State* nextState = this->state->nextState;
        if(nextState)
        {
            DEBUG("nextState: %p\n", nextState);
            delete this->state;
            this->state = nextState;
        }
    }

    oldTouch = touch;
}
