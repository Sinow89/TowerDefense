#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 100

// Structure for the queue
typedef struct {
    int items[MAX_NODES];
    int front, rear;
} Queue;

// Queue functions
void initQueue(Queue *q) {
    q->front = 0;
    q->rear = -1;
}

bool isEmpty(Queue *q) {
    return q->rear < q->front;
}

void enqueue(Queue *q, int value) {
    if (q->rear < MAX_NODES - 1) {
        q->items[++q->rear] = value;
    }
}

int dequeue(Queue *q) {
    if (!isEmpty(q)) {
        return q->items[q->front++];
    }
    return -1; // Error case
}

// Graph representation
typedef struct {
    int vertices;
    int adj[MAX_NODES][MAX_NODES];
} Graph;

void initGraph(Graph *g, int vertices) {
    g->vertices = vertices;
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            g->adj[i][j] = 0;
        }
    }
}

void addEdge(Graph *g, int src, int dest) {
    g->adj[src][dest] = 1;
    g->adj[dest][src] = 1; // For undirected graph
}

// BFS function
void bfs(Graph *g, int startVertex) {
    bool visited[MAX_NODES] = {false};
    Queue q;
    initQueue(&q);

    visited[startVertex] = true;
    enqueue(&q, startVertex);

    while (!isEmpty(&q)) {
        int currentVertex = dequeue(&q);
        printf("%d ", currentVertex);

        for (int i = 0; i < g->vertices; i++) {
            if (g->adj[currentVertex][i] == 1 && !visited[i]) {
                visited[i] = true;
                enqueue(&q, i);
            }
        }
    }
    printf("\n");
}

int main() {
    Graph g;
    initGraph(&g, 6);

    addEdge(&g, 0, 1);
    addEdge(&g, 0, 2);
    addEdge(&g, 1, 3);
    addEdge(&g, 1, 4);
    addEdge(&g, 2, 5);

    printf("BFS Traversal starting from vertex 0: ");
    bfs(&g, 0);

    return 0;
}
