#ifndef TILES_H
#define TILES_H

#define TILE_HEIGHT 64
#define TILE_WIDTH 64
#define MAP_HEIGHT 14
#define MAP_WIDTH 28

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

void create_tiles(){
    for (int y = 0; y < MAP_HEIGHT; y++){
        for (int x = 0; x < MAP_WIDTH; x++){
            tiles[y][x].position.y = y * 64;
            tiles[y][x].position.x = x * 64;
            tiles[y][x].type = FLOOR;
        }
    }
};

#endif // tiles.h