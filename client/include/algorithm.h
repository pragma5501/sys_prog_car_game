#ifndef __ALGO_H__
#define __ALGO_H__

#include <stdio.h>
#include <limits.h>
#include "type.h"


typedef enum { ITEM1 = 1, ITEM2, ITEM3, ITEM4, BOMB = -2 } _Item;
typedef enum { UP, DOWN, LEFT, RIGHT } Direction;

typedef struct {
    int row;
    int col;
    _Item item;
} Node_t;

typedef struct {
    Node_t map[MAP_ROW][MAP_COL];
    int start_row;
    int start_col;
} MAP_t;





void dfs(MAP_t* dgist, int visited[MAP_ROW][MAP_COL], int row, int col, int steps, int current_score, int path_length, Direction current_direction);

int getMaxScore(MAP_t* dgist, int start_row, int start_col, int** ret_best_path);


#endif