// by Yunghsien Chung (hiiragi4000)

// This file defines the structs needed and declares some functions.
// All functions declared in this file are defined in 'sokoban.c'.

#ifndef SOKOBAN_H
#define SOKOBAN_H

#include<stdbool.h>
#include<stdio.h>
#include<vector>

#define HEIGHT_MAX 15
#define WIDTH_MAX 15
#define AREA_MAX 50

// extern bool canAchieveBrd[HEIGHT_MAX][WIDTH_MAX+2];
extern bool isPush;
typedef struct{
    int height, width;
    char board[HEIGHT_MAX][WIDTH_MAX+2]; // +2 for LF and NULL characters
    // m_cnt: number of '$'s
    // the player is at (p_row, p_col)
    int m_cnt, p_row, p_col;
    int boxCnt;
    unsigned long long int id;
}Board;

// read a 'Board' from 'fin'
extern bool Get_Board(FILE *const fin, Board *const b_ptr);

// UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3
typedef enum{
    UP, DOWN, LEFT, RIGHT
}Direction;

class Move{
public:
    Direction dir;
    Move(Direction d){
        dir = d;
    }
};

// whether (i, j) is not outside the board '*b_ptr' and is not '#'
extern bool Inside(Board const *const b_ptr, int const i, int const j);

// make a move on the board '*b_ptr' in the direction 'dir'
// the function returns true iff 'dir' is valid
// if 'history' is nonNULL and 'dir' is valid, append the move to '*history'
extern bool Do_move(Board *const b_ptr, Direction const dir, std::vector<Move> *const history);


extern bool Do_backward_move(Board *const b_ptr, Direction const dir, std::vector<Move> *const history, bool pushBox);
// given the last move 'mv', undo the board '*b_ptr'
// the function asserts the validity of 'mv'
// extern void Undo(Board *const b_ptr, Move const mv);

extern bool is2X2Deadlock(int const x, int const y, Board *const b_ptr);
#endif
