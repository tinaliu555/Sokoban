// by Yunghsien Chung (hiiragi4000)

// This file defines the functions declared in 'sokoban.h'.
// You don't need to know much about how it works.

#include<assert.h>
#include<stdbool.h>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<iostream>
#include"sokoban.h"

bool Get_Board(FILE *const fin, Board *const b_ptr){
    b_ptr->boxCnt = 0;
    b_ptr->id = 0;
    if(fscanf(fin, "%d%d ", &b_ptr->height, &b_ptr->width) != 2){
        return false;
    }
    b_ptr->m_cnt = 0;
    int i,j;
    for(i=0; i<=b_ptr->height-1; ++i){
        if(!fgets(b_ptr->board[i], WIDTH_MAX+2, fin)){
            assert(0);
        }
        b_ptr->board[i][b_ptr->width] = 0;
        for(j=0; j<=b_ptr->width-1; ++j) switch(b_ptr->board[i][j]){
            case '@':
                b_ptr->id |= ( (unsigned long long int) (i * b_ptr->width + j)) << 50;
            case '+':
                b_ptr->p_row = i, b_ptr->p_col = j;
                b_ptr->id |= ( (unsigned long long int) (i * b_ptr->width + j)) << 50;
                break;
            case '$':
                ++b_ptr->m_cnt;
                ++b_ptr->boxCnt;
                b_ptr->id |= (1ULL << ((i * b_ptr->width) + j));
                break;
            case '*':
                ++b_ptr->boxCnt;
                b_ptr->id |= (1ULL << ((i * b_ptr->width) + j));
                break;
        }
    }
    // std::cout<<"ID:"<<b_ptr->id<<std::endl;
    return true;
}
int const Di[]={-1, 1, 0, 0}, Dj[]={0, 0, -1, 1};
bool Inside(Board const *const b_ptr, int const i, int const j){
//	std::cout << "[Inside] height: " << b_ptr->height << " , weight : " << b_ptr->width << ", Player: ("<< i << "," << j << ")" <<std::endl;
    return 0<=i && i<=b_ptr->height-1 && 0<=j && j<=b_ptr->width-1 && b_ptr->board[i][j]!='#';
}
bool Do_move(Board *const b_ptr, Direction const dir, std::vector<Move> *const history){
    if(!Inside(b_ptr, b_ptr->p_row+Di[dir], b_ptr->p_col+Dj[dir])){
        return false;
    }
    char *curr = &b_ptr->board[b_ptr->p_row][b_ptr->p_col], *next = &b_ptr->board[b_ptr->p_row+Di[dir]][b_ptr->p_col+Dj[dir]];
    // "@-", "@.", "+-", "+."
    if(*next=='-' || *next=='.'){
        *curr = (*curr=='@')? '-': '.';
        *next = (*next=='-')? '@': '+';
        b_ptr->id ^= (unsigned long long int)(b_ptr->p_row * b_ptr->width + b_ptr->p_col) << 50;
        b_ptr->p_row += Di[dir], b_ptr->p_col += Dj[dir];
        b_ptr->id |= (unsigned long long int)(b_ptr->p_row * b_ptr->width + b_ptr->p_col) << 50;
        // std::cout<<"1ID:"<<b_ptr->id<<std::endl;
        if(history){
            history->emplace_back(Move(dir));
        }
        isPush = false;
        return true;
    }
    // "@$?", "@*?", "+$?", "+*?"
    if(!Inside(b_ptr, b_ptr->p_row+2*Di[dir], b_ptr->p_col+2*Dj[dir])){
        return false;
    }
    char *nnext = &b_ptr->board[b_ptr->p_row+2*Di[dir]][b_ptr->p_col+2*Dj[dir]];
    if(*nnext=='$' || *nnext=='*'){
        return false;
    }
    *curr = (*curr=='@')? '-': '.';
    if(*next == '$'){
        *next = '@';
        --b_ptr->m_cnt;
    }else{
        *next = '+';
    }
    if(*nnext == '-'){
        *nnext = '$';
        ++b_ptr->m_cnt;
    }else{
        *nnext = '*';
    }
    if(is2X2Deadlock(b_ptr->p_row + 2 * Di[dir], b_ptr->p_col + 2 * Dj[dir] ,b_ptr)){
        // for(int i=0;i<b_ptr->height;i++){
        //     for(int j=0;j<b_ptr->width;j++){
        //         std::cout<<b_ptr->board[i][j];
        //     }
        //     std::cout<<std::endl;
        // }
        // std::cout<<std::endl;
        return false;
    }
    b_ptr->id ^= (unsigned long long int)(b_ptr->p_row * b_ptr->width + b_ptr->p_col) << 50;
    b_ptr->p_row += Di[dir], b_ptr->p_col += Dj[dir];
    int newBoxNo = b_ptr->p_row * b_ptr->width + b_ptr->p_col;
    b_ptr->id |= (unsigned long long int)newBoxNo << 50;
    b_ptr->id ^= 1ULL << newBoxNo;
    b_ptr->id |= 1ULL << ((b_ptr->p_row + Di[dir])*b_ptr->width + (b_ptr->p_col + Dj[dir]));
    // std::cout<<"2ID:"<<b_ptr->id<<std::endl;
    // int newBoxX = b_ptr->p_row + Di[dir], newBoxY = b_ptr->p_col + Dj[dir];
    if(history){
        history->emplace_back(Move(dir));
    }
    isPush = true;
    return true;
}

