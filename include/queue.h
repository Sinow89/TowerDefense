#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE_SIZE (MAP_HEIGHT * MAP_WIDTH)

#include "tiles.h"

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point items[MAX_QUEUE_SIZE];
    int front, rear;
} Queue;

// Initialize the queue
void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
}

// Check if queue is empty
bool isEmpty(Queue *q) {
    return q->rear < q->front;
}

// Add to queue
void enqueue(Queue *q, int x, int y) {
    if (q->rear < MAX_QUEUE_SIZE - 1) {
        q->rear++;
        q->items[q->rear].x = x;
        q->items[q->rear].y = y;
    }
}

// Remove from queue
Point dequeue(Queue *q) {
    Point p = q->items[q->front];
    q->front++;
    return p;
}

#endif // queue.h