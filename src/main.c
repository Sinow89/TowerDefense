// Created 2025-03-10 by Christoffer Rozenbachs

#include <stdio.h>
#include "raylib.h"
#include "raymath.h"

#define MAP_HEIGHT 45
#define MAP_WIDTH 90
#define TILE_HEIGHT 64
#define TILE_WIDTH 64
#define MAX_TEXTURES 1

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

int main(void) {

    create_tiles();

    InitWindow(screen_width, screen_height, "Tower Defense");
    SetTargetFPS(60);

    GameScreen current_screen = GAMEPLAY;
    Vector2 enemy_pos = {200,200};
    Vector2 mouse_point = { 0.0f, 0.0f };

    int frames_counter = 0;
    float speed = 0.05f;

    Image image = LoadImage("assets/towerDefense_tilesheet.png");
    textures[TEXTURE_TILE_MAP] = LoadTextureFromImage(image);
  
    while (!WindowShouldClose()) { 

        float delta_time = GetFrameTime();
        mouse_point = GetMousePosition();
        
        update_tiles_on_mouse(mouse_point, tiles);

        switch (current_screen)
        {
            case LOGO:
            {
                frames_counter++;
                if (frames_counter > 240)
                {
                    current_screen = TITLE;
                }
            } break;
            case TITLE:
            {
                if (IsKeyPressed(KEY_SPACE))
                {
                    current_screen = GAMEPLAY;
                }
            } break;
            case GAMEPLAY:
            {
                if (IsKeyPressed(KEY_Q))
                {
                    current_screen = ENDING;
                }
            } break;
            case ENDING:
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    current_screen = TITLE;
                }
            } break;
            default: break;
        }

        BeginDrawing();

            ClearBackground(RAYWHITE);

            switch(current_screen)
            {
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
                    DrawText("You walk with WASD", 400, 250, 20, GRAY);
                    DrawText("To pick up things you press E", 400, 300, 20, GRAY);
                    DrawText("In order the progress you have to pick up the key before", 400, 350, 20, GRAY);
                    DrawText("Press Space to start the game", 600, 600, 20, WHITE);

                } break;
                case GAMEPLAY:
                {
                    
                    int texture_index_x = 0;
                    int texture_index_y = 0;

                    for (int y = 0; y < MAP_HEIGHT; y++) {
                        for (int x = 0; x < MAP_WIDTH; x++) {
                
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
                
                            Rectangle source = {
                                (float)texture_index_x * TILE_HEIGHT,
                                (float)texture_index_y * TILE_WIDTH,
                                (float)TILE_WIDTH,
                                (float)TILE_HEIGHT
                            };
                
                            Rectangle dest = {
                                (float)(tiles[y][x].position.x),
                                (float)(tiles[y][x].position.y),
                                64,
                                64
                            };
                
                            Vector2 origin = {0, 0};
                            DrawTexturePro(textures[TEXTURE_TILE_MAP], source, dest, origin, 0.0f, WHITE);

                            Vector2 player_position = GetMousePosition();

                            Vector2 size = {64, 64};
                            player_position.x -= size.x / 2;
                            player_position.y -= size.y / 2;
                         
                            // enemy_pos = move_towards_player(enemy_pos, player_position, speed, delta_time);

                            // DrawRectangleV(player_position, size, GREEN);

                            // DrawRectangleV(enemy_pos, size, RED);
                            
                        }
                    }

                    char keyText[20];
                    sprintf(keyText, "Keys: %d", key);
                    DrawText(keyText, 20, 20, 20, WHITE);

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
    CloseWindow();
}