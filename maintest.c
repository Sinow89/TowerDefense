// Created 2025-03-10 by Christoffer Rozenbachs

#define MAX_TEXTURES 1
#define MAX_ENEMIES 10

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <float.h>
#include "raylib.h"
#include "bfs.h"
#include "map.h"
#include "tower.h"

const int screen_width = 12 * TILE_WIDTH;
const int screen_height = 12 * TILE_HEIGHT;
const int key = 0;
int path_length = 0;
Vector2 goal_position = {0, 0};
bool path_found = false;
int hp = 100;
int max_tower = 0;

typedef enum GameScreen{ 
    LOGO,
    TITLE,
    GAMEPLAY,
    ENDING
} GameScreen;

typedef enum {
    ENEMY_TANK,
    ENEMY_SOLDIER
} EnemyType;

typedef enum{
    TEXTURE_TILE_MAP = 0,
} texture_asset;

Texture2D textures[MAX_TEXTURES];

void update_tiles_on_mouse(Vector2 mouse_point, tiles_t tiles[MAP_HEIGHT][MAP_WIDTH],overlay_t overlayTiles[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            Rectangle tile_rect = { tiles[i][j].position.x, tiles[i][j].position.y, TILE_WIDTH, TILE_HEIGHT };

            if (CheckCollisionPointRec(mouse_point, tile_rect)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (max_tower >= 0 && max_tower <= 3) && tiles[i][j].type != SAND){
                    tiles[i][j].type = WALL;

                    overlayTiles[i][j].active = true;
                    overlayTiles[i][j].texture_x_body = 19;
                    overlayTiles[i][j].texture_y_body = 7;

                    overlayTiles[i][j].texture_x_cannon = 20;
                    overlayTiles[i][j].texture_y_cannon = 8;
                    max_tower++;
                }
                if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && tiles[i][j].type == WALL) {
                    tiles[i][j].type = FLOOR;

                    tiles[i][j].texture_x = 1;  tiles[i][j].texture_y = 1;
                    overlayTiles[i][j].active = false;
                    max_tower--;
                }
            }
        }
    }
}

// Function to visualize the path
void drawPath(Point *path, int path_length, Color color) {
    // int i;
    // for (i = 0; i < path_length; i++) {
    //     DrawRectangle(
    //         path[i].x * TILE_WIDTH + TILE_WIDTH/4,
    //         path[i].y * TILE_HEIGHT + TILE_HEIGHT/4,
    //         TILE_WIDTH/2,
    //         TILE_HEIGHT/2,
    //         color
    //     );
    // }
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
    EnemyType type;
    int texture_x_bottom;
    int texture_y_bottom;
    int texture_x_top;
    int texture_y_top;
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
        enemies[i].type = ENEMY_TANK;
        enemies[i].texture_x_bottom = 15;  // tiles[12][2] - Tank bottom
        enemies[i].texture_y_bottom = 11;
        enemies[i].texture_x_top = 15;     // tiles[12][3] - Tank top
        enemies[i].texture_y_top = 12;
    }
}

// Function to spawn a new enemy
void spawnEnemy(Vector2 position, EnemyType type) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].position = position;
            enemies[i].health = 250; 
            enemies[i].active = true;
            enemies[i].current_path_index = 0;
            enemies[i].velocity = (Vector2){0, 0};
            enemies[i].type = type;

            switch (type) {
                case ENEMY_TANK:
                    enemies[i].health = 250;
                    enemies[i].speed = 100.0f;
                    enemies[i].size = (Vector2){64, 64};
                    enemies[i].texture_x_bottom = 15;
                    enemies[i].texture_y_bottom = 11;
                    enemies[i].texture_x_top = 15; 
                    enemies[i].texture_y_top = 12;
                    break;
                case ENEMY_SOLDIER:
                    enemies[i].health = 100;
                    enemies[i].speed = 150.0f;
                    enemies[i].size = (Vector2){32, 32};
                    enemies[i].texture_x_bottom = 15;
                    enemies[i].texture_y_bottom = 10;
                    enemies[i].texture_x_top = 15;
                    enemies[i].texture_y_top = 10;
                    break;
            }
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
                hp = hp - 10;
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

