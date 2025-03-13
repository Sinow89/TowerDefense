#ifndef TILES_H
#define TILES_H

#define TILE_HEIGHT 64
#define TILE_WIDTH 64
#define MAP_HEIGHT 45
#define MAP_WIDTH 90

#include "raymath.h"

typedef enum tiles_type{
    FLOOR,
    WALL,
    GOAL,
    DOOR,
    TREASURE,
} tiles_type;

typedef struct tiles_t{
    Vector2 position;
    Vector2 size;
    int type;
} tiles_t;

tiles_t tiles[MAP_HEIGHT][MAP_WIDTH];

#endif // tiles.h