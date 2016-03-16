#include "../include/GV.h"

//“G‚ÌˆÚ“®ƒpƒ^[ƒ“0‚Å‚ÌˆÚ“®§Œä
void enemy_pattern0(int i){
    if(enemy[i].cnt<60){
        enemy[i].y+=2.0;
    }
    if(enemy[i].cnt>60+240){
        enemy[i].y-=2.0;
    }
}

//“Gƒf[ƒ^‚Ì“o˜^
void enemy_enter(){
    if(stage_count==100){//ƒQ[ƒ€‚ªn‚Ü‚Á‚Ä100ƒJƒEƒ“ƒg‚Å“o˜^
        enemy[0].cnt    =0;
        enemy[0].muki   =1;
        enemy[0].flag   =1;
        enemy[0].bltime =150;
        enemy[0].hp     =1000;
        enemy[0].hp_max =enemy[0].hp;
        enemy[0].pattern=0;
        enemy[0].x      =FIELD_MAX_X/2;
        enemy[0].y      =-20;
    }
}

//“G‚Ìs“®§Œä
void enemy_act(){
    int i;
    for(i=0;i<ENEMY_MAX;i++){
        if(enemy[i].flag==1){//‚»‚Ì“G‚Ìƒtƒ‰ƒO‚ªƒIƒ“‚É‚È‚Á‚Ä‚½‚ç
            enemy_pattern0(i);
            enemy[i].cnt++;
            enemy[i].img=enemy[i].muki*3+(enemy[i].cnt%18)/6;
            //“G‚ª‰æ–ÊŠO‚ÉŠO‚ê‚½‚çÁ‚·
            if(enemy[i].x<-50 || FIELD_MAX_X+50<enemy[i].x || enemy[i].y<-50 || FIELD_MAX_Y+50<enemy[i].y)
                enemy[i].flag=0;
        }
    }
}

//“Gˆ—ƒƒCƒ“
void enemy_main(){
    enemy_enter();
    enemy_act();
}

