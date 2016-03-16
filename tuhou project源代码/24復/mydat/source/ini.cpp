#include "../include/GV.h"

void input_bullet_info(bullet_info_t *binfo,int size_x,int size_y,int col_num,double range){
	binfo->size_x =size_x;	binfo->size_y =size_y;
	binfo->col_num=col_num;	binfo->range  =range;
}

//àÍî‘ç≈èâÇÃèâä˙âª
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

	input_bullet_info(&bullet_info[0],76, 76, 5,17.0);
	input_bullet_info(&bullet_info[1],22, 22, 6, 4.0);
	input_bullet_info(&bullet_info[2], 5,120,10, 2.5);
	input_bullet_info(&bullet_info[3],19, 34, 5, 2.0);
	input_bullet_info(&bullet_info[4],38, 38,10, 2.0);
	input_bullet_info(&bullet_info[5],14, 16, 3, 3.5);
	input_bullet_info(&bullet_info[6],14, 18, 3, 2.0);
	input_bullet_info(&bullet_info[7],16, 16, 9, 2.5);
	input_bullet_info(&bullet_info[8],12, 18,10, 1.5);
	input_bullet_info(&bullet_info[9],13, 19, 3, 2.0);
}

//ÉQÅ[ÉÄÇÃèâä˙âª
void ini(){
	stage_count=1;
	memset(&ch,0,sizeof(ch_t));
	memset(enemy,0,sizeof(enemy_t)*ENEMY_MAX);
	memset(enemy_order,0,sizeof(enemy_order_t)*ENEMY_ORDER_MAX);
	memset(shot,0,sizeof(shot_t)*SHOT_MAX);
	memset(cshot,0,sizeof(cshot_t)*CSHOT_MAX);
	memset(effect,0,sizeof(effect_t)*EFFECT_MAX);
	memset(del_effect,0,sizeof(del_effect_t)*DEL_EFFECT_MAX);
	memset(&bom,0,sizeof(bom_t));
	memset(&bright_set,0,sizeof(bright_set_t));
	memset(&dn,0,sizeof(dn_t));
	memset(&boss,0,sizeof(boss_t));

	ch.x=FIELD_MAX_X/2;
	ch.y=FIELD_MAX_Y*3/4;
	ch.power=500;
	boss.appear_count[0]=100;
	for(int i=0;i<DANMAKU_MAX;i++)
		boss.set_hp[i]=5000;
	bright_set.brt=255;
}