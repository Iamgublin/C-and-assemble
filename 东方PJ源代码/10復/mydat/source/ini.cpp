#include "../include/GV.h"

//一番最初の初期化
void first_ini(){
        configpad.down=0;
        configpad.left=1;
        configpad.right=2;
        configpad.up=3;
        configpad.bom=4;
        configpad.shot=5;
        configpad.slow=11;
        configpad.start=13;
        configpad.change=6;
}

//ゲームの初期化
void ini(){
        stage_count=0;
        memset(&ch,0,sizeof(ch_t));//自機データの初期化
        ch.x=FIELD_MAX_X/2;
        ch.y=FIELD_MAX_Y*3/4;
        memset(enemy,0,sizeof(enemy_t)*ENEMY_MAX);//敵データの初期化
}
