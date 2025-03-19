// Created 2025-03-10 by Christoffer Rozenbachs

#define MAX_TEXTURES 1
#define MAX_ENEMIES 10

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "raylib.h"
#include "bfs.h"
#include "map.h"
#include "tower.h"

const int screen_width = MAP_WIDTH * TILE_WIDTH;
const int screen_height = MAP_HEIGHT * TILE_HEIGHT;
const int key = 0;
int path_length = 0;
Vector2 goal_position = {0, 0};
bool path_found = false;

typedef enum GameScreen{ 
    LOGO,
    TITLE,
    GAMEPLAY,
    ENDING
} GameScreen;

typedef struct player_t{
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    int hp;
    int key;
} player_t;

typedef enum{
    TEXTURE_TILE_MAP = 0,
} texture_asset;

Texture2D textures[MAX_TEXTURES];

void update_tiles_on_mouse(Vector2 mouse_point, tiles_t tiles[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            Rectangle tile_rect = { tiles[i][j].position.x, tiles[i][j].position.y, TILE_WIDTH, TILE_HEIGHT };

            if (CheckCollisionPointRec(mouse_point, tile_rect)) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    tiles[i][j].type = WALL;
                    tiles[i][j].texture_x = 11;  tiles[i][j].texture_y = 1;
                }
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    tiles[i][j].type = SAND;
                    tiles[i][j].texture_x = 11;  tiles[i][j].texture_y = 10;
                }
            }
        }
    }
}

// Function to visualize the path
void drawPath(Point *path, int path_length, Color color) {
    int i;
    for (i = 0; i < path_length; i++) {
        DrawRectangle(
            path[i].x * TILE_WIDTH + TILE_WIDTH/4,
            path[i].y * TILE_HEIGHT + TILE_HEIGHT/4,
            TILE_WIDTH/2,
            TILE_HEIGHT/2,
            color
        );
    }
}

// Function to get the next position along a path
Vector2 moveAlongPath(Vector2 current_pos, Point *path, int path_length, int *current_path_index, float speed, float delta_time) {
    if (*current_path_index >= path_length) {
        return current_pos;
    }
    
    // Target position on the path
    Vector2 target;
    target.x = path[*current_path_index].x * TILE_WIDTH + TILE_WIDTH/2;
    target.y = path[*current_path_index].y * TILE_HEIGHT + TILE_HEIGHT/2;
    
    // Calculate direction and distance
    Vector2 direction = Vector2Subtract(target, current_pos);
    float distance = Vector2Length(direction);
    
    // If we're close enough to the current target, move to the next point on the path
    if (distance < 5.0f) {
        (*current_path_index)++;
        
        // If we've reached the end of the path, return the current position
        if (*current_path_index >= path_length) {
            return current_pos;
        }
        
        // Update the target to the new path point
        target.x = path[*current_path_index].x * TILE_WIDTH + TILE_WIDTH/2;
        target.y = path[*current_path_index].y * TILE_HEIGHT + TILE_HEIGHT/2;
        direction = Vector2Subtract(target, current_pos);
        distance = Vector2Length(direction);
    }
    
    // Move towards the target
    if (distance > 0) {
        direction = Vector2Normalize(direction);
        float move_distance = speed * delta_time;
        
        if (move_distance > distance) {
            return target;
        } else {
            return Vector2Add(current_pos, Vector2Scale(direction, move_distance));
        }
    }
    
    return current_pos;
}

typedef struct enemy_t {
    Vector2 position;
    Vector2 size;
    float speed;
    int health;
    bool active;
    int current_path_index;
    Point path[MAP_WIDTH * MAP_HEIGHT]; // Individual path for this enemy
    int path_length;
    Vector2 velocity;
} enemy_t;

enemy_t enemies[MAX_ENEMIES];

// Function to initialize enemies
void initEnemies(void) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].size.x = 64;
        enemies[i].size.y = 64;
        enemies[i].speed = 100.0f;
        enemies[i].health = 100;
        enemies[i].active = false;
        enemies[i].current_path_index = 0;
    }
}

// Function to spawn a new enemy
void spawnEnemy(Vector2 position) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].position = position;
            enemies[i].active = true;
            enemies[i].current_path_index = 0;
            enemies[i].velocity = (Vector2){0, 0};
            break;
        }
    }
}

float enemy_speed_on_tile(Vector2 position, tiles_t tiles[MAP_HEIGHT][MAP_WIDTH]) {
    int tileX = (int)(position.x / TILE_WIDTH);
    int tileY = (int)(position.y / TILE_HEIGHT);
    
    if (tileX >= 0 && tileX < MAP_WIDTH && tileY >= 0 && tileY < MAP_HEIGHT) {
        if (tiles[tileY][tileX].type == SAND) {
            return 50.0f;
        }
        if (tiles[tileY][tileX].type == WATER) {
            return 25.0f;
        }
    }
    return 100.0f;
}

