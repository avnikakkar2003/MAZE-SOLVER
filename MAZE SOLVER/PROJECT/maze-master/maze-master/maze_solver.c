#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX 100

char maze[MAX][MAX];
int visited[MAX][MAX];
int rows, cols;
int startX, startY, goalX, goalY;

// Directions: Right, Down, Left, Up
int dx[] = {0, 1, 0, -1};
int dy[] = {1, 0, -1, 0};

// ---------- Load Maze ----------
int load_maze(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open %s\n", filename);
        return 0;
    }

    rows = 0;
    while (fgets(maze[rows], MAX, file)) {
        cols = 0;
        while (maze[rows][cols] != '\n' && maze[rows][cols] != '\0') {
            if (maze[rows][cols] == 'S') { startX = rows; startY = cols; }
            if (maze[rows][cols] == 'G') { goalX = rows; goalY = cols; }
            cols++;
        }
        rows++;
    }
    fclose(file);
    return 1;
}

// ---------- Print Maze ----------
void print_maze() {
    for (int i = 0; i < rows; i++) {
        printf("%s", maze[i]);
    }
}

// ---------- DFS ----------
int solve_maze(int x, int y) {
    if (x < 0 || y < 0 || x >= rows || y >= cols || maze[x][y] == '#' || visited[x][y])
        return 0;

    if (x == goalX && y == goalY) return 1;

    visited[x][y] = 1;
    if (maze[x][y] != 'S') maze[x][y] = '*';

    for (int i = 0; i < 4; i++) {
        if (solve_maze(x + dx[i], y + dy[i])) return 1;
    }

    if (maze[x][y] != 'S') maze[x][y] = ' ';
    return 0;
}

// ---------- BFS ----------
typedef struct {
    int x, y;
} Point;

typedef struct {
    Point points[MAX * MAX];
    int front, rear;
} Queue;

void init_queue(Queue* q) {
    q->front = q->rear = 0;
}

bool is_empty(Queue* q) {
    return q->front == q->rear;
}

void enqueue(Queue* q, Point p) {
    q->points[q->rear++] = p;
}

Point dequeue(Queue* q) {
    return q->points[q->front++];
}

int bfs_solve_maze() {
    Queue q;
    init_queue(&q);
    enqueue(&q, (Point){startX, startY});
    visited[startX][startY] = 1;

    while (!is_empty(&q)) {
        Point p = dequeue(&q);
        int x = p.x, y = p.y;

        if (x == goalX && y == goalY) {
            maze[x][y] = 'G';
            return 1;
        }

        for (int i = 0; i < 4; i++) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && ny >= 0 && nx < rows && ny < cols &&
                maze[nx][ny] != '#' && !visited[nx][ny]) {

                visited[nx][ny] = 1;
                if (maze[nx][ny] != 'G') maze[nx][ny] = '*';
                enqueue(&q, (Point){nx, ny});
            }
        }
    }
    return 0;
}

// ---------- Main ----------
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <maze_file>\n", argv[0]);
        return 1;
    }

    if (!load_maze(argv[1])) return 1;

    // Backup original maze
    char original_maze[MAX][MAX];
    for (int i = 0; i < rows; i++) {
        strcpy(original_maze[i], maze[i]);
    }

    // ---------- DFS ----------
    printf("Solving using DFS:\n");
    if (solve_maze(startX, startY)) {
        print_maze();
    } else {
        printf("No path found using DFS.\n");
    }

    // Reset maze and visited
    for (int i = 0; i < rows; i++) {
        strcpy(maze[i], original_maze[i]);
        for (int j = 0; j < cols; j++) visited[i][j] = 0;
    }

    // ---------- BFS ----------
    printf("\nSolving using BFS:\n");
    if (bfs_solve_maze()) {
        print_maze();
    } else {
        printf("No path found using BFS.\n");
    }

    return 0;
}
