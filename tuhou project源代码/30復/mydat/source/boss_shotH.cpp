#include "../include/GV.h"

extern int search_boss_shot();
extern double bossatan2();
extern int move_boss_pos(double x1,double y1,double x2,double y2,double dist, int t);
extern void input_phy_pos(double x,double y,int t);

void boss_shot_bulletH000(){
#define TM000 120
    int i,k,t=boss_shot.cnt%TM000;
    double angle;
    if(t<60 && t%10==0){
        angle=bossatan2();
        for(i=0;i<30;i++){
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].col      = 0;
                boss_shot.bullet[k].x     = boss.x;
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 8;
                boss_shot.bullet[k].angle = angle+PI2/30*i;
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt      = 0;
                boss_shot.bullet[k].spd      = 3;
                se_flag[0]=1;
            }
        }
    }
    for(i=0;i<BOSS_BULLET_MAX;i++){
        if(boss_shot.bullet[i].flag>0){

        }
    }
}

//サイレントセレナ
void boss_shot_bulletH001(){
#define TM001 60
    int i,k,t=boss_shot.cnt%TM001,t2=boss_shot.cnt;
    static int cnum;
    double angle;
    if(t2==0)
        cnum=0;
    if(t==0){
        boss_shot.base_angle[0]=bossatan2();
        if(cnum%4==3){
            move_boss_pos(40,30,FMX-40,120,60, 60);
        }
    }
    if(t==TM001/2-1)
        boss_shot.base_angle[0]+=PI2/20/2;
    if(t%(TM001/10)==0){
        angle=bossatan2();
        for(i=0;i<20;i++){
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].col      = 4;
                boss_shot.bullet[k].x     = boss.x;
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 8;
                boss_shot.bullet[k].angle = boss_shot.base_angle[0]+PI2/20*i;
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt      = 0;
                boss_shot.bullet[k].spd      = 2.7;
                se_flag[0]=1;
            }
        }
    }
    if(t%4==0){
        if((k=search_boss_shot())!=-1){
            boss_shot.bullet[k].col      = 0;
            boss_shot.bullet[k].x     = GetRand(FMX);
            boss_shot.bullet[k].y     = GetRand(200);
            boss_shot.bullet[k].knd   = 8;
            boss_shot.bullet[k].angle = PI/2;
            boss_shot.bullet[k].flag  = 1;
            boss_shot.bullet[k].cnt      = 0;
            boss_shot.bullet[k].spd      = 1+rang(0.5);
            se_flag[0]=1;
        }
    }
//    for(i=0;i<BOSS_BULLET_MAX;i++){
//        if(boss_shot.bullet[i].flag>0){
//
//        }
//    }
    if(t==TM001-1)
        cnum++;
}
//パーフェクトフリーズ
void boss_shot_bulletH002(){
#define TM002 650
    int i,k,t=boss_shot.cnt%TM002;
    double angle;
    if(t==0 || t==210){
        //40<x<FMX-40  50<y<150　の範囲で100離れた位置に80カウントで移動する
        move_boss_pos(40,50,FMX-40,150,100, 80);
    }
    if(t<180){
        for(i=0;i<2;i++){
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].col      = GetRand(6);
                boss_shot.bullet[k].x     = boss.x;
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 7;
                boss_shot.bullet[k].angle = rang(PI2/20)+PI2/10*t;
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt      = 0;
                boss_shot.bullet[k].spd      = 3.2+rang(2.1);
                boss_shot.bullet[k].state = 0;
                boss_shot.bullet[k].kaiten= 1;
            }
        }
        if(t%10==0)
            se_flag[0]=1;
    }
    if(210<t && t<270 && t%3==0){
        angle=bossatan2();
        for(i=0;i<8;i++){
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].col      = 0;
                boss_shot.bullet[k].x     = boss.x;
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 7;
                boss_shot.bullet[k].angle = angle-PI/2*0.8+PI*0.8/7*i+rang(PI/180);
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt      = 0;
                boss_shot.bullet[k].spd      = 3.0+rang(0.3);
                boss_shot.bullet[k].state = 2;
                boss_shot.bullet[k].kaiten= 1;
            }
        }
        if(t%10==0)
            se_flag[0]=1;
    }
    for(i=0;i<BOSS_BULLET_MAX;i++){
        if(boss_shot.bullet[i].flag>0){
            //tが190の時に全てストップさせ、白くし、カウントリセット
            if(boss_shot.bullet[i].state==0){
                if(t==190){
                    boss_shot.bullet[i].kaiten=0;
                    boss_shot.bullet[i].spd      =0;
                    boss_shot.bullet[i].col   =9;
                    boss_shot.bullet[i].cnt   =0;
                    boss_shot.bullet[i].state =1;
                }
            }
            //ランダムな方向に移動し始める
            if(boss_shot.bullet[i].state==1){
                if(boss_shot.bullet[i].cnt==200){
                    boss_shot.bullet[i].angle=rang(PI);
                    boss_shot.bullet[i].kaiten=1;
                }
                if(boss_shot.bullet[i].cnt>200)
                    boss_shot.bullet[i].spd+=0.01;
            }
        }
    }
}
//恋の迷路
void boss_shot_bulletH003(){
#define TM003 600
#define DF003 20 
    int i,j,k,t=boss_shot.cnt%TM003,t2=boss_shot.cnt;
    static int tcnt,cnt,cnum;
    double angle;
    if(t2==0){
        //40<x<FMX-40  50<y<150　の範囲で100離れた位置に80カウントで移動する
        input_phy_pos(FMX/2,FMY/2, 50);
        cnum=0;
    }
    if(t==0){
        boss_shot.base_angle[0]=bossatan2();
        cnt=0;
        tcnt=2;
    }
    if(t<540 && t%3){
        angle=bossatan2();
        if(tcnt-2==cnt || tcnt-1==cnt){
            if(tcnt-1==cnt){
                boss_shot.base_angle[1]=boss_shot.base_angle[0]+PI2/DF003*cnt*(cnum?-1:1)-PI2/(DF003*6)*3;
                tcnt+=DF003-2;
            }
        }
        else{
            for(i=0;i<6;i++){
                if((k=search_boss_shot())!=-1){
                    boss_shot.bullet[k].col      = cnum?1:4;
                    boss_shot.bullet[k].x     = boss.x;
                    boss_shot.bullet[k].y     = boss.y;
                    boss_shot.bullet[k].knd   = 8;
                    boss_shot.bullet[k].angle 
                        = boss_shot.base_angle[0]+PI2/DF003*cnt*(cnum?-1:1)+PI2/(DF003*6)*i*(cnum?-1:1);
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt      = 0;
                    boss_shot.bullet[k].spd      = 2;
                    se_flag[0]=1;
                }
            }
        }
        cnt++;
    }
    if(40<t&&t<540 && t%30==0){
        for(j=0;j<3;j++){
            angle=boss_shot.base_angle[1]-PI2/36*4;
            for(i=0;i<27;i++){
                if((k=search_boss_shot())!=-1){
                    boss_shot.bullet[k].col      = cnum?6:0;
                    boss_shot.bullet[k].x     = boss.x;
                    boss_shot.bullet[k].y     = boss.y;
                    boss_shot.bullet[k].knd   = 7;
                    boss_shot.bullet[k].angle = angle;
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt      = 0;
                    boss_shot.bullet[k].spd      = 4-1.6/3*j;
                    se_flag[0]=1;
                }
                angle-=PI2/36;
            }
        }
    }
//    for(i=0;i<BOSS_BULLET_MAX;i++){
//        if(boss_shot.bullet[i].flag>0){
//
//        }
//    }
    if(t==TM003-1)
        cnum++;
}

