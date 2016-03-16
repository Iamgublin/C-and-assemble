#include "../include/GV.h"

void calc_stage_title(){
	if(stage_title.appear_cnt==stage_count){//今が表示開始時なら
		stage_title.flag=1;
		stage_title.img = LoadGraph("../dat/img/board/subject0.png");
	}

	if(stage_title.flag==0)return;

	if(stage_title.cnt<128)//最初の128の間段々明るく
		stage_title.brt+=2;
	if(stage_title.cnt>128+128)//段々暗く
		stage_title.brt-=2;
	if(stage_title.cnt>128+128+128){//終り
		stage_title.cnt=0;
		stage_title.brt=0;
		stage_title.flag=0;
		DeleteGraph(stage_title.img);//画像解放
		return ;
	}

	stage_title.cnt++;
	//強制終了した時の画像解放処理が必要
}

void calc_main(){
	calc_stage_title();
}