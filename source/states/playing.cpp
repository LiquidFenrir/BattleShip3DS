#include "states/playing.h"

#include "game/room.h"
#include "game/grid.h"

C2D_ImageTint hitCursorTint, enemyHitCursorTint;

PlayingState::PlayingState(bool firstPlayer)
{
    DEBUG("PlayingState::PlayingState\n");
    this->firstPlayer = firstPlayer;
    this->enemyReady = false;
    this->enemyJoined = false;
    initGrids();
    this->state = PLAYING_PLACING_SHIPS;
    this->hoveringShip = SHIPS_AMOUNT;
    this->toHover = SHIP_CARRIER;
    this->lastHitX = -1;
    this->lastHitY = -1;
    this->remainingDecoys = 5;
    this->remainingSonars = 5;
    this->swapGrids = false;

    for(auto& soundEffect : this->soundEffects)
        soundEffect = nullptr;
}

PlayingState::~PlayingState()
{
    DEBUG("PlayingState::~PlayingState\n");
}

void PlayingState::drawTop()
{
    if(this->state == PLAYING_ENEMY_LEFT)
    {
        drawCentered(200, makeText("Enemy left."), true, 120, 0.6f, 1.0f, 1.0f, darkGrey);
        drawCentered(200, makeText("Press START to quit."), false, 120, 0.6f, 1.0f, 1.0f, darkGrey);
    }
    else if(this->state == PLAYING_YOU_WON)
    {
        drawCentered(200, makeText("You won!"), true, 120, 0.6f, 1.0f, 1.0f, darkGrey);
        drawCentered(200, makeText("Press START to quit."), false, 120, 0.6f, 1.0f, 1.0f, darkGrey);
    }
    else if(this->state == PLAYING_ENEMY_WON)
    {
        drawCentered(200, makeText("You lost!"), true, 120, 0.6f, 1.0f, 1.0f, darkGrey);
        drawCentered(200, makeText("Press START to quit."), false, 120, 0.6f, 1.0f, 1.0f, darkGrey);
    }
    else
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_water_with_grid_idx), 80.0f, 0.0f, 0.5f);
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_hud_idx), 0.0f, 0.0f, 0.55f);

        drawEnemyShips(this->enemyShips);
        for(const auto& ship : this->ships)
            ship.drawList();

        if(this->swapGrids)
        {
            this->drawGrid(GFX_TOP);
        }
        else
        {
            this->drawEnemyGrid(GFX_TOP);
        }
    }
}

void PlayingState::drawBottom()
{
    if(this->state == PLAYING_ENEMY_LEFT)
    {

    }
    else if(this->state == PLAYING_YOU_WON)
    {

    }
    else if(this->state == PLAYING_ENEMY_WON)
    {

    }
    else
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_water_with_grid_idx), 40.0f, 0.0f, 0.5f);
        this->drawButtons();

        if(this->swapGrids)
        {
            this->drawEnemyGrid(GFX_BOTTOM);
        }
        else
        {
            this->drawGrid(GFX_BOTTOM);
        }

        if(this->state == PLAYING_PLACING_SHIPS)
        {
            drawCurrentShip(this->toHover);
            if(this->hoveringShip != SHIPS_AMOUNT)
            {
                if(kHeld & KEY_TOUCH)
                    drawShipHoverTouch(this->hoveringShip, this->hoverRotation, touch.px, touch.py);
                else
                    drawShipHover(this->hoveringShip, this->hoverRotation, this->squareX, this->squareY);
            }
        }
        else if(this->state == PLAYING_YOUR_TURN)
        {
            C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_cursor_idx), 40 + SHIP_PART_SIZE + (this->squareX*SHIP_PART_SIZE), SHIP_PART_SIZE + (this->squareY*SHIP_PART_SIZE), 0.65f, &hitCursorTint);
        }
    }
}

