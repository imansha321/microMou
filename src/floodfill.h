#ifndef FLOODFILL_H
#define FLOODFILL_H

#include "walls.h"

char direction = 'N'; // current direction

const int rows = 16;
const int cols = 16;

// Define the WallMatrix to hold 4 data points per cell
int WallMatrix[rows][cols][4];
int Distances[rows][cols];

 // Directions: [top, right, bottom, left]
int dx[] = {-1, 0, 1, 0};
int dy[] = {0, 1, 0, -1};


int current_cell[2] = {15,0};



void InitializeDistances() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      Distances[i][j] = 255; // Maximum value for int
    }
  }

  // Set the zero points
  Distances[7][7] = 0;
  Distances[7][8] = 0;
  Distances[8][7] = 0;
  Distances[8][8] = 0;
}


void CalculateDistances() {
  InitializeDistances();

  // BFS for distance propagation
  struct Cell {
    int x, y;
    int distance;
  };

  Cell queue[rows * cols];
  int front = 0, rear = 0;

  // Add zero points to the queue
  queue[rear++] = {7, 7, 0};
  queue[rear++] = {7, 8, 0};
  queue[rear++] = {8, 7, 0};
  queue[rear++] = {8, 8, 0};

 

  while (front < rear) {
    Cell current = queue[front++];
    for (int d = 0; d < 4; d++) {
      int nx = current.x + dx[d];
      int ny = current.y + dy[d];

      // Check bounds
      if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
        // Check walls for accessibility
        if (WallMatrix[current.x][current.y][d] == 0 && WallMatrix[nx][ny][(d + 2) % 4] == 0) {
          // Update distance if new distance is smaller
          if (Distances[nx][ny] > current.distance + 1) {
            Distances[nx][ny] = current.distance + 1;
            queue[rear++] = {nx, ny, (int)(current.distance + 1)};
          }
        }
      }
    }
  }
}

void InitializeDistances2() {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      Distances[i][j] = 255; // Maximum value for int
    }
  }

  // Set the zero points
  Distances[15][0] = 0;
  
}


void CalculateDistances2() {
  InitializeDistances2();

  // BFS for distance propagation
  struct Cell {
    int x, y;
    int distance;
  };

  Cell queue[rows * cols];
  int front = 0, rear = 0;

  // Add zero points to the queue
  queue[rear++] = {15, 0, 0};
 

 

  while (front < rear) {
    Cell current = queue[front++];
    for (int d = 0; d < 4; d++) {
      int nx = current.x + dx[d];
      int ny = current.y + dy[d];

      // Check bounds
      if (nx >= 0 && nx < rows && ny >= 0 && ny < cols) {
        // Check walls for accessibility
        if (WallMatrix[current.x][current.y][d] == 0 && WallMatrix[nx][ny][(d + 2) % 4] == 0) {
          // Update distance if new distance is smaller
          if (Distances[nx][ny] > current.distance + 1) {
            Distances[nx][ny] = current.distance + 1;
            queue[rear++] = {nx, ny, (int)(current.distance + 1)};
          }
        }
      }
    }
  }
}


