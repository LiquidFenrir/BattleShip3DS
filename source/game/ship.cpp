#include "game/ship.h"

#include "game/grid.h"

C2D_ImageTint damagedTint;

// ------------------------------------------
static constexpr size_t idxFromType[SHIPS_AMOUNT] = {
    sprites_Carrier_part_1_idx,
    sprites_Battleship_part_1_idx,
    sprites_Cruiser_part_1_idx,
    sprites_Submarine_part_1_idx,
    sprites_Destroyer_part_1_idx,
};
static constexpr size_t lengthFromType[SHIPS_AMOUNT] = {
    5,
    4,
    3,
    3,
    2,
};
static constexpr int xDiffFromDirection[DIRECTIONS_AMOUNT] = {
    1,
    0,
    -1,
    0,
};
static constexpr int yDiffFromDirection[DIRECTIONS_AMOUNT] = {
    0,
    1,
    0,
    -1,
};
// ------------------------------------------

// ------------------------------------------
static void drawShipList(bool enemy, ShipTypes type, std::vector<bool>::const_iterator partsDamaged)
{
    static constexpr float xOffFromTypeForTop[SHIPS_AMOUNT] = {
        0,
        0,
        10,
        10,
        20,
    };
    float x = enemy ? (400 - xOffFromTypeForTop[type] - SHIP_PART_SIZE + (type == SHIP_CARRIER ? 4 : 0)) : xOffFromTypeForTop[type];
    float y = 40 * (static_cast<int>(type) + 1);
    float scaleX = (type == SHIP_CARRIER ? 0.8f : 1.0f) * (enemy ? -1.0f : 1.0f);

    size_t idx = idxFromType[type];
    size_t length = lengthFromType[type];
    for(size_t i = 0; i < length; ++i, ++partsDamaged, x += SHIP_PART_SIZE*scaleX)
    {
        C2D_ImageTint* tint = NULL;
        if(*partsDamaged)
            tint = &damagedTint;
        C2D_DrawImageAt(C2D_SpriteSheetGetImage(spritesheet, idx+i), x, y, 0.6f, tint, scaleX, 1.0f);
    }
}

static void drawShipOnGrid(ShipTypes type, PointingNose pointingTo, float x, float y, std::vector<bool>::const_iterator partsDamaged)
{
    static constexpr float directionToAngle[DIRECTIONS_AMOUNT] = {
        C3D_AngleFromDegrees(0.0f),
        C3D_AngleFromDegrees(90.0f),
        C3D_AngleFromDegrees(180.0f),
        C3D_AngleFromDegrees(270.0f),
    };

    size_t idx = idxFromType[type];
    size_t length = lengthFromType[type];

    float xDiff = xDiffFromDirection[pointingTo]*SHIP_PART_SIZE;
    static constexpr float xOff = SHIP_PART_SIZE/2;
    float yDiff = yDiffFromDirection[pointingTo]*SHIP_PART_SIZE;
    static constexpr float yOff = SHIP_PART_SIZE/2;
    float angle = directionToAngle[pointingTo];
    for(size_t i = 0; i < length; ++i, ++partsDamaged, x += xDiff, y += yDiff)
    {
        C2D_ImageTint* tint = NULL;
        if(*partsDamaged)
            tint = &damagedTint;
        C2D_DrawImageAtRotated(C2D_SpriteSheetGetImage(spritesheet, idx+i), x + xOff, y + yOff, 0.6f, angle, tint);
    }
}
// ------------------------------------------

// ------------------------------------------
void FriendlyShip::place(ShipTypes type, PointingNose pointingTo, int x, int y)
{
    this->x = x;
    this->y = y;
    this->pointingTo = pointingTo;
    this->type = type;
    this->partsDamaged = std::vector<bool>(lengthFromType[type], false);
    this->placed = true;

    int xDiff = xDiffFromDirection[pointingTo];
    int yDiff = yDiffFromDirection[pointingTo];
    size_t length = lengthFromType[type];
    for(size_t i = 0; i < length; i++, x += xDiff, y += yDiff)
    {
        shipsGrid[x][y] = type;
    }
}

void FriendlyShip::pickUp()
{
    if(this->placed)
    {
        this->placed = false;

        int x = this->x;
        int y = this->y;
        int xDiff = xDiffFromDirection[this->pointingTo];
        int yDiff = yDiffFromDirection[this->pointingTo];
        size_t length = lengthFromType[this->type];
        for(size_t i = 0; i < length; i++, x += xDiff, y += yDiff)
        {
            shipsGrid[x][y] = SHIPS_AMOUNT;
        }
    }
}

bool FriendlyShip::checkHit(int hitX, int hitY)
{
    int x = this->x;
    int y = this->y;
    int xDiff = xDiffFromDirection[this->pointingTo];
    int yDiff = yDiffFromDirection[this->pointingTo];
    size_t length = lengthFromType[this->type];

    for(size_t i = 0; i < length; i++, x += xDiff, y += yDiff)
    {
        if(hitX == x && hitY == y)
        {
            this->partsDamaged[i] = true;
            return true;
        }
    }

    return false;
}