void PlayingState::drawButtons()
{
    if(this->state == PLAYING_PLACING_SHIPS)
    {
        C2D_DrawRectSolid(0.0f, 0.0f, 0.5f, 40.0f, 40.0f, darkGrey);
        C2D_DrawRectSolid(2.0f, 2.0f, 0.55f, 36.0f, 36.0f, lightGrey);
        drawCentered(20, makeText("\uE004"), false, 6, 0.6f, 0.5f, 0.5f, darkGrey);
        drawCentered(20, makeText("Prev"), true, 34, 0.6f, 0.5f, 0.5f, darkGrey);

        C2D_DrawRectSolid(0.0f, 200.0f, 0.5f, 40.0f, 40.0f, darkGrey);
        C2D_DrawRectSolid(2.0f, 202.0f, 0.55f, 36.0f, 36.0f, lightGrey);
        drawCentered(20, makeText("\uE005"), false, 206, 0.6f, 0.5f, 0.5f, darkGrey);
        drawCentered(20, makeText("Next"), true, 234, 0.6f, 0.5f, 0.5f, darkGrey);

        bool allShipsReady = this->allShipsReady();
        u32 backColor = allShipsReady ? darkGrey : lightGrey;
        u32 frontColor = allShipsReady ? lightGrey : darkGrey;
        C2D_DrawRectSolid(280.0f, 120.0f, 0.5f, 40.0f, 120.0f, backColor);
        C2D_DrawRectSolid(282.0f, 122.0f, 0.55f, 36.0f, 116.0f, frontColor);
        drawCentered(300, makeText("\uE002"), false, 140.0f, 0.6f, 0.5f, 0.5f, backColor);
        drawCentered(300, makeText("Reafy"), true, 220.0f, 0.6f, 0.5f, 0.5f, backColor);

        u32 color = this->enemyReady ? readyColor : damagedColor;
        drawCentered(300, makeText("Foe"), false, 20.0f, 0.6f, 0.5f, 0.5f, color);
        if(!this->enemyReady)
            drawCentered(300, makeText("Not"), false, 50.0f, 0.6f, 0.5f, 0.5f, color);
        drawCentered(300, makeText("Ready"), false, 80.0f, 0.6f, 0.5f, 0.5f, color);
    }
    else if(this->state == PLAYING_WAITING_FOR_ENEMY_PLACING)
    {
        C2D_DrawRectSolid(280.0f, 120.0f, 0.5f, 40.0f, 120.0f, darkGrey);
        C2D_DrawRectSolid(282.0f, 122.0f, 0.55f, 36.0f, 116.0f, lightGrey);
        drawCentered(300, makeText("\uE001"), false, 140.0f, 0.6f, 0.5f, 0.5f, darkGrey);
        drawCentered(300, makeText("Back"), true, 220.0f, 0.6f, 0.5f, 0.5f, darkGrey);

        u32 color = this->enemyReady ? readyColor : damagedColor;
        drawCentered(300, makeText("Foe"), false, 20.0f, 0.6f, 0.5f, 0.5f, color);
        if(!this->enemyReady)
            drawCentered(300, makeText("Not"), false, 50.0f, 0.6f, 0.5f, 0.5f, color);
        drawCentered(300, makeText("Ready"), false, 80.0f, 0.6f, 0.5f, 0.5f, color);
    }
    else if(this->state == PLAYING_YOUR_TURN)
    {
        u32 decoyBackColor = this->canDecoy() ? darkGrey : lightGrey;
        u32 decoyFrontColor = this->canDecoy() ? lightGrey : darkGrey;
        C2D_DrawRectSolid(0.0f, 0.0f, 0.5f, 40.0f, 120.0f, decoyBackColor);
        C2D_DrawRectSolid(2.0f, 2.0f, 0.55f, 36.0f, 116.0f, decoyFrontColor);
        drawCentered(20, makeText("\uE002"), false, 20.0f, 0.6f, 0.5f, 0.5f, decoyBackColor);
        drawCentered(20, makeText("Decoy"), true, 100.0f, 0.6f, 0.5f, 0.5f, decoyBackColor);

        u32 sonarBackColor = this->canSonar() ? darkGrey : lightGrey;
        u32 sonarFrontColor = this->canSonar() ? lightGrey : darkGrey;
        C2D_DrawRectSolid(0.0f, 120.0f, 0.5f, 40.0f, 120.0f, sonarBackColor);
        C2D_DrawRectSolid(2.0f, 122.0f, 0.55f, 36.0f, 116.0f, sonarFrontColor);
        drawCentered(20, makeText("\uE003"), false, 140.0f, 0.6f, 0.5f, 0.5f, sonarBackColor);
        drawCentered(20, makeText("Sonar"), true, 220.0f, 0.6f, 0.5f, 0.5f, sonarBackColor);

        C2D_DrawRectSolid(280.0f, 0.0f, 0.5f, 40.0f, 120.0f, darkGrey);
        C2D_DrawRectSolid(282.0f, 2.0f, 0.55f, 36.0f, 116.0f, lightGrey);
        drawCentered(300, makeText("\uE001"), false, 20.0f, 0.6f, 0.5f, 0.5f, darkGrey);
        drawCentered(300, makeText("Swap"), true, 100.0f, 0.6f, 0.5f, 0.5f, darkGrey);

        u32 shootBackColor = this->canShoot() ? darkGrey : lightGrey;
        u32 shootFrontColor = this->canShoot() ? lightGrey : darkGrey;
        C2D_DrawRectSolid(280.0f, 120.0f, 0.5f, 40.0f, 120.0f, shootBackColor);
        C2D_DrawRectSolid(282.0f, 122.0f, 0.55f, 36.0f, 116.0f, shootFrontColor);
        drawCentered(300, makeText("\uE000"), false, 140.0f, 0.6f, 0.5f, 0.5f, shootBackColor);
        drawCentered(300, makeText("Shoot"), true, 220.0f, 0.6f, 0.5f, 0.5f, shootBackColor);
    }
}

