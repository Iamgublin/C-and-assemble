#include "../include/GV.h"

//1発だけ、自機に向かって直線発射
void shot_bullet_H000(int n){
	int k;
	if(shot[n].cnt==0){
		if(shot[n].flag!=2 && (k=shot_search(n))!=-1){
			shot[n].bullet[k].knd	=enemy[shot[n].num].blknd2;
			shot[n].bullet[k].angle	=shotatan2(n);
			shot[n].bullet[k].flag	=1;
			shot[n].bullet[k].x		=enemy[shot[n].num].x;
			shot[n].bullet[k].y		=enemy[shot[n].num].y;
			shot[n].bullet[k].col	=enemy[shot[n].num].col;
			shot[n].bullet[k].cnt	=0;
			shot[n].bullet[k].spd	=3;
			se_flag[0]=1;
		}
	}
}

//100カウント中に10発、自機に向かって直線発射(常に自機狙い)
void shot_bullet_H001(int n){
	int t=shot[n].cnt;
	int k;
	if(t>=0 && t<100 && t%10==0){
		if(shot[n].flag!=2 && (k=shot_search(n))!=-1){
			shot[n].bullet[k].knd	=enemy[shot[n].num].blknd2;
			shot[n].bullet[k].angle	=shotatan2(n);
			shot[n].bullet[k].flag	=1;
			shot[n].bullet[k].x		=enemy[shot[n].num].x;
			shot[n].bullet[k].y		=enemy[shot[n].num].y;
			shot[n].bullet[k].col	=enemy[shot[n].num].col;
			shot[n].bullet[k].cnt	=0;
			shot[n].bullet[k].spd	=3;
			se_flag[0]=1;
		}
	}
}

//100カウント中に10発、自機に向かって直線発射(角度記憶)
void shot_bullet_H002(int n){
	int t=shot[n].cnt;
	int k;
	if(t>=0 && t<100 && t%10==0){
		if(t==0)
			shot[n].base_angle[0]=shotatan2(n);
		if(shot[n].flag!=2 && (k=shot_search(n))!=-1){
			shot[n].bullet[k].knd	=enemy[shot[n].num].blknd2;
			shot[n].bullet[k].angle	=shot[n].base_angle[0];
			shot[n].bullet[k].flag	=1;
			shot[n].bullet[k].x		=enemy[shot[n].num].x;
			shot[n].bullet[k].y		=enemy[shot[n].num].y;
			shot[n].bullet[k].col	=enemy[shot[n].num].col;
			shot[n].bullet[k].cnt	=0;
			shot[n].bullet[k].spd	=3;
			se_flag[0]=1;
		}
	}
}

//100カウント中に10発、自機に向かってスピード変化直線発射
void shot_bullet_H003(int n){
	int t=shot[n].cnt;
	int k;
	if(t>=0 && t<100 && t%10==0){
		if(shot[n].flag!=2 && (k=shot_search(n))!=-1){
			shot[n].bullet[k].knd	=enemy[shot[n].num].blknd2;
			shot[n].bullet[k].angle	=shotatan2(n);
			shot[n].bullet[k].flag	=1;
			shot[n].bullet[k].x		=enemy[shot[n].num].x;
			shot[n].bullet[k].y		=enemy[shot[n].num].y;
			shot[n].bullet[k].col	=enemy[shot[n].num].col;
			shot[n].bullet[k].cnt	=0;
			shot[n].bullet[k].spd	=1+5.0/100*t;
			se_flag[0]=1;
		}
	}
}

//0.5秒に1回ずつ円形発射
void shot_bullet_H004(int n){
	int t=shot[n].cnt;
	int k;
	if(t>=0 && t<120 && t%20==0){
		double angle=shotatan2(n);
		for(int i=0;i<20;i++){
			if(shot[n].flag!=2 && (k=shot_search(n))!=-1){
				shot[n].bullet[k].knd	=enemy[shot[n].num].blknd2;
				shot[n].bullet[k].angle	=angle+PI2/20*i;
				shot[n].bullet[k].flag	=1;
				shot[n].bullet[k].x		=enemy[shot[n].num].x;
				shot[n].bullet[k].y		=enemy[shot[n].num].y;
				shot[n].bullet[k].col	=enemy[shot[n].num].col;
				shot[n].bullet[k].cnt	=0;
				shot[n].bullet[k].spd	=4;
			}
			se_flag[0]=1;
		}
	}
}

//ばらまきショット
void shot_bullet_H005(int n){
	int t=shot[n].cnt;
	int k;
	if(t>=0 && t<120 && t%2==0){
		if(shot[n].flag!=2 && (k=shot_search(n))!=-1){
			shot[n].bullet[k].knd	=enemy[shot[n].num].blknd2;
			shot[n].bullet[k].angle	=shotatan2(n)+rang(PI/4);
			shot[n].bullet[k].flag	=1;
			shot[n].bullet[k].x		=enemy[shot[n].num].x;
			shot[n].bullet[k].y		=enemy[shot[n].num].y;
			shot[n].bullet[k].col	=enemy[shot[n].num].col;
			shot[n].bullet[k].cnt	=0;
			shot[n].bullet[k].spd	=3+rang(1.5);
			se_flag[0]=1;
		}
	}
}

//ばらまきショット(減速)
void shot_bullet_H006(int n){
	int t=shot[n].cnt;
	int k;
	if(t>=0 && t<120 && t%2==0){
		if(shot[n].flag!=2 && (k=shot_search(n))!=-1){
			shot[n].bullet[k].knd	=enemy[shot[n].num].blknd2;
			shot[n].bullet[k].angle	=shotatan2(n)+rang(PI/4);
			shot[n].bullet[k].flag	=1;
			shot[n].bullet[k].x		=enemy[shot[n].num].x;
			shot[n].bullet[k].y		=enemy[shot[n].num].y;
			shot[n].bullet[k].col	=enemy[shot[n].num].col;
			shot[n].bullet[k].cnt	=0;
			shot[n].bullet[k].spd	=4+rang(2);
			se_flag[0]=1;
		}
	}
	for(int i=0;i<SHOT_BULLET_MAX;i++){
		if(shot[n].bullet[i].spd>1.5)
			shot[n].bullet[i].spd-=0.04;
	}
}