// Function to update all enemies
void updateEnemies(float delta_time, PathInfo info[MAP_HEIGHT][MAP_WIDTH]) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            int tile_x = enemies[i].position.x / TILE_WIDTH;
            int tile_y = enemies[i].position.y / TILE_HEIGHT;
            
            if (info[tile_y][tile_x].parent_x < 0 || info[tile_y][tile_x].parent_y < 0) {
                enemies[i].active = false;
                continue;
            }
            
            float goal_x = (info[tile_y][tile_x].parent_x * TILE_WIDTH) + (TILE_WIDTH / 2);
            float goal_y = (info[tile_y][tile_x].parent_y * TILE_HEIGHT) + (TILE_HEIGHT / 2);
            
            // Calculate direction to parent tile center
            Vector2 target_direction = Vector2Subtract((Vector2){goal_x, goal_y}, enemies[i].position);
            
            // Only move if we're not already at the target (prevents jittering)
            float distance = Vector2Length(target_direction);
            if (distance > 2.0f) {
                target_direction = Vector2Normalize(target_direction);
                enemies[i].velocity = Vector2Scale(target_direction, enemies[i].speed);
                enemies[i].position = Vector2Add(enemies[i].position, Vector2Scale(enemies[i].velocity, delta_time));
            } else {
                // We're close enough to this tile center, check if we need to move to the next tile
                // Get the next tile in the path from the parent information
                int next_tile_x = info[tile_y][tile_x].parent_x;
                int next_tile_y = info[tile_y][tile_x].parent_y;
                
                // If we're at the center of the current tile, snap to it to prevent floating point issues
                enemies[i].position.x = tile_x * TILE_WIDTH + (TILE_WIDTH / 2);
                enemies[i].position.y = tile_y * TILE_HEIGHT + (TILE_HEIGHT / 2);
            }
            
            enemies[i].speed = enemy_speed_on_tile(enemies[i].position, tiles);
        }
    }
}

// Function to draw all enemies
void drawEnemies(void) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            Vector2 position;
            position.x = enemies[i].position.x - enemies[i].size.x/2;
            position.y = enemies[i].position.y - enemies[i].size.y/2;

            DrawRectangleV(position, enemies[i].size, RED);
        }
    }
}

void shoot_bullet(Vector2 *bulletPos, Vector2 enemyPos, float speed) {
    Vector2 direction = Vector2Subtract(enemyPos, *bulletPos);
    direction = Vector2Normalize(direction);
    *bulletPos = Vector2Add(*bulletPos, Vector2Scale(direction, speed));
}

