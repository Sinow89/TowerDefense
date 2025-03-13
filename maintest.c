// Created 2025-03-10 by Christoffer Rozenbachs

#define MAX_TEXTURES 1
#define MAX_ENEMIES 10

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "raylib.h"
#include "bfs.h"

const int screen_width = 1792;
const int screen_height = 896;
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
                }
                if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                    tiles[i][j].type = GOAL;
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
            break;
        }
    }
}

// Function to update all enemies
void updateEnemies(float delta_time) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            // Move the enemy along the path
            enemies[i].position = moveAlongPath(
                enemies[i].position, 
                path, 
                path_length, 
                &enemies[i].current_path_index, 
                enemies[i].speed, 
                delta_time
            );
            
            // Check if enemy reached the end of the path
            if (enemies[i].current_path_index >= path_length) {
                enemies[i].active = false;
                // Here you would typically subtract lives or handle enemy reaching the goal
            }
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
    goal_position.x = 0;
    goal_position.y = 0;
    
    // Initialize enemies
    initEnemies();
    
    // Set a starting position for spawning enemies
    Vector2 spawn_position;
    spawn_position.x = 200;
    spawn_position.y = 200;
    
    // Timer for spawning enemies
    float enemy_spawn_timer = 0;
    float enemy_spawn_interval = 3.0f; // Spawn an enemy every 3 seconds

    int frames_counter = 0;
    float speed = 100.0f;  // Increased speed for better visualization

    Image image = LoadImage("assets/towerDefense_tilesheet.png");
    textures[TEXTURE_TILE_MAP] = LoadTextureFromImage(image);
    UnloadImage(image);
  
    while (!WindowShouldClose()) { 
        float delta_time = GetFrameTime();
        mouse_point = GetMousePosition();
        

        update_tiles_on_mouse(mouse_point, tiles);
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            goal_position = mouse_point;
            // Find path from enemy spawn to goal
            path_found = findPath(tiles, spawn_position, goal_position, path, &path_length);
        }
        
        // Handle enemy spawning
        enemy_spawn_timer += delta_time;
        if (enemy_spawn_timer >= enemy_spawn_interval && path_found) {
            spawnEnemy(spawn_position);
            enemy_spawn_timer = 0;
        }
        
        updateEnemies(delta_time);

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

                    // Draw the map tiles
                    for (y = 0; y < MAP_HEIGHT; y++) {
                        for (x = 0; x < MAP_WIDTH; x++) {
                            switch (tiles[y][x].type) {
                                case FLOOR:
                                    texture_index_x = 15;
                                    texture_index_y = 1;
                                    break;
                                case WALL:
                                    texture_index_x = 11;
                                    texture_index_y = 1;
                                    break;
                                case GOAL:
                                    texture_index_x = 4;
                                    texture_index_y = 2;
                                    break;
                                case DOOR:
                                    texture_index_x = 2;
                                    texture_index_y = 4;
                                    break;
                                case TREASURE:
                                    texture_index_x = 10;
                                    texture_index_y = 5;
                                    break;
                                default:
                                    texture_index_x = 4;
                                    texture_index_y = 3;
                                    break;
                            }
                
                            Rectangle source;
                            source.x = (float)texture_index_x * TILE_HEIGHT;
                            source.y = (float)texture_index_y * TILE_WIDTH;
                            source.width = (float)TILE_WIDTH;
                            source.height = (float)TILE_HEIGHT;
                
                            Rectangle dest;
                            dest.x = (float)(tiles[y][x].position.x);
                            dest.y = (float)(tiles[y][x].position.y);
                            dest.width = 64;
                            dest.height = 64;
                
                            Vector2 origin = {0, 0};
                            DrawTexturePro(textures[TEXTURE_TILE_MAP], source, dest, origin, 0.0f, WHITE);
                        }
                    }
                    
                    // Draw the path if found
                    if (path_found) {
                        Color pathColor = ColorAlpha(BLUE, 0.5f);
                        drawPath(path, path_length, pathColor);
                    }
                    
                    // Draw the enemies
                    drawEnemies();
                    
                    // Draw the spawn point
                    DrawCircleV(spawn_position, 10, GREEN);
                    
                    // Draw the goal point
                    if (path_found) {
                        DrawCircleV(goal_position, 10, PURPLE);
                    }

                    // Draw UI information
                    char keyText[20];
                    sprintf(keyText, "Keys: %d", key);
                    DrawText(keyText, 20, 20, 20, WHITE);
                    
                    DrawText("Left-click: Place wall", 20, 50, 20, WHITE);
                    DrawText("Right-click: Set goal", 20, 80, 20, WHITE);
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