void UpdateWalls() {
    int x = current_cell[0];
    int y = current_cell[1];

    if (direction == 'N') {
        if (wallFront()) WallMatrix[x][y][0] = 1; // Top
        if (wallRight()) WallMatrix[x][y][1] = 1; // Right
        if (wallLeft()) WallMatrix[x][y][3] = 1;  // Left
    } else if (direction == 'E') {
        if (wallFront()) WallMatrix[x][y][1] = 1; // Right
        if (wallRight()) WallMatrix[x][y][2] = 1; // Bottom
        if (wallLeft()) WallMatrix[x][y][0] = 1;  // Top
    } else if (direction == 'S') {
        if (wallFront()) WallMatrix[x][y][2] = 1; // Bottom
        if (wallRight()) WallMatrix[x][y][3] = 1; // Left
        if (wallLeft()) WallMatrix[x][y][1] = 1;  // Right
    } else if (direction == 'W') {
        if (wallFront()) WallMatrix[x][y][3] = 1; // Left
        if (wallRight()) WallMatrix[x][y][0] = 1; // Top
        if (wallLeft()) WallMatrix[x][y][2] = 1;  // Bottom
    }
}
void UpdatePosition(char currentMove) {
    // Update the robot's direction and position based on the current move
    switch (direction) {
        case 'N': // Facing North
            if (currentMove == 'l') {
                direction = 'W'; // Turn left
            } else if (currentMove == 'r') {
                direction = 'E'; // Turn right
            } else if (currentMove == 'f') {
                current_cell[0]--; // Move forward (decrease row index)
            } else if (currentMove == 'b') {
                direction = 'S'; // Turn around (backtrack)
                current_cell[0]++; // Move backward (increase row index)
            }
            break;

        case 'S': // Facing South
            if (currentMove == 'l') {
                direction = 'E'; // Turn left
            } else if (currentMove == 'r') {
                direction = 'W'; // Turn right
            } else if (currentMove == 'f') {
                current_cell[0]++; // Move forward (increase row index)
            } else if (currentMove == 'b') {
                direction = 'N'; // Turn around (backtrack)
                current_cell[0]--; // Move backward (decrease row index)
            }
            break;

        case 'E': // Facing East
            if (currentMove == 'l') {
                direction = 'N'; // Turn left
            } else if (currentMove == 'r') {
                direction = 'S'; // Turn right
            } else if (currentMove == 'f') {
                current_cell[1]++; // Move forward (increase column index)
            } else if (currentMove == 'b') {
                direction = 'W'; // Turn around (backtrack)
                current_cell[1]--; // Move backward (decrease column index)
            }
            break;

        case 'W': // Facing West
            if (currentMove == 'l') {
                direction = 'S'; // Turn left
            } else if (currentMove == 'r') {
                direction = 'N'; // Turn right
            } else if (currentMove == 'f') {
                current_cell[1]--; // Move forward (decrease column index)
            } else if (currentMove == 'b') {
                direction = 'E'; // Turn around (backtrack)
                current_cell[1]++; // Move backward (increase column index)
            }
            break;

        default:
            // Reset to starting position if direction is invalid
            current_cell[0] = 0;
            current_cell[1] = 0;
            direction = 'N';
            break;
    }
}

char ChooseNextMove() {
    int x = current_cell[0];
    int y = current_cell[1];
    int minDistance = 255;
    int bestDirection = -1;

    // Check all four directions
    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d];
        int ny = y + dy[d];

        // Ensure the cell is within bounds and accessible
        if (nx >= 0 && nx < rows && ny >= 0 && cols && WallMatrix[x][y][d] == 0) {
            if (Distances[nx][ny] < minDistance) {
                minDistance = Distances[nx][ny];
                bestDirection = d;
            }
        }
    }

    // Handle the situation when the robot is trapped (no accessible cell)
    if (bestDirection == -1) {
        return 'b'; // Default to backtrack
    }

    // Determine the move based on the robot's current direction
    switch (direction) {
        case 'N': // Facing North
            if (bestDirection == 0) return 'f'; // Top
            if (bestDirection == 1) return 'r'; // Right
            if (bestDirection == 2) return 'b'; // Bottom (backtrack)
            if (bestDirection == 3) return 'l'; // Left
            break;
        case 'E': // Facing East
            if (bestDirection == 0) return 'l'; // Top
            if (bestDirection == 1) return 'f'; // Right
            if (bestDirection == 2) return 'r'; // Bottom
            if (bestDirection == 3) return 'b'; // Left (backtrack)
            break;
        case 'S': // Facing South
            if (bestDirection == 0) return 'b'; // Top (backtrack)
            if (bestDirection == 1) return 'l'; // Right
            if (bestDirection == 2) return 'f'; // Bottom
            if (bestDirection == 3) return 'r'; // Left
            break;
        case 'W': // Facing West
            if (bestDirection == 0) return 'r'; // Top
            if (bestDirection == 1) return 'b'; // Right (backtrack)
            if (bestDirection == 2) return 'l'; // Bottom
            if (bestDirection == 3) return 'f'; // Left
            break;
    }

    return 'f'; // Default to forward
}



#endif // FLOODFILL_H