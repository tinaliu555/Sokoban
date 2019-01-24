#include<stdio.h>
#include"sokoban.h"

Board Brd;
Vector_Move History;

bool Dfs(int d){
    if(!Brd.m_cnt){
        return true;
    }
    if(!d){
        return false;
    }
    for(int i=0; i<=3; ++i){
        if(Do_move(&Brd, i, &History)){
            if(Dfs(d-1)){
                return true;
            }
            Undo(&Brd, History.data[History.size-1]);
            VM_pop_back(&History);
        }
    }
    return false;
}

void Iddfs(){
    for(int i=1; ; ++i){
        if(Dfs(i)){
            return;
        }
    }
}

int main(){
    while(Get_Board(stdin, &Brd)){
        Iddfs();
        printf("%d\n", History.size);
        for(int i=0; i<History.size; ++i){
            putchar("udlr"[History.data[i].dir]);
        }
        putchar('\n');
        VM_clear(&History);
    }
    return 0;
}
