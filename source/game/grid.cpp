#include "game/grid.h"

std::array<std::array<GridHitType, GRID_WIDTH>, GRID_HEIGHT> playerGrid, enemyGrid;
std::array<std::array<ShipTypes, GRID_WIDTH>, GRID_HEIGHT> shipsGrid, enemyShipsGrid;
std::array<std::array<SonarType, GRID_WIDTH>, GRID_HEIGHT> sonarGrid;
std::array<std::array<bool, GRID_WIDTH>, GRID_HEIGHT> enemySonarGrid;

void initGrids()
{
    for(auto& row : playerGrid)
        std::fill(row.begin(), row.end(), GRID_HIT_NONE);
    for(auto& row : enemyGrid)
        std::fill(row.begin(), row.end(), GRID_HIT_NONE);
    for(auto& row : shipsGrid)
        std::fill(row.begin(), row.end(), SHIPS_AMOUNT);
    for(auto& row : enemyShipsGrid)
        std::fill(row.begin(), row.end(), SHIPS_AMOUNT);
    for(auto& row : sonarGrid)
        std::fill(row.begin(), row.end(), SONAR_NONE);
    for(auto& row : enemySonarGrid)
        std::fill(row.begin(), row.end(), false);
}

void getGridCoordsFromTouch(touchPosition touch, int* x, int* y)
{
    *x = (touch.px - 40) / SHIP_PART_SIZE;
    *x -= 1;
    *y = (touch.py) / SHIP_PART_SIZE;
    *y -= 1;
}