bool FriendlyShip::isDead() const
{
    if(!this->placed)
        return false;
    return std::all_of(this->partsDamaged.cbegin(), this->partsDamaged.cend(), [](bool partDamaged) { return partDamaged; });
}

void FriendlyShip::drawList() const
{
    if(this->placed)
        drawShipList(false, this->type, this->partsDamaged.cbegin());
}

void FriendlyShip::drawOnGrid(gfxScreen_t screen) const
{
    if(this->placed)
        drawShipOnGrid(this->type, this->pointingTo, (screen == GFX_TOP ? 80 : 40) + SHIP_PART_SIZE + (this->x*SHIP_PART_SIZE), SHIP_PART_SIZE + (this->y*SHIP_PART_SIZE), this->partsDamaged.cbegin());
}
// ------------------------------------------

// ------------------------------------------
void EnemyShip::place(ShipTypes type, PointingNose pointingTo, int x, int y)
{
    this->x = x;
    this->y = y;
    this->pointingTo = pointingTo;
    this->type = type;
    this->dead = true;

    int xDiff = xDiffFromDirection[pointingTo];
    int yDiff = yDiffFromDirection[pointingTo];
    size_t length = lengthFromType[type];
    for(size_t i = 0; i < length; i++, x += xDiff, y += yDiff)
    {
        enemyShipsGrid[x][y] = type;
    }
}

void EnemyShip::drawOnGrid(gfxScreen_t screen) const
{
    if(this->dead)
    {
        std::vector<bool> partsDamaged(lengthFromType[this->type], this->dead);
        drawShipOnGrid(this->type, this->pointingTo, (screen == GFX_TOP ? 80 : 40) + SHIP_PART_SIZE + (this->x*SHIP_PART_SIZE), SHIP_PART_SIZE + (this->y*SHIP_PART_SIZE), partsDamaged.cbegin());
    }
}
// ------------------------------------------

// ------------------------------------------
void drawEnemyShips(const std::array<EnemyShip, SHIPS_AMOUNT>& enemyShips)
{
    for(int type = SHIP_CARRIER; type < SHIPS_AMOUNT; ++type)
    {
        const auto& ship = enemyShips[type];
        std::vector<bool> partsDamaged(lengthFromType[type], ship.dead);
        drawShipList(true, static_cast<ShipTypes>(type), partsDamaged.cbegin());
    }
}
// ------------------------------------------

// ------------------------------------------
static void drawShipHoverInternal(ShipTypes type, PointingNose pointingTo, float x, float y, bool canPlace)
{
    std::vector<bool> partsDamaged(lengthFromType[type], !canPlace);
    drawShipOnGrid(type, pointingTo, x, y, partsDamaged.cbegin());
}
void drawShipHoverTouch(ShipTypes type, PointingNose pointingTo, u16 x, u16 y)
{
    int actualX, actualY;
    getGridCoordsFromTouch(touch, &actualX, &actualY);
    bool canPlace = canPlaceShip(type, pointingTo, actualX, actualY);
    drawShipHoverInternal(type, pointingTo, x-(SHIP_PART_SIZE/2), y-(SHIP_PART_SIZE/2), canPlace);
}
void drawShipHover(ShipTypes type, PointingNose pointingTo, int x, int y)
{
    bool canPlace = canPlaceShip(type, pointingTo, x, y);
    drawShipHoverInternal(type, pointingTo, 40 + SHIP_PART_SIZE + (x*SHIP_PART_SIZE), SHIP_PART_SIZE + (y*SHIP_PART_SIZE), canPlace);
}
void drawCurrentShip(ShipTypes type)
{
    size_t length = lengthFromType[type];
    size_t idx = idxFromType[type];
    static constexpr float x = 10.0f;
    static constexpr float xOff = SHIP_PART_SIZE/2;
    float y = 120.0f - length*SHIP_PART_SIZE/2;
    static constexpr float yOff = SHIP_PART_SIZE/2;
    for(size_t i = length; i > 0; i--, y += SHIP_PART_SIZE)
    {
        C2D_DrawImageAtRotated(C2D_SpriteSheetGetImage(spritesheet, idx+i-1), x + xOff, y + yOff, 0.6f, C3D_AngleFromDegrees(-90.0f));
    }
}
// ------------------------------------------

// ------------------------------------------
bool canPlaceShip(ShipTypes type, PointingNose pointingTo, int x, int y)
{
    int xDiff = xDiffFromDirection[pointingTo];
    int yDiff = yDiffFromDirection[pointingTo];
    size_t length = lengthFromType[type];
    for(size_t i = 0; i < length; i++, x += xDiff, y += yDiff)
    {
        if(x < 0 || y < 0 || x >= GRID_WIDTH || y >= GRID_HEIGHT)
            return false;
        if(shipsGrid[x][y] != SHIPS_AMOUNT)
            return false;
    }
    return true;
}
// ------------------------------------------