void PlayingState::drawGrid(gfxScreen_t screen)
{
    for(int x = 0; x < GRID_WIDTH; x++)
    {
        for(int y = 0; y < GRID_HEIGHT; y++)
        {
            if(enemySonarGrid[x][y])
                C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_sonar_circle_idx), (screen == GFX_TOP ? 80 : 40) + (x*SHIP_PART_SIZE), (y*SHIP_PART_SIZE), 0.6f);
            if(shipsGrid[x][y] == SHIP_DECOY)
            {
                C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_decoy_idx), (screen == GFX_TOP ? 80 : 40) + SHIP_PART_SIZE + (x*SHIP_PART_SIZE), SHIP_PART_SIZE + (y*SHIP_PART_SIZE), 0.55f);
            }
            else if(enemyGrid[x][y] == GRID_HIT_MISS)
            {
                C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_hit_marker_idx), (screen == GFX_TOP ? 80 : 40) + SHIP_PART_SIZE + (x*SHIP_PART_SIZE), SHIP_PART_SIZE + (y*SHIP_PART_SIZE), 0.55f);
            }
        }
    }
    if(this->lastHitX != -1 && this->lastHitY != -1)
    {
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_cursor_idx), (screen == GFX_TOP ? 80 : 40) + SHIP_PART_SIZE + (this->lastHitX*SHIP_PART_SIZE), SHIP_PART_SIZE + (this->lastHitY*SHIP_PART_SIZE), 0.65f, &enemyHitCursorTint, -1.0f, 1.0f);
    }

    for(const auto& ship : this->ships)
        ship.drawOnGrid(screen);
}

void PlayingState::drawEnemyGrid(gfxScreen_t screen)
{
    for(int x = 0; x < GRID_WIDTH; x++)
    {
        for(int y = 0; y < GRID_HEIGHT; y++)
        {
            if(enemyShipsGrid[x][y] == SHIPS_AMOUNT)
            {
                if(playerGrid[x][y] == GRID_HIT_NONE)
                {
                    if(sonarGrid[x][y] == SONAR_FOUND)
                        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_sonar_found_idx), (screen == GFX_TOP ? 80 : 40) + SHIP_PART_SIZE + (x*SHIP_PART_SIZE), SHIP_PART_SIZE + (y*SHIP_PART_SIZE), 0.55f, &damagedTint);
                }
                else
                {
                    C2D_ImageTint* tint = NULL;
                    if(playerGrid[x][y] == GRID_HIT_SUCCESS)
                        tint = &damagedTint;
                    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, sprites_hit_marker_idx), (screen == GFX_TOP ? 80 : 40) + SHIP_PART_SIZE + (x*SHIP_PART_SIZE), SHIP_PART_SIZE + (y*SHIP_PART_SIZE), 0.55f, tint);
                }
            }
        }
    }

    for(const auto& enemyShip : this->enemyShips)
        enemyShip.drawOnGrid(screen);
}

