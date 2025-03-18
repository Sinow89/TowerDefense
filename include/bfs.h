#ifndef BFS_H
#define BFS_H
#include "queue.h"
#include "tiles.h"

const int dx[4] = {-1, 0, 1, 0};
const int dy[4] = {0, 1, 0, -1};
Point path[MAP_WIDTH * MAP_HEIGHT];

typedef struct {
    int parent_x;
    int parent_y;
    bool visited;
} PathInfo;

// Function to find path using BFS
bool findPath(tiles_t tiles[MAP_HEIGHT][MAP_WIDTH],
            PathInfo info[MAP_HEIGHT][MAP_WIDTH],
             Vector2 start, Vector2 goal, 
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

#endif // bfs.h