bool Do_backward_move(Board *const b_ptr, Direction const dir, std::vector<Move> *const history, bool pushBox){
    if(!Inside(b_ptr, b_ptr->p_row-Di[dir], b_ptr->p_col-Dj[dir])){
        // std::cout<<"IN 1"<<std::endl;
        return false;
    }
    // std::cout<<"IN backward: dir:" << dir<<", pushBox="<<pushBox<<std::endl;
    // for(int i=0;i<b_ptr->height;++i){
    //  for(int j=0;j<b_ptr->width;++j){
    //          std::cout << b_ptr->board[i][j];
    //      }
    //      std::cout << std::endl;
    //  }
    char *curr = &b_ptr->board[b_ptr->p_row][b_ptr->p_col], *prev=&b_ptr->board[b_ptr->p_row-Di[dir]][b_ptr->p_col-Dj[dir]];
    if(*prev == '-'){
        *prev = '@';
    }else{
        if(*prev != '.'){
            // std::cout<<"IN 2"<<std::endl;
            return false;
        }
        *prev = '+';
    }

    if(pushBox){
        if(!Inside(b_ptr, b_ptr->p_row+Di[dir], b_ptr->p_col+Dj[dir])){
            // std::cout<<"IN 3"<<std::endl;
            return false;
        }
        char *next = &b_ptr->board[b_ptr->p_row+Di[dir]][b_ptr->p_col+Dj[dir]];
        if(*curr == '@'){
            *curr = '$';
            ++b_ptr->m_cnt;
        }else{
            *curr = '*';
        }
        if(*next == '$'){
            *next = '-';
            --b_ptr->m_cnt;
        }else{
            if(*next != '*'){
                // std::cout<<"IN 4"<<std::endl;
                return false;
            }
            *next = '.';
        }
        
        b_ptr->id ^= 1ULL << ((b_ptr->p_row + Di[dir])*b_ptr->width + (b_ptr->p_col + Dj[dir]));
        b_ptr->id |= 1ULL << (b_ptr->p_row * b_ptr->width + b_ptr->p_col);
    }else{
        *curr = (*curr=='@')? '-': '.';
    }
    b_ptr->id ^= (unsigned long long int)(b_ptr->p_row * b_ptr->width + b_ptr->p_col) << 50;
    b_ptr->p_row -= Di[dir], b_ptr->p_col -= Dj[dir];
    b_ptr->id |= (unsigned long long int)(b_ptr->p_row * b_ptr->width + b_ptr->p_col) << 50;
    if(history){
        std::vector<Move>::iterator it = history->begin();
        history->insert(it,Move(dir));
    }
    // std::cout<<"Out backward: dir:" << dir<<", pushBox="<<pushBox<<std::endl;
    // for(int i=0;i<b_ptr->height;++i){
    //      for(int j=0;j<b_ptr->width;++j){
    //          std::cout << b_ptr->board[i][j];
    //      }
    //      std::cout << std::endl;
    //  }
    // std::cout<<"IN 5"<<std::endl;
    return true;
}
// void Undo(Board *const b_ptr, Move const mv){
//     assert(Inside(b_ptr, b_ptr->p_row-Di[mv.dir], b_ptr->p_col-Dj[mv.dir]));
//     char *curr = &b_ptr->board[b_ptr->p_row][b_ptr->p_col], *prev=&b_ptr->board[b_ptr->p_row-Di[mv.dir]][b_ptr->p_col-Dj[mv.dir]];
    // if(*prev == '-'){
    //     *prev = '@';
    // }else{
    //     assert(*prev == '.');
    //     *prev = '+';
    // }
//     if(mv.push){
//         assert(Inside(b_ptr, b_ptr->p_row+Di[mv.dir], b_ptr->p_col+Dj[mv.dir]));
//         char *next = &b_ptr->board[b_ptr->p_row+Di[mv.dir]][b_ptr->p_col+Dj[mv.dir]];
//         if(*curr == '@'){
//             *curr = '$';
//             ++b_ptr->m_cnt;
//         }else{
//             *curr = '*';
//         }
//         if(*next == '$'){
//             *next = '-';
//             --b_ptr->m_cnt;
//         }else{
//             assert(*next == '*');
//             *next = '.';
//         }
//     }else{
//         *curr = (*curr=='@')? '-': '.';
//     }
//     b_ptr->p_row -= Di[mv.dir], b_ptr->p_col -= Dj[mv.dir];
// }
