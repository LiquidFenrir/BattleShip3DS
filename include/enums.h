#pragma once

#include "common.h"

typedef enum : u8 {
    SHIP_CARRIER = 0,
    SHIP_BATTLESHIP,
    SHIP_CRUISER,
    SHIP_SUBMARINE,
    SHIP_DESTROYER,

    SHIPS_AMOUNT,
    
    SHIP_DECOY
} ShipTypes;

typedef enum {
    POINTING_EAST,
    POINTING_SOUTH,
    POINTING_WEST,
    POINTING_NORTH,

    DIRECTIONS_AMOUNT
} PointingNose;

typedef enum : u8 {
    GRID_HIT_NONE = 0,
    GRID_HIT_MISS,
    GRID_HIT_SUCCESS,
} GridHitType;

typedef enum : u8 {
    SONAR_NONE,
    SONAR_NOTHING,
    SONAR_FOUND,
} SonarType;