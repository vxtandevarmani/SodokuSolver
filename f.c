#include <stdlib.h>

#define BOX_GRID 9
#define MAX_EMPTY BOX_GRID*BOX_GRID
#define BOX_INDEX(r, c) ((r/3)*3 + (c/3))

typedef struct{
    size_t row;
    size_t col;
    uint8_t moves;
} coords;

int comp(const void* a, const void* b){
    return ((coords*)a)->moves - ((coords*)b)->moves;
}
int CheckValid( unsigned short* rowMask, 
                unsigned short* colMask,
                unsigned short* boxMask,
                unsigned char number,
                unsigned long r,
                unsigned long c 
                ){
    unsigned short bit = 1 << (number - 1);
    return !( (rowMask[r] & bit) || (colMask[c] & bit) || (boxMask[BOX_INDEX(r,c)] & bit) );
}

int init(char** board,
         unsigned short* rowMask,
         unsigned short* colMask,
         unsigned short* boxMask,
         coords* empties,
         unsigned long* counter
         ){
    for(int r = 0; r < 9; r++){
        for(int c = 0; c < 9; c++){
            if(board[r][c] == '.'){
                if(*counter > 81){
                    return 0;
                }
                empties[*counter].row = r;
                empties[*counter].col = c;
                (*counter)++;
                continue;
            }
            unsigned char n = board[r][c] - '0';
            if(!CheckValid(rowMask, colMask, boxMask, n, r, c)){
                return 0;
            }
            unsigned short bit = 1 << (n - 1);
            rowMask[r] |= bit;
            colMask[c] |= bit;
            boxMask[BOX_INDEX(r, c)] |= bit;
        }
    }
    for(int i = 0; i < *counter; i++){
        int row = empties[i].row;
        int col = empties[i].col;
        unsigned short used = rowMask[row] | colMask[col] | boxMask[BOX_INDEX(row, col)];
        empties[i].moves = 9 - __builtin_popcount(used);
    }
    qsort(empties, *counter, sizeof(empties[0]), comp);
    return 1;
}

int sol(
    char** board,
    unsigned short* rowMask,
    unsigned short* colMask,
    unsigned short* boxMask,
    coords* empties,
    unsigned long counter
){
    unsigned char stack[81] = {0};
    int sPointer = 0;
    while(sPointer < counter){
        int row = empties[sPointer].row;
        int col = empties[sPointer].col;
        for(int i = stack[sPointer] + 1; i <= 9; i++){
            if(!CheckValid(rowMask, colMask, boxMask, i, row, col)){
                continue;
            }
            unsigned short bit = 1 << (i - 1);
            board[row][col] = '0' + i;
            rowMask[row] |= bit;
            colMask[col] |= bit;
            boxMask[BOX_INDEX(row, col)] |= bit;
            stack[sPointer] = i;
            sPointer++;
            break;
        }
        if(board[row][col] == '.'){
            stack[sPointer] = 0;
            if(sPointer <= 0){
                return 0;
            }
            sPointer--;
            int rowPrev = empties[sPointer].row;
            int colPrev = empties[sPointer].col;
            unsigned short bits = 1 << (stack[sPointer] -1);
            board[rowPrev][colPrev] = '.';
            rowMask[rowPrev] &= ~bits;
            colMask[colPrev] &= ~bits;
            boxMask[BOX_INDEX(rowPrev, colPrev)] &= ~bits;
        }
    }
    return 1;
}

void solveSudoku(char** board, int boardSize, int* boardColSize) {
    unsigned short rowMask[9] = {0};
    unsigned short colMask[9] = {0};
    unsigned short boxMask[9] = {0};
    unsigned long counter = 0;
    coords* empties = (coords*)calloc(81, sizeof(coords));
    if(!empties){
        return;
    }
    if(!init(board, rowMask, colMask, boxMask, empties, &counter)){
        free(empties);
        return;
    }
    if(!sol(board, rowMask, colMask, boxMask, empties, counter)){
        free(empties);
        return;
    }
}
