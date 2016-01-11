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

//アイテムの吸収処理。
void calc_item_indraw(int i){
	double v = item[i].state ? 8 : 3;//state1ならスピード8、違うなら2
	double angle=atan2(ch.y-item[i].y,ch.x-item[i].x);//自機への角度を計算して
	item[i].x += cos(angle)*v;
	item[i].y += sin(angle)*v;
}

//アイテム移動など計算
void calc_item(){
	for(int i=0;i<ITEM_MAX;i++){
		if(item[i].flag>0){
/*			if(item[i].state==0)
				if(ch.y<ITEM_GET_BORDER_LINE)//吸収状態なら
					item[i].state=1;*///(b1)
			if(item[i].state==0){//普通の状態なら
				double x=ch.x-item[i].x,y=ch.y-item[i].y;
				//低速状態で自機付近なら
				if(CheckStatePad(configpad.slow)>0 &&
						x*x+y*y<ITEM_INDRAW_RANGE*ITEM_INDRAW_RANGE){
					calc_item_indraw(i);//吸収
				}
				else{
					if(item[i].v<2.5)//速度アップ
						item[i].v+=0.06;
					item[i].y+=item[i].v;//移動
				}
			}
			else{//吸収状態なら
				calc_item_indraw(i);
			}
			item[i].cnt++;
			if(item[i].y>FMY+50)
				item[i].flag=0;
		}
	}
}

//(b1)
void calc_kago(){
	kago.x+=kago.muki*2;
	if(kago.x<FX || kago.x>FMX)
		kago.muki*=-1;
}

void calc_main(){
	calc_stage_title();
	calc_item();
	calc_kago();
}