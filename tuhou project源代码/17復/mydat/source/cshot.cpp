#include "../include/GV.h"

int cshot0num[2]  ={2,4};
int cshot0pos_x[4]={-10, 10,-30, 30};
int cshot0pos_y[4]={-30,-30,-10,-10};

//自機ショットの登録可能番号を返す
int search_cshot(){
	for(int i=0;i<CSHOT_MAX;i++){
		if(cshot[i].flag==0)
			return i;
	}
	return -1;
}

//通常ショット登録
void ch0_shot_pattern(){
	int k;
	for(int i=0;i<cshot0num[ch.power<200?0:1];i++){
		if((k=search_cshot())!=-1){
			cshot[k].flag=1;
			cshot[k].cnt=0;
			cshot[k].angle=-PI/2;
			cshot[k].spd=20;
			cshot[k].x=ch.x+cshot0pos_x[i];
			cshot[k].y=ch.y+cshot0pos_y[i];
			cshot[k].power=23;
			cshot[k].knd=0;
		}
	}
	se_flag[2]=1;//発射音オン
}

//低速通常ショット登録
void ch1_shot_pattern(){
	int k;
	for(int i=0;i<cshot0num[ch.power<200?0:1];i++){
		if((k=search_cshot())!=-1){
			cshot[k].flag=1;
			cshot[k].cnt=0;
			cshot[k].angle=-PI/2;
			cshot[k].spd=20;
			cshot[k].x=ch.x+cshot0pos_x[i]/3;//低速中なら位置を中心側へ
			cshot[k].y=ch.y+cshot0pos_y[i]/2;
			cshot[k].power=23;
			cshot[k].knd=0;
		}
	}
	se_flag[2]=1;
}

//ショット登録部
void enter_shot(){
	//ショットボタンが押されていたら
	if(CheckStatePad(configpad.shot)>0){
		ch.shot_cnt++;
		if(ch.shot_cnt%3==0){//3カウントに1回
			if(CheckStatePad(configpad.slow)>0)//低速移動中なら
				ch1_shot_pattern();
			else
				ch0_shot_pattern();
		}
	}
	else
		ch.shot_cnt=0;
}

//ショットの移動計算
#define CSOUT 20 //どれ位画面から外れたら消すか
void calc_cshot(){
	for(int i=0;i<CSHOT_MAX;i++){
		if(cshot[i].flag==1){
			cshot[i].x+=cos(cshot[i].angle)*cshot[i].spd;
			cshot[i].y+=sin(cshot[i].angle)*cshot[i].spd;
			if(cshot[i].x<-CSOUT || cshot[i].x>FIELD_MAX_X+CSOUT ||
				cshot[i].y<-CSOUT || cshot[i].y>FIELD_MAX_Y+CSOUT)//画面から外れたら
				cshot[i].flag=0;
		}
	}
}

//キャラクタショットに関する関数
void cshot_main(){
	calc_cshot();//ショットの起動計算
	enter_shot();//ショット登録
}