//ケロちゃん風雨にも負けず
void boss_shot_bulletH004(){
#define TM004 200
    int i,j,k,n,t=boss_shot.cnt%TM004,t2=boss_shot.cnt;
    static int tm;
    double angle;
    if(t==0)
        tm=190+rang(30);
    angle=PI*1.5+PI/6*sin(PI2/tm*t2);
    if(t2%4==0){
        for(n=0;n<8;n++){
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].flag=1;
                boss_shot.bullet[k].state=0;
                boss_shot.bullet[k].cnt=0;
                boss_shot.bullet[k].knd=4;
                boss_shot.bullet[k].col=0;
                boss_shot.bullet[k].angle=0;
                boss_shot.bullet[k].eff_detail=1;
                boss_shot.bullet[k].x=boss.x;
                boss_shot.bullet[k].y=boss.y;
                boss_shot.bullet[k].vx = cos(angle-PI/8*4+PI/8*n+PI/16)*3;
                boss_shot.bullet[k].vy = sin(angle-PI/8*4+PI/8*n+PI/16)*3;
            }
        }
        se_flag[0]=1;
    }
    if(t%1==0 && t2>80){
        int num=1;
        if(t%2)
            num=2;
        for(n=0;n<num;n++){
            if((k=search_boss_shot())!=-1){
                angle=PI*1.5-PI/2+PI/12*(t2%13)+rang(PI/15);
                boss_shot.bullet[k].flag=1;
                boss_shot.bullet[k].state=1;
                boss_shot.bullet[k].cnt=0;
                boss_shot.bullet[k].knd=8;
                boss_shot.bullet[k].eff_detail=0;
                boss_shot.bullet[k].col=4;
                boss_shot.bullet[k].angle=0;
                boss_shot.bullet[k].x=boss.x;
                boss_shot.bullet[k].y=boss.y;
                boss_shot.bullet[k].vx = cos(angle)*1.4*1.2;
                boss_shot.bullet[k].vy = sin(angle)*1.4;
            }
        }
        se_flag[7]=1;
    }
    for(i=0;i<BOSS_BULLET_MAX;i++){
        if(boss_shot.bullet[i].flag>0){
            if(boss_shot.bullet[i].state==0){
                if(boss_shot.bullet[i].cnt<150)
                    boss_shot.bullet[i].vy+=0.03;
                boss_shot.bullet[i].x+=boss_shot.bullet[i].vx;
                boss_shot.bullet[i].y+=boss_shot.bullet[i].vy;
            }
            if(boss_shot.bullet[i].state==1){
                if(boss_shot.bullet[i].cnt<160)
                    boss_shot.bullet[i].vy+=0.03;
                boss_shot.bullet[i].x+=boss_shot.bullet[i].vx;
                boss_shot.bullet[i].y+=boss_shot.bullet[i].vy;
                boss_shot.bullet[i].angle=atan2(boss_shot.bullet[i].vy,boss_shot.bullet[i].vx);
            }
        }
    }
}

//☆弾
void boss_shot_bulletH005(){
}