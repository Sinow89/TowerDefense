#ifndef TOWER_H
#define TOWER_H
#define MAX_BULLETS 100
#include "raymath.h"

typedef struct{
    Vector2 position;
    Vector2 velocity;
    float radius;
    float speed;
    int damage;
    bool active;
} bullets_t;

typedef struct{
    Vector2 position;
    Vector2 size;
} tower_t;

bullets_t bullet[MAX_BULLETS] = {350, 300, 0, 0, 10, 10, 10, true};

#endif // tower.h