void PlayingState::update()
{
    if(!this->enemyJoined && currentRoom->getPlayerCount() == 2)
        this->enemyJoined = true;

    if(this->state != PLAYING_YOU_WON && this->state != PLAYING_ENEMY_WON)
    {
        if(this->checkEnemyLeft())
        {
            enemyLeft:
            this->state = PLAYING_ENEMY_LEFT;
            return;
        }
    }

    if(this->won())
    {
        this->state = PLAYING_YOU_WON;
    }
    if(this->lost())
    {
        this->state = PLAYING_ENEMY_WON;
    }


    if(this->state == PLAYING_PLACING_SHIPS)
    {
        bool ready = false;
        size_t actualSize = 0;
        currentRoom->receiveData(&ready, sizeof(ready), &actualSize);
        if(actualSize == sizeof(ready))
            this->enemyReady = ready;

        if(this->hoveringShip == SHIPS_AMOUNT)
        {
            if(kDown & KEY_TOUCH)
            {
                if(touch.px < 40)
                {
                    if(touch.py < 40)
                        goto prevShip;
                    else if(touch.py > 240-40)
                        goto nextShip;
                }
                else if(touch.px > 320-40)
                {
                    if(touch.py > 240-120)
                        goto readyForPlaying;
                }
                else if (touch.px >= 40 + SHIP_PART_SIZE && touch.px <= 320 - 40 - SHIP_PART_SIZE && touch.py >= SHIP_PART_SIZE && touch.py <= 240-SHIP_PART_SIZE)
                {
                    getGridCoordsFromTouch(touch, &this->squareX, &this->squareY);
                    if(shipsGrid[this->squareX][this->squareY] == SHIPS_AMOUNT)
                    {
                        this->ships[this->toHover].pickUp();
                        this->hoveringShip = toHover;
                        this->hoverRotation = POINTING_EAST;
                    }
                    else
                    {
                        auto type = shipsGrid[this->squareX][this->squareY];
                        auto& ship = this->ships[type];
                        ship.pickUp();
                        this->hoveringShip = type;
                        this->hoverRotation = ship.pointingTo;
                    }
                }
            }
            else if(kDown & KEY_A)
            {
                this->ships[this->toHover].pickUp();
                this->hoveringShip = toHover;
                this->hoverRotation = POINTING_EAST;

                this->squareX = 0;
                this->squareY = 0;
            }
            else if(kDown & KEY_X)
            {
                readyForPlaying:
                if(this->allShipsReady())
                {
                    ready = true;
                    currentRoom->sendData(&ready, sizeof(ready));
                    this->state = PLAYING_WAITING_FOR_ENEMY_PLACING;
                }
            }
            else if(kDown & KEY_L)
            {
                prevShip:
                switch(this->toHover)
                {
                    case SHIP_CARRIER:
                        DEBUG("switching to destroyer\n");
                        this->toHover = SHIP_DESTROYER;
                        break;
                    case SHIP_BATTLESHIP:
                        DEBUG("switching to carrier\n");
                        this->toHover = SHIP_CARRIER;
                        break;
                    case SHIP_CRUISER:
                        DEBUG("switching to battleship\n");
                        this->toHover = SHIP_BATTLESHIP;
                        break;
                    case SHIP_SUBMARINE:
                        DEBUG("switching to cruiser\n");
                        this->toHover = SHIP_CRUISER;
                        break;
                    case SHIP_DESTROYER:
                        DEBUG("switching to submarine\n");
                        this->toHover = SHIP_SUBMARINE;
                        break;
                    default:
                        this->toHover = SHIP_CARRIER;
                };
            }
            else if(kDown & KEY_R)
            {
                nextShip:
                switch(this->toHover)
                {
                    case SHIP_CARRIER:
                        DEBUG("switching to battleship\n");
                        this->toHover = SHIP_BATTLESHIP;
                        break;
                    case SHIP_BATTLESHIP:
                        DEBUG("switching to cruiser\n");
                        this->toHover = SHIP_CRUISER;
                        break;
                    case SHIP_CRUISER:
                        DEBUG("switching to submarine\n");
                        this->toHover = SHIP_SUBMARINE;
                        break;
                    case SHIP_SUBMARINE:
                        DEBUG("switching to destroyer\n");
                        this->toHover = SHIP_DESTROYER;
                        break;
                    case SHIP_DESTROYER:
                        DEBUG("switching to carrier\n");
                        this->toHover = SHIP_CARRIER;
                        break;
                    default:
                        this->toHover = SHIP_CARRIER;
                };
            }
        }
        else
        {
            if(kUp & KEY_TOUCH)
            {
                getGridCoordsFromTouch(oldTouch, &this->squareX, &this->squareY);
                goto placeShip;
            }
            else if(kDown & KEY_A)
            {
                placeShip:
                if(canPlaceShip(this->hoveringShip, this->hoverRotation, this->squareX, this->squareY))
                {
                    this->ships[this->hoveringShip].place(this->hoveringShip, this->hoverRotation, this->squareX, this->squareY);
                }
                this->hoveringShip = SHIPS_AMOUNT;
            }
            else if(kDown & KEY_B)
            {
                this->hoveringShip = SHIPS_AMOUNT;
            }
            else if(kDown & KEY_LEFT)
            {
                if(this->squareX > 0)
                    this->squareX--;
            }
            else if(kDown & KEY_RIGHT)
            {
                if(this->squareX < GRID_WIDTH-1)
                    this->squareX++;
            }
            else if(kDown & KEY_UP)
            {
                if(this->squareY > 0)
                    this->squareY--;
            }
            else if(kDown & KEY_DOWN)
            {
                if(this->squareY < GRID_HEIGHT-1)
                    this->squareY++;
            }
            else if(kDown & KEY_L)
            {
                switch(this->hoverRotation)
                {
                    case POINTING_EAST:
                        this->hoverRotation = POINTING_NORTH;
                        break;
                    case POINTING_SOUTH:
                        this->hoverRotation = POINTING_EAST;
                        break;
                    case POINTING_WEST:
                        this->hoverRotation = POINTING_SOUTH;
                        break;
                    case POINTING_NORTH:
                        this->hoverRotation = POINTING_WEST;
                        break;
                    default:
                        this->hoverRotation = POINTING_EAST;
                };
            }
            else if(kDown & KEY_R)
            {
                switch(this->hoverRotation)
                {
                    case POINTING_EAST:
                        this->hoverRotation = POINTING_SOUTH;
                        break;
                    case POINTING_SOUTH:
                        this->hoverRotation = POINTING_WEST;
                        break;
                    case POINTING_WEST:
                        this->hoverRotation = POINTING_NORTH;
                        break;
                    case POINTING_NORTH:
                        this->hoverRotation = POINTING_EAST;
                        break;
                    default:
                        this->hoverRotation = POINTING_EAST;
                };
            }
        }
    }
    else if(this->state == PLAYING_WAITING_FOR_ENEMY_PLACING)
    {
        if(this->enemyReady)
        {
            this->squareX = 0;
            this->squareY = 0;
            if(this->firstPlayer)
                this->startYourTurn();
            else
                this->state = PLAYING_WAITING_FOR_ENEMY_MOVE;
            return;
        }

        bool ready = false;
        size_t actualSize = 0;
        currentRoom->receiveData(&ready, sizeof(ready), &actualSize);
        if(actualSize == sizeof(ready))
            this->enemyReady = ready;

        if(kDown & KEY_TOUCH)
        {
            if(touch.px > 320-40 && touch.py > 240-120)
            {
                goto notReady;
            }
        }
        else if(kDown & KEY_B)
        {
            notReady:
            ready = false;
            currentRoom->sendData(&ready, sizeof(ready));
            this->state = PLAYING_PLACING_SHIPS;
        }
    }
    else if(this->state == PLAYING_YOUR_TURN)
    {
        if(kDown & KEY_TOUCH)
        {
            if(touch.px < 40)
            {
                if(touch.py < 120)
                {
                    goto sendDecoy;
                }
                else
                {
                    goto sendSonar;
                }
            }
            else if(touch.px > 320-40)
            {
                if(touch.py < 120)
                {
                    goto swapGrids;
                }
                else
                {
                    goto shoot;
                }
            }
            else if (touch.px >= 40 + SHIP_PART_SIZE && touch.px <= 320 - 40 - SHIP_PART_SIZE && touch.py >= SHIP_PART_SIZE && touch.py <= 240-SHIP_PART_SIZE)
            {
                getGridCoordsFromTouch(touch, &this->squareX, &this->squareY);
            }
        }
        if(kDown & KEY_LEFT)
        {
            if(this->squareX > 0)
                this->squareX--;
        }
        else if(kDown & KEY_RIGHT)
        {
            if(this->squareX < GRID_WIDTH-1)
                this->squareX++;
        }
        else if(kDown & KEY_UP)
        {
            if(this->squareY > 0)
                this->squareY--;
        }
        else if(kDown & KEY_DOWN)
        {
            if(this->squareY < GRID_HEIGHT-1)
                this->squareY++;
        }
        else if(kDown & KEY_A)
        {
            shoot:
            if(this->canShoot())
            {
                this->startNewSoundEffect(SFX_SHOT, 0);

                PositionPacket positionPacket = {false, this->squareX, this->squareY};
                currentRoom->sendData(&positionPacket, sizeof(positionPacket));

                ShipSunkPacket shipSunkPacket;
                size_t actualSize = 0;
                do {
                    currentRoom->receiveData(&shipSunkPacket, sizeof(shipSunkPacket), &actualSize);
                    if(this->checkEnemyLeft())
                        goto enemyLeft;
                } while(actualSize != sizeof(shipSunkPacket));

                svcSleepThread(1e7);
                if(shipSunkPacket.miss)
                {
                    this->startNewSoundEffect(SFX_MISS, 1e9);
                    playerGrid[this->squareX][this->squareY] = GRID_HIT_MISS;
                }
                else
                {
                    this->startNewSoundEffect(SFX_HIT, 1e9);
                    playerGrid[this->squareX][this->squareY] = GRID_HIT_SUCCESS;
                    if(shipSunkPacket.sunk)
                    {
                        this->startNewSoundEffect(SFX_ENEMY_SHIP_DEAD, 2e9);
                        auto& enemyShip = this->enemyShips[shipSunkPacket.type];
                        enemyShip.place(shipSunkPacket.type, shipSunkPacket.pointingTo, shipSunkPacket.x, shipSunkPacket.y);
                    }
                }

                this->swapGrids = false;
                this->state = PLAYING_WAITING_FOR_ENEMY_MOVE;
            }
        }
        else if(kDown & KEY_X)
        {
            sendDecoy:
            if(this->canDecoy())
            {
                shipsGrid[this->squareX][this->squareY] = SHIP_DECOY;
                this->remainingDecoys--;
                this->usedSpecial = true;
            }
        }
        else if(kDown & KEY_Y)
        {
            sendSonar:
            if(this->canSonar())
            {
                this->startNewSoundEffect(SFX_SONAR, 0);

                PositionPacket positionPacket = {true, this->squareX, this->squareY};
                currentRoom->sendData(&positionPacket, sizeof(positionPacket));

                SonarResultPacket resultPacket;
                size_t actualSize = 0;
                do {
                    currentRoom->receiveData(&resultPacket, sizeof(resultPacket), &actualSize);
                    if(this->checkEnemyLeft())
                        goto enemyLeft;
                } while(actualSize != sizeof(resultPacket));

                for(int x = -1; x < 1; x++)
                {
                    for(int y = -1; y < 1; y++)
                    {
                        sonarGrid[this->squareX+x][this->squareY+y] = resultPacket.result[x+1][y+1];
                    }
                }

                this->remainingSonars--;
                this->usedSpecial = true;
            }
        }
        else if(kDown & KEY_B)
        {
            swapGrids:
            this->swapGrids = !this->swapGrids;
        }
    }
    else if(this->state == PLAYING_WAITING_FOR_ENEMY_MOVE)
    {
        PositionPacket positionPacket;
        size_t actualSize = 0;
        currentRoom->receiveData(&positionPacket, sizeof(positionPacket), &actualSize);
        if(actualSize == sizeof(positionPacket))
        {
            void* data = nullptr;
            size_t size = 0;
            if(positionPacket.sonar)
            {
                enemySonarGrid[positionPacket.x][positionPacket.y] = true;
                this->startNewSoundEffect(SFX_SONAR, 0);

                static SonarResultPacket result;
                data = &result;
                size = sizeof(result);

                for(int x = -1; x <= 1; x++)
                {
                    for(int y = -1; y <= 1; y++)
                    {
                        result.result[x+1][y+1] = (shipsGrid[positionPacket.x+x][positionPacket.y+y] == SHIPS_AMOUNT) ? SONAR_NOTHING : SONAR_FOUND;
                    }
                }
            }
            else
            {
                this->startNewSoundEffect(SFX_SHOT, 0);

                static ShipSunkPacket result;
                data = &result;
                size = sizeof(result);

                result.miss = true;
                if(shipsGrid[positionPacket.x][positionPacket.y] != SHIPS_AMOUNT)
                {
                    for(auto& ship : this->ships)
                    {
                        if(ship.checkHit(positionPacket.x, positionPacket.y))
                        {
                            result.miss = false;
                            if(ship.isDead())
                            {
                                this->startNewSoundEffect(SFX_SHIP_DEAD, 1e9);
                                result.sunk = true;
                                result.type = ship.type;
                                result.pointingTo = ship.pointingTo;
                                result.x = ship.x;
                                result.y = ship.y;
                            }
                            break;
                        }
                    }
                }
            }
            currentRoom->sendData(data, size);
            if(!positionPacket.sonar)
                this->startYourTurn();
        }
    }
}

