#pragma once

#include "common.h"

#include "enums.h"

class Ship
{
    public:
        ShipTypes type;
        PointingNose pointingTo;
        int x, y;
};

class FriendlyShip : public Ship
{
    public:
        bool placed = false;

        void place(ShipTypes type, PointingNose pointingTo, int x, int y);
        void pickUp();
        
        bool checkHit(int hitX, int hitY);
        bool isDead() const;

        void drawList() const;
        void drawOnGrid(gfxScreen_t screen) const;

    private:
        std::vector<bool> partsDamaged;
};

class EnemyShip : public Ship
{
    public:
        bool dead = false;

        void place(ShipTypes type, PointingNose pointingTo, int x, int y);

        void drawOnGrid(gfxScreen_t screen) const;
};

void drawEnemyShips(const std::array<EnemyShip, SHIPS_AMOUNT>& enemyShips);

void drawShipHoverTouch(ShipTypes type, PointingNose pointingTo, u16 x, u16 y);
void drawShipHover(ShipTypes type, PointingNose pointingTo, int x, int y);
void drawCurrentShip(ShipTypes type);

bool canPlaceShip(ShipTypes type, PointingNose pointingTo, int x, int y);
