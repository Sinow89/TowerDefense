#ifndef TILES_H
#define TILES_H

#define TILE_HEIGHT 64
#define TILE_WIDTH 64
#define MAP_HEIGHT 12
#define MAP_WIDTH 12

#include "raymath.h"

typedef enum tiles_type{
    FLOOR,
    WALL,
    SAND,
    WATER,
    TREASURE,
} tiles_type;

typedef struct tiles_t{
    Vector2 position;
    Vector2 size;
    int type;
    int texture_x;
    int texture_y;
} tiles_t;

tiles_t tiles[MAP_HEIGHT][MAP_WIDTH];

void create_tiles()
{
    for (int y = 0; y < MAP_HEIGHT; y++){
        for (int x = 0; x < MAP_WIDTH; x++){
            tiles[y][x].position.y = y * TILE_HEIGHT;
            tiles[y][x].position.x = x * TILE_WIDTH;
            tiles[y][x].type = FLOOR;

            tiles[y][x].texture_x = 1;
            tiles[y][x].texture_y = 1;
        }
    }

    tiles[0][0].texture_x = 1;  tiles[0][0].texture_y = 1;
    tiles[0][1].texture_x = 1;  tiles[0][1].texture_y = 1;
    tiles[0][2].texture_x = 1;  tiles[0][2].texture_y = 1;
    tiles[0][3].texture_x = 1;  tiles[0][3].texture_y = 1;
    tiles[0][4].texture_x = 1;  tiles[0][4].texture_y = 1;
    tiles[0][5].texture_x = 1;  tiles[0][5].texture_y = 1;
    tiles[0][6].texture_x = 1;  tiles[0][6].texture_y = 1;
    tiles[0][7].texture_x = 1;  tiles[0][7].texture_y = 1;
    tiles[0][8].texture_x = 1;  tiles[0][8].texture_y = 1;
    tiles[0][9].texture_x = 1; tiles[0][9].texture_y = 1;
    tiles[0][10].texture_x = 1; tiles[0][10].texture_y = 1;
    tiles[0][11].texture_x = 1; tiles[0][11].texture_y = 1;
    
    tiles[1][0].texture_x = 1;  tiles[1][0].texture_y = 1;
    tiles[1][1].texture_x = 1;  tiles[1][1].texture_y = 1;
    tiles[1][2].texture_x = 1;  tiles[1][2].texture_y = 1;
    tiles[1][3].texture_x = 1;  tiles[1][3].texture_y = 1;
    tiles[1][4].texture_x = 1;  tiles[1][4].texture_y = 1;
    tiles[1][5].texture_x = 1;  tiles[1][5].texture_y = 1;
    tiles[1][6].texture_x = 1;  tiles[1][6].texture_y = 1;
    tiles[1][7].texture_x = 1;  tiles[1][7].texture_y = 1;
    tiles[1][8].texture_x = 1;  tiles[1][8].texture_y = 1;
    tiles[1][9].texture_x = 1; tiles[1][9].texture_y = 1;
    tiles[1][10].texture_x = 1; tiles[1][10].texture_y = 1;
    tiles[1][11].texture_x = 1; tiles[1][11].texture_y = 1;
    
    tiles[2][0].texture_x = 1;  tiles[2][0].texture_y = 1;
    tiles[2][1].texture_x = 1;  tiles[2][1].texture_y = 1;
    tiles[2][2].texture_x = 1;  tiles[2][2].texture_y = 1;
    tiles[2][3].texture_x = 1;  tiles[2][3].texture_y = 1;
    tiles[2][4].texture_x = 1;  tiles[2][4].texture_y = 1;
    tiles[2][5].texture_x = 1;  tiles[2][5].texture_y = 1;
    tiles[2][6].texture_x = 1;  tiles[2][6].texture_y = 1;
    tiles[2][7].texture_x = 1;  tiles[2][7].texture_y = 1;
    tiles[2][8].texture_x = 1;  tiles[2][8].texture_y = 1;
    tiles[2][9].texture_x = 1; tiles[2][9].texture_y = 1;
    tiles[2][10].texture_x = 1; tiles[2][10].texture_y = 1;
    tiles[2][11].texture_x = 1; tiles[2][11].texture_y = 1;
    
    tiles[3][0].texture_x = 1;  tiles[3][0].texture_y = 1;
    tiles[3][1].texture_x = 1;  tiles[3][1].texture_y = 1;
    tiles[3][2].texture_x = 1;  tiles[3][2].texture_y = 1;
    tiles[3][3].texture_x = 1;  tiles[3][3].texture_y = 1;
    tiles[3][4].texture_x = 1;  tiles[3][4].texture_y = 1;
    tiles[3][5].texture_x = 1;  tiles[3][5].texture_y = 1;
    tiles[3][6].texture_x = 1;  tiles[3][6].texture_y = 1;
    tiles[3][7].texture_x = 1;  tiles[3][7].texture_y = 1;
    tiles[3][8].texture_x = 1;  tiles[3][8].texture_y = 1;
    tiles[3][9].texture_x = 1; tiles[3][9].texture_y = 1;
    tiles[3][10].texture_x = 1; tiles[3][10].texture_y = 1;
    tiles[3][11].texture_x = 1; tiles[3][11].texture_y = 1;
    
    tiles[4][0].texture_x = 1;  tiles[4][0].texture_y = 1;
    tiles[4][1].texture_x = 1;  tiles[4][1].texture_y = 1;
    tiles[4][2].texture_x = 1;  tiles[4][2].texture_y = 1;
    tiles[4][3].texture_x = 1;  tiles[4][3].texture_y = 1;
    tiles[4][4].texture_x = 1;  tiles[4][4].texture_y = 1;
    tiles[4][5].texture_x = 1;  tiles[4][5].texture_y = 1;
    tiles[4][6].texture_x = 1;  tiles[4][6].texture_y = 1;
    tiles[4][7].texture_x = 1;  tiles[4][7].texture_y = 1;
    tiles[4][8].texture_x = 1;  tiles[4][8].texture_y = 1;
    tiles[4][9].texture_x = 1; tiles[4][9].texture_y = 1;
    tiles[4][10].texture_x = 1; tiles[4][10].texture_y = 1;
    tiles[4][11].texture_x = 1; tiles[4][11].texture_y = 1;
    
    tiles[5][0].texture_x = 1;  tiles[5][0].texture_y = 1;
    tiles[5][1].texture_x = 1;  tiles[5][1].texture_y = 1;
    tiles[5][2].texture_x = 1;  tiles[5][2].texture_y = 1;
    tiles[5][3].texture_x = 1;  tiles[5][3].texture_y = 1;
    tiles[5][4].texture_x = 1;  tiles[5][4].texture_y = 1;
    tiles[5][5].texture_x = 1;  tiles[5][5].texture_y = 1;
    tiles[5][6].texture_x = 1;  tiles[5][6].texture_y = 1;
    tiles[5][7].texture_x = 1;  tiles[5][7].texture_y = 1;
    tiles[5][8].texture_x = 1;  tiles[5][8].texture_y = 1;
    tiles[5][9].texture_x = 1; tiles[5][9].texture_y = 1;
    tiles[5][10].texture_x = 1; tiles[5][10].texture_y = 1;
    tiles[5][11].texture_x = 1; tiles[5][11].texture_y = 1;
    
    tiles[6][0].texture_x = 1;  tiles[6][0].texture_y = 1;
    tiles[6][1].texture_x = 1;  tiles[6][1].texture_y = 1;
    tiles[6][2].texture_x = 1;  tiles[6][2].texture_y = 1;
    tiles[6][3].texture_x = 1;  tiles[6][3].texture_y = 1;
    tiles[6][4].texture_x = 1;  tiles[6][4].texture_y = 1;
    tiles[6][5].texture_x = 1;  tiles[6][5].texture_y = 1;
    tiles[6][6].texture_x = 1;  tiles[6][6].texture_y = 1;
    tiles[6][7].texture_x = 1;  tiles[6][7].texture_y = 1;
    tiles[6][8].texture_x = 1;  tiles[6][8].texture_y = 1;
    tiles[6][9].texture_x = 1; tiles[6][9].texture_y = 1;
    tiles[6][10].texture_x = 1; tiles[6][10].texture_y = 1;
    tiles[6][11].texture_x = 1; tiles[6][11].texture_y = 1;
    
    tiles[7][0].texture_x = 1;  tiles[7][0].texture_y = 1;
    tiles[7][1].texture_x = 1;  tiles[7][1].texture_y = 1;
    tiles[7][2].texture_x = 1;  tiles[7][2].texture_y = 1;
    tiles[7][3].texture_x = 1;  tiles[7][3].texture_y = 1;
    tiles[7][4].texture_x = 1;  tiles[7][4].texture_y = 1;
    tiles[7][5].texture_x = 1;  tiles[7][5].texture_y = 1;
    tiles[7][6].texture_x = 1;  tiles[7][6].texture_y = 1;
    tiles[7][7].texture_x = 1;  tiles[7][7].texture_y = 1;
    tiles[7][8].texture_x = 1;  tiles[7][8].texture_y = 1;
    tiles[7][9].texture_x = 1; tiles[7][9].texture_y = 1;
    tiles[7][10].texture_x = 1; tiles[7][10].texture_y = 1;
    tiles[7][11].texture_x = 1; tiles[7][11].texture_y = 1;
    
    tiles[8][0].texture_x = 1;  tiles[8][0].texture_y = 1;
    tiles[8][1].texture_x = 1;  tiles[8][1].texture_y = 1;
    tiles[8][2].texture_x = 1;  tiles[8][2].texture_y = 1;
    tiles[8][3].texture_x = 1;  tiles[8][3].texture_y = 1;
    tiles[8][4].texture_x = 1;  tiles[8][4].texture_y = 1;
    tiles[8][5].texture_x = 1;  tiles[8][5].texture_y = 1;
    tiles[8][6].texture_x = 1;  tiles[8][6].texture_y = 1;
    tiles[8][7].texture_x = 1;  tiles[8][7].texture_y = 1;
    tiles[8][8].texture_x = 1;  tiles[8][8].texture_y = 1;
    tiles[8][9].texture_x = 1; tiles[8][9].texture_y = 1;
    tiles[8][10].texture_x = 1; tiles[8][10].texture_y = 1;
    tiles[8][11].texture_x = 1; tiles[8][11].texture_y = 1;
    
    tiles[9][0].texture_x = 1;  tiles[9][0].texture_y = 1;
    tiles[9][1].texture_x = 1;  tiles[9][1].texture_y = 1;
    tiles[9][2].texture_x = 1;  tiles[9][2].texture_y = 1;
    tiles[9][3].texture_x = 1;  tiles[9][3].texture_y = 1;
    tiles[9][4].texture_x = 1;  tiles[9][4].texture_y = 1;
    tiles[9][5].texture_x = 1;  tiles[9][5].texture_y = 1;
    tiles[9][6].texture_x = 1;  tiles[9][6].texture_y = 1;
    tiles[9][7].texture_x = 1;  tiles[9][7].texture_y = 1;
    tiles[9][8].texture_x = 1;  tiles[9][8].texture_y = 1;
    tiles[9][9].texture_x = 1; tiles[9][9].texture_y = 1;
    tiles[9][10].texture_x = 1; tiles[9][10].texture_y = 1;
    tiles[9][11].texture_x = 1; tiles[9][11].texture_y = 1;
    
    tiles[10][0].texture_x = 1;  tiles[10][0].texture_y = 1;
    tiles[10][1].texture_x = 1;  tiles[10][1].texture_y = 1;
    tiles[10][2].texture_x = 1;  tiles[10][2].texture_y = 1;
    tiles[10][3].texture_x = 1;  tiles[10][3].texture_y = 1;
    tiles[10][4].texture_x = 1;  tiles[10][4].texture_y = 1;
    tiles[10][5].texture_x = 1;  tiles[10][5].texture_y = 1;
    tiles[10][6].texture_x = 1;  tiles[10][6].texture_y = 1;
    tiles[10][7].texture_x = 1;  tiles[10][7].texture_y = 1;
    tiles[10][8].texture_x = 1;  tiles[10][8].texture_y = 1;
    tiles[10][9].texture_x = 1; tiles[10][9].texture_y = 1;
    tiles[10][10].texture_x = 1; tiles[10][10].texture_y = 1;
    tiles[10][11].texture_x = 1; tiles[10][11].texture_y = 1;
    
    tiles[11][0].texture_x = 1;  tiles[11][0].texture_y = 1;
    tiles[11][1].texture_x = 1;  tiles[11][1].texture_y = 1;
    tiles[11][2].texture_x = 1;  tiles[11][2].texture_y = 1;
    tiles[11][3].texture_x = 1;  tiles[11][3].texture_y = 1;
    tiles[11][4].texture_x = 1;  tiles[11][4].texture_y = 1;
    tiles[11][5].texture_x = 1;  tiles[11][5].texture_y = 1;
    tiles[11][6].texture_x = 1;  tiles[11][6].texture_y = 1;
    tiles[11][7].texture_x = 1;  tiles[11][7].texture_y = 1;
    tiles[11][8].texture_x = 1;  tiles[11][8].texture_y = 1;
    tiles[11][9].texture_x = 1; tiles[11][9].texture_y = 1;
    tiles[11][10].texture_x = 1; tiles[11][10].texture_y = 1;
    tiles[11][11].texture_x = 1; tiles[11][11].texture_y = 1;

};

#endif // tiles.h