#include "../include/GV.h"

extern void shot_bullet_H000(int);extern void shot_bullet_H001(int);extern void shot_bullet_H002(int);
extern void shot_bullet_H003(int);extern void shot_bullet_H004(int);extern void shot_bullet_H005(int);
extern void shot_bullet_H006(int);extern void shot_bullet_H007(int);extern void shot_bullet_H008(int);
extern void shot_bullet_H009(int);extern void shot_bullet_H010(int);extern void shot_bullet_H011(int);
extern void shot_bullet_H012(int);extern void shot_bullet_H013(int);extern void shot_bullet_H014(int);
extern void shot_bullet_H015(int);extern void shot_bullet_H016(int);extern void shot_bullet_H017(int);

void (*shot_bullet[SHOT_KND_MAX])(int) ={
	shot_bullet_H000,shot_bullet_H001,shot_bullet_H002,shot_bullet_H003,shot_bullet_H004,
	shot_bullet_H005,shot_bullet_H006,shot_bullet_H007,shot_bullet_H008,shot_bullet_H009,
	shot_bullet_H010,shot_bullet_H011,shot_bullet_H012,shot_bullet_H013,shot_bullet_H014,
	shot_bullet_H015,shot_bullet_H016,shot_bullet_H017,
};

//n番目のショットを登録した敵と自機との角度を返す
double shotatan2(int n){
	return atan2(ch.y-enemy[shot[n].num].y,ch.x-enemy[shot[n].num].x);
}

//空いている弾を探す
int shot_search(int n){
	int i;
	for(i=0;i<SHOT_BULLET_MAX;i++){
		if(shot[n].bullet[i].flag==0){
			return i;
		}
	}
	return -1;
}

void shot_calc(int n){
	int i,max=0;
	if(enemy[shot[n].num].flag!=1)//敵が倒されたら
		shot[n].flag=2;//それ以上ショットを登録しないフラグに変える
	for(i=0;i<SHOT_BULLET_MAX;i++){//n番目の弾幕データの弾を計算
		if(shot[n].bullet[i].flag>0){//その弾が登録されていたら
			shot[n].bullet[i].x+=cos(shot[n].bullet[i].angle)*shot[n].bullet[i].spd;
			shot[n].bullet[i].y+=sin(shot[n].bullet[i].angle)*shot[n].bullet[i].spd;
			shot[n].bullet[i].cnt++;
			if(shot[n].bullet[i].x<-60 || shot[n].bullet[i].x>FIELD_MAX_X+60 ||
				shot[n].bullet[i].y<-60 || shot[n].bullet[i].y>FIELD_MAX_Y+60){//画面外に外れたら
				if(shot[n].bullet[i].till<shot[n].bullet[i].cnt)//最低消えない時間より長ければ
					shot[n].bullet[i].flag=0;//消す
			}
		}
	}
	//現在表示中の弾が一つでもあるかどうか調べる
	for(i=0;i<SHOT_BULLET_MAX;i++)
		if(shot[n].bullet[i].flag>0)
			return;
	//現在表示中の弾が一つもなければ
	if(enemy[shot[n].num].flag!=1){
		shot[n].flag=0;//終了
		enemy[shot[n].num].flag=0;
	}
}

void shot_main(){
	int i;
	for(i=0;i<SHOT_MAX;i++){//弾幕データ計算
		//フラグが立っていて、設定した種類が間違っていなければ(オーバーフロー対策)
		if(shot[i].flag!=0 && 0<=shot[i].knd && shot[i].knd<SHOT_KND_MAX){
			shot_bullet[shot[i].knd](i);//.kndの弾幕計算関数を呼ぶ関数ポインタ
			shot_calc(i);//i番目の弾幕を計算
			shot[i].cnt++;
		}
	}
}
