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
				boss_shot.bullet[k].col	  = 0;
				boss_shot.bullet[k].x     = boss.x;
				boss_shot.bullet[k].y     = boss.y;
				boss_shot.bullet[k].knd   = 8;
				boss_shot.bullet[k].angle = angle+PI2/30*i;
				boss_shot.bullet[k].flag  = 1;
				boss_shot.bullet[k].cnt	  = 0;
				boss_shot.bullet[k].spd	  = 3;
				se_flag[0]=1;
			}
		}
	}
	for(i=0;i<BOSS_BULLET_MAX;i++){
		if(boss_shot.bullet[i].flag>0){

		}
	}
}


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
				boss_shot.bullet[k].col	  = 4;
				boss_shot.bullet[k].x     = boss.x;
				boss_shot.bullet[k].y     = boss.y;
				boss_shot.bullet[k].knd   = 8;
				boss_shot.bullet[k].angle = boss_shot.base_angle[0]+PI2/20*i;
				boss_shot.bullet[k].flag  = 1;
				boss_shot.bullet[k].cnt	  = 0;
				boss_shot.bullet[k].spd	  = 2.7;
				se_flag[0]=1;
			}
		}
	}
	if(t%4==0){
		if((k=search_boss_shot())!=-1){
			boss_shot.bullet[k].col	  = 0;
			boss_shot.bullet[k].x     = GetRand(FMX);
			boss_shot.bullet[k].y     = GetRand(200);
			boss_shot.bullet[k].knd   = 8;
			boss_shot.bullet[k].angle = PI/2;
			boss_shot.bullet[k].flag  = 1;
			boss_shot.bullet[k].cnt	  = 0;
			boss_shot.bullet[k].spd	  = 1+rang(0.5);
			se_flag[0]=1;
		}
	}
//	for(i=0;i<BOSS_BULLET_MAX;i++){
//		if(boss_shot.bullet[i].flag>0){
//
//		}
//	}
	if(t==TM001-1)
		cnum++;
}
