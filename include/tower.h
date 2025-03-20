#ifndef TOWER_H
#define TOWER_H
#define MAX_BULLETS 100
#include "raymath.h"

typedef struct{
    Vector2 position;
    Vector2 velocity;
    float radius;
    float rotation;
    float speed;
    int damage;
    bool active;
    int texture_x;
    int texture_y;
} bullets_t;

typedef struct{
    Vector2 position;
    Vector2 size;
} tower_t;

bullets_t bullets[MAX_BULLETS];

#endif // tower.h