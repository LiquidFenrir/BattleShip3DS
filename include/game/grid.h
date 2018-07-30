#pragma once

#include "common.h"
#include "enums.h"

extern std::array<std::array<GridHitType, GRID_HEIGHT>, GRID_WIDTH> playerGrid, enemyGrid;
extern std::array<std::array<ShipTypes, GRID_HEIGHT>, GRID_WIDTH> shipsGrid, enemyShipsGrid;
extern std::array<std::array<SonarType, GRID_HEIGHT>, GRID_WIDTH> sonarGrid;
extern std::array<std::array<bool, GRID_HEIGHT>, GRID_WIDTH> enemySonarGrid;

void initGrids();
void getGridCoordsFromTouch(touchPosition touch, int* x, int* y);