bool PlayingState::allShipsReady()
{
    return std::all_of(this->ships.cbegin(), this->ships.cend(), [](const auto& ship) { return ship.placed; });
}

void PlayingState::startYourTurn()
{
    this->state = PLAYING_YOUR_TURN;
    this->usedSpecial = false;
    this->swapGrids = true;
}

bool PlayingState::canDecoy()
{
    bool canDecoy = !this->swapGrids && (this->remainingDecoys > 0 && !this->usedSpecial) && shipsGrid[this->squareX][this->squareY] == SHIPS_AMOUNT && enemyGrid[this->squareX][this->squareY] == GRID_HIT_NONE;
    return canDecoy;
}
bool PlayingState::canSonar()  // Don't allow "wasting" sonars by using them where they wont be as effective
{
    bool canSonar = this->swapGrids && (this->remainingSonars > 0 && !this->usedSpecial) && (this->squareX > 0 && this->squareX < GRID_WIDTH-1 && this->squareY > 0 && this->squareY < GRID_HEIGHT-1);
    return canSonar;
}
bool PlayingState::canShoot()
{
    bool canShoot = this->swapGrids && playerGrid[this->squareX][this->squareY] == GRID_HIT_NONE && sonarGrid[this->squareX][this->squareY] != SONAR_NOTHING;
    return canShoot;
}

