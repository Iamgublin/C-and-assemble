#include "../include/GV.h"

#define FLAME 60

//fpsのカウンタ、60フレームに1回基準となる時刻を記録する変数
int fps_count,count0t;
//平均を計算するため60回の1周時間を記録
int f[FLAME];
//平均fps
double ave;

void fps_wait(){
    int term,i,gnt;
    static int t=0;
    if(fps_count==0){//60フレームの1回目なら
        if(t==0)//完全に最初ならまたない
            term=0;
        else//前回記録した時間を元に計算
            term=count0t+1000-GetNowCount();
    }
    else    //待つべき時間=現在あるべき時刻-現在の時刻
        term = (int)(count0t+fps_count*(1000.0/FLAME))-GetNowCount();
    if(term>0)//待つべき時間だけ待つ
        Sleep(term);
    gnt=GetNowCount();
    if(fps_count==0)//60フレームに1度基準を作る
        count0t=gnt;
    f[fps_count]=gnt-t;//１周した時間を記録
    t=gnt;
    //平均計算
    if(fps_count==FLAME-1){
        ave=0;
        for(i=0;i<FLAME;i++)
            ave+=f[i];
        ave/=FLAME;
    }
    fps_count = (++fps_count)%FLAME ;
}

void draw_fps(int x, int y){
    if(ave!=0){
        DrawFormatString(x, y,color[0],"[%.1f]",1000/ave);
    }
    return;
}