float GetRotationFromVelocity(Vector2 velocity) {
    if (velocity.x == 0 && velocity.y == 0) return 0.0f;
    
    float angle = atan2f(velocity.y, velocity.x) * RAD2DEG;
    
    return angle;
}

void update_tower_rotation(tiles_t tiles[MAP_HEIGHT][MAP_WIDTH], overlay_t overlayTiles[MAP_HEIGHT][MAP_WIDTH], enemy_t enemies[MAX_ENEMIES], float delta_time) {
    static float fire_timer[MAP_HEIGHT][MAP_WIDTH] = {0};
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (overlayTiles[i][j].active) {
                Vector2 tower_pos = { 
                    tiles[i][j].position.x,
                    tiles[i][j].position.y
                };
                
                // Find nearest enemy
                float min_dist = FLT_MAX;
                Vector2 target_pos = tower_pos;  // Default to current position if no enemy found
                
                for (int e = 0; e < MAX_ENEMIES; e++) {
                    if (enemies[e].active) {
                        float dist = Vector2Distance(tower_pos, enemies[e].position);
                        if (dist < min_dist) {
                            min_dist = dist;
                            target_pos = enemies[e].position;
                        }
                    }
                }

                Vector2 direction = Vector2Subtract(target_pos, tower_pos);
                if (direction.x != 0 || direction.y != 0) {
                    float length = Vector2Length(direction);
                    Vector2 norm_dir = {direction.x / length, direction.y / length};
                    float angle = acosf(norm_dir.x) * RAD2DEG;
                    if (norm_dir.y < 0) {
                        angle = -angle;  // Up is negative y
                    }
                    overlayTiles[i][j].cannon_rotation = angle + 90.0f;
                    printf("Tower at (%f, %f), Target at (%f, %f), Direction (%f, %f), Angle: %f\n",
                           tower_pos.x, tower_pos.y, target_pos.x, target_pos.y,
                           direction.x, direction.y, angle);
                }

                fire_timer[i][j] += delta_time;
                float fire_rate = 1.0f;  // Shoot every 1 second - adjust as needed
                if (min_dist < 200.0f && fire_timer[i][j] >= fire_rate) {  // Range check (200 pixels)
                    fire_timer[i][j] = 0.0f;  // Reset timer
                    for (int b = 0; b < MAX_BULLETS; b++) {
                        if (!bullets[b].active) {
                            bullets[b].position = tower_pos;
                            bullets[b].velocity = Vector2Scale(Vector2Normalize(direction), bullets[b].speed);
                            bullets[b].active = true;
                            bullets[b].rotation = overlayTiles[i][j].cannon_rotation;
                            bullets[b].damage = 25;  // Ensure damage is set
                            printf("Bullet %d spawned at (%f, %f), Tower at (%f, %f), Rotation: %f\n",
                                b, bullets[b].position.x, bullets[b].position.y,
                                tower_pos.x, tower_pos.y, bullets[b].rotation);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void update_bullets(bullets_t bullets[MAX_BULLETS], enemy_t enemies[MAX_ENEMIES], float delta_time) {
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (bullets[b].active) {
            // Move bullet
            bullets[b].position = Vector2Add(bullets[b].position, Vector2Scale(bullets[b].velocity, delta_time));

            // Check collisions
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {
                    Rectangle rec = {enemies[i].position.x - 32, enemies[i].position.y - 32, 32, 32};  // Adjust size to match enemy
                    if (CheckCollisionCircleRec(bullets[b].position, bullets[b].radius, rec)) {
                        enemies[i].health -= 25;  // Deal damage - adjust as needed
                        if (enemies[i].health <= 0) {
                            enemies[i].active = false;
                        }
                        bullets[b].active = false;
                        break;
                    }
                }
            }

            // Deactivate if off-screen
            if (bullets[b].position.x < 0 || bullets[b].position.x > screen_width ||
                bullets[b].position.y < 0 || bullets[b].position.y > screen_height) {
                bullets[b].active = false;
            }
        }
    }
}

void draw_bullets(bullets_t bullets[MAX_BULLETS]) {
    for (int b = 0; b < MAX_BULLETS; b++) {
        if (bullets[b].active) {
            tiles_t bulletTile = {0};
            bulletTile.texture_x = 21;
            bulletTile.texture_y = 10;
            bulletTile.position = bullets[b].position;
            DrawTile(textures[TEXTURE_TILE_MAP], bulletTile, bullets[b].position, 1.0f, bullets[b].rotation);  // Scale 0.5 for smaller bullet
        }
    }
}

void drawEnemies(void) {
    int i;
    for (i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            Vector2 position;
            position.x = enemies[i].position.x - enemies[i].size.x/2;
            position.y = enemies[i].position.y - enemies[i].size.y/2;

            float rotation = GetRotationFromVelocity(enemies[i].velocity);
            float scale = (enemies[i].type == ENEMY_SOLDIER) ? 0.7f : 1.0f;

            tiles_t bottomTile = {0};
            bottomTile.texture_x = enemies[i].texture_x_bottom;
            bottomTile.texture_y = enemies[i].texture_y_bottom;
            bottomTile.position = position;

            tiles_t topTile = {0};
            topTile.texture_x = enemies[i].texture_x_top;
            topTile.texture_y = enemies[i].texture_y_top;
            topTile.position = position;

            DrawTile(textures[TEXTURE_TILE_MAP], bottomTile, position, scale, rotation);
            DrawTile(textures[TEXTURE_TILE_MAP], topTile, position, scale, rotation);

        }
    }
}

void draw_overlay_tiles(tiles_t tiles[MAP_HEIGHT][MAP_WIDTH], overlay_t overlayTiles[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            Vector2 position = { tiles[i][j].position.x, tiles[i][j].position.y };

            if (overlayTiles[i][j].active) {
                
                tiles_t bodyTile = {0};
                bodyTile.texture_x = overlayTiles[i][j].texture_x_body;
                bodyTile.texture_y = overlayTiles[i][j].texture_y_body;
                bodyTile.position = position;
                DrawTile(textures[TEXTURE_TILE_MAP], bodyTile, position, 1.0f, 0.0f);

                tiles_t cannonTile = {0};
                cannonTile.texture_x = overlayTiles[i][j].texture_x_cannon;
                cannonTile.texture_y = overlayTiles[i][j].texture_y_cannon;
                cannonTile.position = position;
                DrawTile(textures[TEXTURE_TILE_MAP], cannonTile, position, 1.0f, overlayTiles[i][j].cannon_rotation);
            }
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

    overlay_t overlayTiles[MAP_HEIGHT][MAP_WIDTH];
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            overlayTiles[i][j].active = false;
            overlayTiles[i][j].texture_x_body = 0;
            overlayTiles[i][j].texture_y_body = 0;
            overlayTiles[i][j].texture_x_cannon = 0;
            overlayTiles[i][j].texture_y_cannon = 0;
            overlayTiles[i][j].cannon_rotation = 0.0f;
        }
    }

    // Initialize bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = false;
        bullets[i].radius = 10.0f;
        bullets[i].texture_x = 21;
        bullets[i].texture_y = 10;
        bullets[i].speed = 300.0f;
        bullets[i].damage = 25;
        bullets[i].rotation = 0.0f;
        bullets[i].position = (Vector2){0, 0};
        bullets[i].velocity = (Vector2){0, 0};
    }

    InitWindow(screen_width, screen_height, "Tower Defense");
    SetTargetFPS(60);

    GameScreen current_screen = GAMEPLAY;
    Vector2 enemy_pos = {200, 200};
    Vector2 mouse_point = { 0.0f, 0.0f };
    
    // Initialize pathfinding variables
    path_found = false;
    path_length = 0;
    goal_position.x = 5 * TILE_WIDTH + (TILE_WIDTH / 2);
    goal_position.y = 10 * TILE_HEIGHT + (TILE_HEIGHT / 2);
    
    initEnemies();
    
    Vector2 spawn_position;
    spawn_position.x = 5 * TILE_WIDTH + (TILE_WIDTH / 2);
    spawn_position.y = 0 * TILE_HEIGHT + (TILE_HEIGHT / 2);
    
    float enemy_spawn_timer = 0;
    float enemy_spawn_interval = 3.0f;
    int spawn_counter = 0;

    int frames_counter = 3;

    Image image = LoadImage("assets/towerDefense_tilesheet.png");
    textures[TEXTURE_TILE_MAP] = LoadTextureFromImage(image);
    UnloadImage(image);
    
    PathInfo info[MAP_HEIGHT][MAP_WIDTH] = { 0 };
    
    while (!WindowShouldClose()) { 
        float delta_time = GetFrameTime();
        mouse_point = GetMousePosition();
        
        update_tiles_on_mouse(mouse_point, tiles, overlayTiles);


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
                if (hp == 0){
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
                    DrawText("Tower Defense", 100, 200, 75, WHITE);
                    DrawText("by Christoffer Rozenbachs", 100, 350, 20, GRAY);
                } break;
                case TITLE:
                {
                    DrawRectangle(0, 0, screen_width, screen_height, BLACK);
                    DrawText("How to play the game", 100, 150, 50, WHITE);
                    DrawText("Left-click to place Towers", 100, 250, 20, GRAY);
                    DrawText("Right-click to remove them", 100, 300, 20, GRAY);
                    DrawText("Enemies will spawn and follow the path to the goal", 100, 350, 20, GRAY);
                    DrawText("Press Space to start the game", 100, 400, 20, WHITE);
                } break;
                case GAMEPLAY:
                {
                    int texture_index_x = 0;
                    int texture_index_y = 0;
                    int x, y;

                    path_found = findPath(tiles, info, goal_position, spawn_position, path, &path_length);
        
                    // Handle enemy spawning
                    enemy_spawn_timer += delta_time;
                    if (enemy_spawn_timer >= enemy_spawn_interval && path_found) {
                        EnemyType type = (spawn_counter % 2 == 0) ? ENEMY_TANK : ENEMY_SOLDIER;
                        spawnEnemy(spawn_position, type);
                        enemy_spawn_timer = 0;
                        spawn_counter++;
                    }
                    
                    updateEnemies(delta_time, info);
                    update_tower_rotation(tiles, overlayTiles, enemies, delta_time);
                    update_bullets(bullets, enemies, delta_time);

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
                            
                            DrawText(TextFormat("Health: %d", hp), 5, 5, 30, WHITE);
                            // DrawText(TextFormat("%d, %d", info[y][x].parent_x, info[y][x].parent_y),
                            //     x * TILE_WIDTH + (TILE_WIDTH / 2), y * TILE_HEIGHT + (TILE_HEIGHT / 2), 16, WHITE);
                        }
                    }
                    draw_overlay_tiles(tiles,  overlayTiles);
                    draw_bullets(bullets);
                    DrawTile(textures[TEXTURE_TILE_MAP], tiles[12][12], (Vector2){320,640}, 1.0f, 0.0F);

                    // Draw the path if found
                    if (path_found) {
                        Color pathColor = ColorAlpha(BLUE, 0.5f);
                        drawPath(path, path_length, pathColor);
                    }

                    drawEnemies();

                } break;
                case ENDING:
                {
                    DrawRectangle(0, 0, screen_width, screen_height, BLACK);
                    DrawText("Game over general!", 100, 150, 60, RED);
                    DrawText("Press Enter to play again", 240, 300, 20, RED);
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