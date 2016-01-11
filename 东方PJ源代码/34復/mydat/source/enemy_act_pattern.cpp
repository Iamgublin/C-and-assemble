#include "../include/GV.h"

//渡された-ang～angまでのランダムな角度を返す
double rang(double ang){
    return ( -ang + ang*2 * GetRand(10000)/10000.0 );
}

//移動パターン0
//下がってきて停滞して上がっていく
void enemy_pattern0(int i){
    int t=enemy[i].cnt;
    if(t==0)
        enemy[i].vy=3;//下がってくる
    if(t==40)
        enemy[i].vy=0;//止まる
    if(t==40+enemy[i].wait)//登録された停滞時間だけして
        enemy[i].vy=-3;//上がっていく
}

//移動パターン1
//下がってきて停滞して左下に行く
void enemy_pattern1(int i){
    int t=enemy[i].cnt;
    if(t==0)
        enemy[i].vy=3;//下がってくる
    if(t==40)
        enemy[i].vy=0;//止まる
    if(t==40+enemy[i].wait){//登録された停滞時間だけして
        enemy[i].vx=-1;//止まる
        enemy[i].vy=2;//上がっていく
        enemy[i].muki=0;//左向きセット
    }
}

//移動パターン2
//下がってきて停滞して右下に行く
void enemy_pattern2(int i){
    int t=enemy[i].cnt;
    if(t==0)
        enemy[i].vy=3;//下がってくる
    if(t==40)
        enemy[i].vy=0;//止まる
    if(t==40+enemy[i].wait){//登録された停滞時間だけして
        enemy[i].vx=1;//上がっていく
        enemy[i].vy=2;//止まる
        enemy[i].muki=2;//右向きセット
    }
}

//行動パターン3
//すばやく降りてきて左へ
void enemy_pattern3(int i){
    int t=enemy[i].cnt;
    if(t==0)
        enemy[i].vy=5;//下がってくる
    if(t==30){//途中で左向きに
        enemy[i].muki=0;
    }
    if(t<100){
        enemy[i].vx-=5/100.0;//左向き加速
        enemy[i].vy-=5/100.0;//減速
    }
}

//行動パターン4
//すばやく降りてきて左へ
void enemy_pattern4(int i){
    int t=enemy[i].cnt;
    if(t==0)
        enemy[i].vy=5;//下がってくる
    if(t==30){//途中で右向きに
        enemy[i].muki=2;
    }
    if(t<100){
        enemy[i].vx+=5/100.0;//右向き加速
        enemy[i].vy-=5/100.0;//減速
    }
}

//行動パターン5
//斜め左下へ
void enemy_pattern5(int i){
    int t=enemy[i].cnt;
    if(t==0){
        enemy[i].vx-=1;
        enemy[i].vy=2;
        enemy[i].muki=0;
    }
}

//行動パターン6
//斜め右下へ
void enemy_pattern6(int i){
    int t=enemy[i].cnt;
    if(t==0){
        enemy[i].vx+=1;
        enemy[i].vy=2;
        enemy[i].muki=2;
    }
}

//移動パターン7
//停滞してそのまま左上に
void enemy_pattern7(int i){
    int t=enemy[i].cnt;
    if(t==enemy[i].wait){//登録された停滞時間だけして
        enemy[i].vx=-0.7;//左へ
        enemy[i].vy=-0.3;//上がっていく
        enemy[i].muki=0;//左向き
    }
}

//移動パターン8
//停滞してそのまま右上に
void enemy_pattern8(int i){
    int t=enemy[i].cnt;
    if(t==enemy[i].wait){//登録された停滞時間だけして
        enemy[i].vx=+0.7;//右へ
        enemy[i].vy=-0.3;//上がっていく
        enemy[i].muki=2;//右向き
    }
}

//移動パターン9
//停滞してそのまま上に
void enemy_pattern9(int i){
    int t=enemy[i].cnt;
    if(t==enemy[i].wait)//登録された停滞時間だけして
        enemy[i].vy=-1;//上がっていく
}


//移動パターン10
//下がってきてウロウロして上がっていく
void enemy_pattern10(int i){
    int t=enemy[i].cnt;
    if(t==0) enemy[i].vy=4;//下がってくる
    if(t==40)enemy[i].vy=0;//止まる
    if(t>=40){
        if(t%60==00){
            int r=cos(enemy[i].ang)< 0 ? 0 : 1;
            enemy[i].sp=6+rang(2);
            enemy[i].ang=rang(PI/4)+PI*r;
            enemy[i].muki=2-2*r;
        }
        enemy[i].sp*=0.95;
    }
    if(t>=40+enemy[i].wait){
        enemy[i].vy-=0.05;
    }
}