bool PlayingState::won()
{
    return std::all_of(this->enemyShips.cbegin(), this->enemyShips.cend(), [](const auto& enemyShip) { return enemyShip.dead; });
}
bool PlayingState::lost()
{
    return std::all_of(this->ships.cbegin(), this->ships.cend(), [](const auto& ship) { return ship.isDead(); });
}
bool PlayingState::checkEnemyLeft()
{
    if(!this->enemyJoined)  // if the enemy didn't join yet, he can't have left
    {
        return false;
    }
    else
    {
        return currentRoom->getPlayerCount() <= 1;  // If there's only 1 player left, the other left
    }
}

void PlayingState::startNewSoundEffect(SoundEffectID id, u64 delay)
{
    static const std::string soundEffectsPaths[SFX_AMOUNT] = {
        SFX_FOLDER  "hit.mp3",
        SFX_FOLDER  "miss.mp3",
        SFX_FOLDER  "shot.mp3",
        SFX_FOLDER  "sonar.mp3",

        SFX_FOLDER  "ship_dead.mp3",
        SFX_FOLDER  "enemy_dead.mp3",
    };
    this->soundEffects[id] = nullptr;  // make sure destructor is called before constructor
    this->soundEffects[id] = std::make_unique<SoundEffect>(soundEffectsPaths[id], id, delay);
}