int main(void) {
    create_tiles();

    InitWindow(screen_width, screen_height, "Tower Defense");
    SetTargetFPS(60);

    GameScreen current_screen = GAMEPLAY;
    Vector2 enemy_pos = {200, 200};
    Vector2 mouse_point = { 0.0f, 0.0f };
    
    // Initialize pathfinding variables
    path_found = false;
    path_length = 0;
    goal_position.x = 5 * TILE_WIDTH + (TILE_WIDTH / 2);
    goal_position.y = 11 * TILE_HEIGHT + (TILE_HEIGHT / 2);
    
    initEnemies();
    
    Vector2 spawn_position;
    spawn_position.x = 5 * TILE_WIDTH + (TILE_WIDTH / 2);
    spawn_position.y = 0 * TILE_HEIGHT + (TILE_HEIGHT / 2);
    
    float enemy_spawn_timer = 0;
    float enemy_spawn_interval = 3.0f;

    int frames_counter = 0;

    Image image = LoadImage("assets/towerDefense_tilesheet.png");
    textures[TEXTURE_TILE_MAP] = LoadTextureFromImage(image);
    UnloadImage(image);
    
    PathInfo info[MAP_HEIGHT][MAP_WIDTH] = { 0 };
    
    while (!WindowShouldClose()) { 
        float delta_time = GetFrameTime();
        mouse_point = GetMousePosition();


        // for (int i = 0; i < MAX_ENEMIES; i++) {
        //     if (enemies[i].active) {
        //         Rectangle rec = {enemies[i].position.x, enemies[i].position.y, 16, 16};
        
        //         if (CheckCollisionCircleRec(bullet[i].position, bullet[i].radius, rec)) {
        //             enemies[i].health = 0;
        //             enemies[i].active = false;
        //             bullet[i].active = false;
        //         }
        //     }
        // }
        
        update_tiles_on_mouse(mouse_point, tiles);
        path_found = findPath(tiles, info, goal_position, spawn_position, path, &path_length);
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            // Find path from enemy spawn to goal
            // enemies[0].health = 0;
            // if (enemies[0].health == 0){
            //     enemies[0].active = false;
            // }
        }
        
        // Handle enemy spawning
        enemy_spawn_timer += delta_time;
        if (enemy_spawn_timer >= enemy_spawn_interval && path_found) {
            spawnEnemy(spawn_position);
            enemy_spawn_timer = 0;
        }
        
        updateEnemies(delta_time, info);

        switch (current_screen) {
            case LOGO:
            {
                frames_counter++;
                if (frames_counter > 240) {
                    current_screen = TITLE;
                }
            } break;
            case TITLE:
            {
                if (IsKeyPressed(KEY_SPACE)) {
                    current_screen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                if (IsKeyPressed(KEY_Q)) {
                    current_screen = ENDING;
                }
            } break;
            case ENDING:
            {
                if (IsKeyPressed(KEY_ENTER)) {
                    current_screen = TITLE;
                }
            } break;
            default: break;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            switch(current_screen) {
                case LOGO:
                {
                    DrawRectangle(0, 0, screen_width, screen_height, BLACK);
                    DrawText("Tower Defense", 400, 200, 150, WHITE);
                    DrawText("by Christoffer Rozenbachs", 800, 350, 20, GRAY);
                } break;
                case TITLE:
                {
                    DrawRectangle(0, 0, screen_width, screen_height, BLACK);
                    DrawText("How to play the game", 400, 150, 50, WHITE);
                    DrawText("Left-click to place walls", 400, 250, 20, GRAY);
                    DrawText("Right-click to set the goal", 400, 300, 20, GRAY);
                    DrawText("Enemies will spawn and follow the path to the goal", 400, 350, 20, GRAY);
                    DrawText("Press Space to start the game", 600, 600, 20, WHITE);
                } break;
                case GAMEPLAY:
                {
                    int texture_index_x = 0;
                    int texture_index_y = 0;
                    int x, y;

                    for (y = 0; y < MAP_HEIGHT; y++) {
                        for (x = 0; x < MAP_WIDTH; x++) {
                            Rectangle source;
                            source.x = (float)tiles[y][x].texture_x * TILE_WIDTH;
                            source.y = (float)tiles[y][x].texture_y * TILE_HEIGHT;
                            source.width = (float)TILE_WIDTH;
                            source.height = (float)TILE_HEIGHT;
                
                            Rectangle dest;
                            dest.x = (float)(tiles[y][x].position.x);
                            dest.y = (float)(tiles[y][x].position.y);
                            dest.width = 64;
                            dest.height = 64;

                            Vector2 origin = {0, 0};
                            DrawTexturePro(textures[TEXTURE_TILE_MAP], source, dest, origin, 0.0f, WHITE);
                            

                            // DrawText(TextFormat("%d, %d", info[y][x].parent_x, info[y][x].parent_y),
                            //     x * TILE_WIDTH + (TILE_WIDTH / 2), y * TILE_HEIGHT + (TILE_HEIGHT / 2), 16, WHITE);
                        }
                    }
                    
                    Rectangle source;
                    source.x = (float)tiles[12][12].texture_x * TILE_WIDTH;
                    source.y = (float)tiles[12][12].texture_y * TILE_HEIGHT;
                    source.width = (float)TILE_WIDTH;
                    source.height = (float)TILE_HEIGHT;
        
                    Rectangle dest;
                    dest.x = (float)(tiles[10][5].position.x);
                    dest.y = (float)(tiles[10][5].position.y);
                    dest.width = 64;
                    dest.height = 64;
                    Vector2 origin = {0, 0};
                    DrawTexturePro(textures[TEXTURE_TILE_MAP], source, dest, origin, 0.0f, WHITE);

                    // Draw the path if found
                    if (path_found) {
                        Color pathColor = ColorAlpha(BLUE, 0.5f);
                        drawPath(path, path_length, pathColor);
                    }
                    
                    // for (int i = 0; i < MAX_BULLETS; i++){
                    //     if (bullet[i].active == true){
                    //         shoot_bullet(&bullet[i].position, enemies[0].position, 5.0f);
                    //         DrawCircleV(bullet[i].position, bullet[i].radius, BLACK);
                    //     }
                    // }

                    // Draw the enemies
                    drawEnemies();
                    
                    // Draw the spawn point
                    // DrawCircleV(spawn_position, 10, GREEN);
                    
                    // Draw the goal point
                    // if (path_found) {
                    //     DrawCircleV(goal_position, 10, PURPLE);
                    // }

                    // Draw UI information
                    // char keyText[20];
                    // sprintf(keyText, "Keys: %d", key);
                    // DrawText(keyText, 20, 20, 20, WHITE);
                    
                    // DrawText("Left-click: Place wall", 20, 50, 20, WHITE);
                    // DrawText("Right-click: Set goal", 20, 80, 20, WHITE);
                } break;
                case ENDING:
                {
                    DrawRectangle(0, 0, screen_width, screen_height, BLACK);
                    DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
                } break;
                default: break;
            }
            
        EndDrawing();
    }
    
    // Unload resources
    UnloadTexture(textures[TEXTURE_TILE_MAP]);
    
    CloseWindow();
    return 0;
}