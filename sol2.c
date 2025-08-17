#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define THRESHOLD 5
#define BOX_GRID 9
#define MAX_EMPTY BOX_GRID*BOX_GRID
#define scale sqrtf(BOX_GRID)
#define BOX_INDEX(r, c) ((r/3)*3 + (c/3))

typedef struct{
    size_t row;
    size_t col;
    uint8_t moves;
} coords;

int comp(const void* a, const void* b){
    return ((coords*)a)->moves - ((coords*)b)->moves;
}
bool CheckValid(uint16_t* rowMask, uint16_t* colMask, uint16_t* boxMask, uint8_t number, size_t r, size_t c){
    uint16_t bit = 1 << (number - 1);
    return !( (rowMask[r] & bit) || (colMask[c] & bit) || (boxMask[BOX_INDEX(r,c)] & bit) );
}
bool init(uint8_t board[BOX_GRID][BOX_GRID], uint16_t* rowMask, uint16_t* colMask, uint16_t* boxMask, coords* empties, size_t* counter){
    for(size_t r = 0; r < BOX_GRID; r++){
        for(size_t c = 0; c < BOX_GRID; c++){
            if(board[r][c]){
                if (!CheckValid(rowMask, colMask, boxMask, board[r][c], r, c)){
                    puts("Invalid board detected");
                    return false;
                }
                uint16_t bit = 1 << (board[r][c] - 1);
                rowMask[r] |= bit;
                colMask[c] |= bit;
                boxMask[BOX_INDEX(r, c)] |= bit;
            }else{
                empties[*counter].row = r;
                empties[*counter].col = c;
                (*counter)++;
            }
        }
    }
    coords* tmp = (coords*)realloc(empties, (*counter) * sizeof(coords));
    if(!tmp){
        free(empties);
        perror("calloc");
        return false;
    }
    empties = tmp;
    for(size_t i = 0; i < *counter; i++){
        size_t row = empties[i].row;
        size_t col = empties[i].col;
        uint16_t used = rowMask[row] | colMask[col] | boxMask[BOX_INDEX(row, col)];
        empties[i].moves = 9 - __builtin_popcount(used);
    }
    qsort(empties, *counter, sizeof(coords), comp);
    return true;
}
bool SolveSodoku(uint8_t board[BOX_GRID][BOX_GRID], uint16_t* rowMask, uint16_t* colMask, uint16_t* boxMask, coords* empties, size_t counter){
    size_t r       = SIZE_MAX;
    size_t c       = SIZE_MAX;
    size_t idx     = SIZE_MAX;
    for(size_t i = 0; i < counter; i++){
        if(empties[i].row != SIZE_MAX){
            r = empties[i].row;
            c = empties[i].col;
            idx = i;
            goto found;
        }
    }
    found:
    if(r == SIZE_MAX){
        return true;
    }
    for(uint8_t i = 1; i <= 9; i++){
        if(CheckValid(rowMask, colMask, boxMask, i, r, c)){
            uint16_t bit = 1 << (i - 1);

            board[r][c] = i;
            rowMask[r] |= bit;
            colMask[c] |= bit;
            boxMask[BOX_INDEX(r, c)] |= bit;
            empties[idx].row = SIZE_MAX;
            empties[idx].col = SIZE_MAX;

            if(SolveSodoku(board, rowMask, colMask, boxMask, empties, counter))
                return true;

            board[r][c] = 0;
            rowMask[r] &= ~bit;
            colMask[c] &= ~bit;
            boxMask[BOX_INDEX(r, c)] &= ~bit;
            empties[idx].row = r;
            empties[idx].col = c;
        }
    }
    return false;
}
void print_sudoku(uint8_t board[BOX_GRID][BOX_GRID]) {
    for (int i = 0; i < BOX_GRID; i++) {
        for (int j = 0; j < BOX_GRID; j++) {
            if (j == 3 || j == 6)
                printf("| ");
            if (!board[i][j])
                printf(". ");
            else
                printf("%d ", board[i][j]);
        }
        puts("");
        if (i == 2 || i == 5)
            puts("---------------------");
    }
}
int main(void){
    static uint16_t rowMask[BOX_GRID]          = {0};
    static uint16_t colMask[BOX_GRID]          = {0};
    static uint16_t boxMask[BOX_GRID]          = {0};
    coords* empties = (coords*)calloc(81, sizeof(coords));
    size_t counter = 0;
    if(!empties){
        perror("calloc");
        return 1;
    }
    uint8_t board[9][9] = {
        {5, 3, 0,  0, 7, 0,  0, 0, 0},
        {6, 0, 0,  1, 9, 5,  0, 0, 0},
        {0, 9, 8,  0, 0, 0,  0, 6, 0},

        {8, 0, 0,  0, 6, 0,  0, 0, 3},
        {4, 0, 0,  8, 0, 3,  0, 0, 1},
        {7, 0, 0,  0, 2, 0,  0, 0, 6},

        {0, 6, 0,  0, 0, 0,  2, 8, 0},
        {0, 0, 0,  4, 1, 9,  0, 0, 5},
        {0, 0, 0,  0, 8, 0,  0, 7, 9},
    };
    if(!init(board, rowMask, colMask, boxMask, empties, &counter)){
        free(empties);
        return 1;
    }
    if(SolveSodoku(board, rowMask, colMask, boxMask, empties, counter)){
        free(empties);
        print_sudoku(board);
        return 0;
    }else{
        free(empties);
        puts("[-] The state of the sodoku board is unsolvable!");
        return 1;
    }
}       // gcc sol.c -o ape -Wall -Werror -pedantic -flto -O2
