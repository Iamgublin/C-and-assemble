#include "../include/GV.h"

extern int search_boss_shot();
extern double bossatan2();

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