#include "../include/GV.h"

//エフェクトの登録空き番号を探す
int search_effect(){
	for(int i=0;i<EFFECT_MAX;i++){
		if(effect[i].flag==0)
			return i;
	}
	return -1;
}

//エフェクトを計算する
void calc_effect(){
	for(int i=0;i<EFFECT_MAX;i++){
		if(effect[i].flag>0){//エフェクトが登録されていたら
			switch(effect[i].knd){//エフェクトの種類によって分岐
				case 0://0番の処理
					effect[i].cnt++;
					effect[i].r+=0.08;//エフェクトの大きさを段々大きくする
					if(effect[i].cnt>10)//10カウント以上になったら
						effect[i].brt-=25;//明るさを少なくする
					if(effect[i].cnt>20)//20カウント以上になったら
						effect[i].flag=0;//消す
					break;
				default:
					printfDx("effect設定エラー\n");
					break;
			}
		}
	}
}

//消滅エフェクトの登録空き番号を探す
int search_del_effect(){
	for(int i=0;i<DEL_EFFECT_MAX;i++){
		if(del_effect[i].flag==0)
			return i;
	}
	return -1;
}

//消滅エフェクトを登録する
void enter_del_effect(int s){
	int k=search_del_effect();
	if(k!=-1){
		del_effect[k].flag=1;//フラグを立てる
		del_effect[k].cnt=0;
		del_effect[k].col=enemy[s].back_col;//敵の背景色を消滅色に設定する
		del_effect[k].x=enemy[s].x;//敵の座標を消滅位置にセット
		del_effect[k].y=enemy[s].y;
	}
}

//消滅エフェクトを計算・エフェクトを登録する
void calc_del_effect(){
	int k;
	for(int i=0;i<DEL_EFFECT_MAX;i++){
		if(del_effect[i].flag>0){
			if(del_effect[i].cnt%2==0){// 1/30秒ずつセット
				if((k=search_effect())!=-1){//空き番号検索
					memset(&effect[k],0,sizeof(effect_t));//初期化
					effect[k].flag=1;//フラグを立てる
					effect[k].brt=255;//輝度を最大値に
					effect[k].ang=rang(PI);//角度をランダムに
					effect[k].col=del_effect[i].col;//色をセット
					effect[k].eff=1;//1は光エフェクト。光エフェクトにセットする
					effect[k].img=img_del_effect[effect[k].col];//消滅する画像をセット
					effect[k].knd=0;//エフェクトの種類は0番(消滅エフェクトは0とする)
					effect[k].x=del_effect[i].x;//座標をセット
					effect[k].y=del_effect[i].y;
				}
			}
			if(del_effect[i].cnt>8){
				del_effect[i].flag=0;
			}
			del_effect[i].cnt++;
		}
	}
}

void effect_main(){
	calc_del_effect();//消滅エフェクトの計算
	calc_effect();//エフェクトの計算
}