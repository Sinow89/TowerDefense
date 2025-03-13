// Created 2025-03-10 by Christoffer Rozenbachs

#define MAP_HEIGHT 45
#define MAP_WIDTH 90
#define TILE_HEIGHT 64
#define TILE_WIDTH 64
#define MAX_TEXTURES 1
#define MAX_QUEUE_SIZE (MAP_HEIGHT * MAP_WIDTH)

#include <stdio.h>
#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdlib.h>
#include "queue.h"

const int screen_width = 1792;
const int screen_height = 896;
const int key = 0;

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

typedef enum tiles_type{
    FLOOR,
    WALL,
    GOAL,
    DOOR,
    TREASURE,
} tiles_type;

typedef enum{
    TEXTURE_TILE_MAP = 0,
} texture_asset;

Texture2D textures[MAX_TEXTURES];

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

Vector2 move_towards_player(Vector2 enemy_pos, Vector2 target_pos, float speed, float delta_time) {
    Vector2 direction = Vector2Subtract(target_pos, enemy_pos);
    float distance = Vector2Length(direction);

    if (distance > 0) {
        direction = Vector2Normalize(direction);
        float move_distance = speed * delta_time;

        if (move_distance > distance) {
            return target_pos;
        } else {
            return Vector2Add(enemy_pos, Vector2Scale(direction, move_distance));
        }
    }
    return enemy_pos;
}

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

const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, 1, 0, -1};

// Structure to store path information
typedef struct {
    int parent_x;
    int parent_y;
    bool visited;
} PathInfo;

// Function to find path using BFS
bool findPath(tiles_t tiles[MAP_HEIGHT][MAP_WIDTH], Vector2 start, Vector2 goal, 
              Point *path, int *path_length) {
    
    // Convert start and goal positions to grid coordinates
    int start_x = (int)(start.x / TILE_WIDTH);
    int start_y = (int)(start.y / TILE_HEIGHT);
    int goal_x = (int)(goal.x / TILE_WIDTH);
    int goal_y = (int)(goal.y / TILE_HEIGHT);
    
    // Check bounds
    if (start_x < 0 || start_x >= MAP_WIDTH || start_y < 0 || start_y >= MAP_HEIGHT ||
        goal_x < 0 || goal_x >= MAP_WIDTH || goal_y < 0 || goal_y >= MAP_HEIGHT) {
        return false;
    }
    
    // Check if start or goal is a wall
    if (tiles[start_y][start_x].type == WALL || tiles[goal_y][goal_x].type == WALL) {
        return false;
    }
    
    // Initialize visited array and parent array
    PathInfo info[MAP_HEIGHT][MAP_WIDTH];
    int i, j;
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            info[i][j].visited = false;
            info[i][j].parent_x = -1;
            info[i][j].parent_y = -1;
        }
    }
    
    // Initialize queue
    Queue q;
    initQueue(&q);
    
    // Mark the start point as visited and enqueue it
    info[start_y][start_x].visited = true;
    enqueue(&q, start_x, start_y);
    
    // BFS loop
    bool found = false;
    while (!isEmpty(&q) && !found) {
        Point current = dequeue(&q);
        
        // Check if we've reached the goal
        if (current.x == goal_x && current.y == goal_y) {
            found = true;
            break;
        }
        
        // Check all four adjacent cells
        int i;
        for (i = 0; i < 4; i++) {
            int new_x = current.x + dx[i];
            int new_y = current.y + dy[i];
            
            // Check if the new position is valid
            if (new_x >= 0 && new_x < MAP_WIDTH && new_y >= 0 && new_y < MAP_HEIGHT && 
                !info[new_y][new_x].visited && tiles[new_y][new_x].type != WALL) {
                
                // Mark as visited and enqueue
                info[new_y][new_x].visited = true;
                info[new_y][new_x].parent_x = current.x;
                info[new_y][new_x].parent_y = current.y;
                enqueue(&q, new_x, new_y);
            }
        }
    }
    
    // If we found a path, reconstruct it
    if (found) {
        // Start from the goal
        int x = goal_x;
        int y = goal_y;
        int idx = 0;
        
        // Temporary path (will be in reverse order)
        Point temp_path[MAP_WIDTH * MAP_HEIGHT];
        
        // Backtrack from goal to start using the parent information
        while (x != -1 && y != -1) {
            temp_path[idx].x = x;
            temp_path[idx].y = y;
            idx++;
            
            int parent_x = info[y][x].parent_x;
            int parent_y = info[y][x].parent_y;
            x = parent_x;
            y = parent_y;
        }
        
        // Reverse the path to get from start to goal
        *path_length = idx;
        int i;
        for (i = 0; i < idx; i++) {
            path[i].x = temp_path[idx - 1 - i].x;
            path[i].y = temp_path[idx - 1 - i].y;
        }
        
        return true;
    }
    
    return false;
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

// Add these structs and variables to your existing code
typedef struct enemy_t {
    Vector2 position;
    Vector2 size;
    float speed;
    int health;
    bool active;
    int current_path_index;
} enemy_t;

#define MAX_ENEMIES 10
enemy_t enemies[MAX_ENEMIES];
Point path[MAP_WIDTH * MAP_HEIGHT];
int path_length = 0;
Vector2 goal_position = {0, 0};
bool path_found = false;

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
    UnloadImage(image);  // Good practice to unload the image after creating the texture
  
    while (!WindowShouldClose()) { 
        float delta_time = GetFrameTime();
        mouse_point = GetMousePosition();
        
        // Update tiles based on mouse input
        update_tiles_on_mouse(mouse_point, tiles);
        
        // Update the goal position when right-clicking
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
        
        // Update enemy positions
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