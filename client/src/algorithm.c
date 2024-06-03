#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "algorithm.h"

int max_score = INT_MIN;
const int directions[4][2] = {{0,1}, {1,0}, {0,-1}, {-1,0}};
char* direction_names[4] = {"RIGHT", "DOWN", "LEFT", "UP"};

static int best_path[5][2];
static int current_path[5][2];


static Direction best_directions[4];
static Direction current_directions[4];

void dfs(MAP_t* dgist, int visited[MAP_ROW][MAP_COL], int row, int col, int steps, int current_score, int path_length, Direction current_direction) {
    if (steps == 0) {
        if (current_score > max_score) {
            max_score = current_score;
            for (int i = 0; i < path_length; ++i) {
                best_path[i][0] = current_path[i][0];
                best_path[i][1] = current_path[i][1];
                if (i > 0) {
                    best_directions[i-1] = current_directions[i-1];
                }
            }
        }
        return;
    }

    for (int i = 0; i < 4; ++i) {
        int new_row = row + directions[i][0];
        int new_col = col + directions[i][1];

        if (new_row >= 0 && new_row < MAP_ROW && new_col >= 0 && new_col < MAP_COL && !visited[new_row][new_col]) {
            visited[new_row][new_col] = 1;
            current_path[path_length][0] = new_row;
            current_path[path_length][1] = new_col;
            current_directions[path_length-1] = (Direction)i;
            dfs(dgist, visited, new_row, new_col, steps - 1, current_score + dgist->map[new_row][new_col].item, path_length + 1, (Direction)i);
            visited[new_row][new_col] = 0;
        }
    }
}

int getMaxScore(MAP_t* dgist, int start_row, int start_col, int** ret_best_path) {
    int visited[MAP_ROW][MAP_COL] = {0};
    visited[start_row][start_col] = 1;
    current_path[0][0] = start_row;
    current_path[0][1] = start_col;
    dfs(dgist, visited, start_row, start_col, 4, dgist->map[start_row][start_col].item, 1, RIGHT);
    
    memcpy(ret_best_path, best_path, 5 * 2 * sizeof(int));

    return max_score;
}