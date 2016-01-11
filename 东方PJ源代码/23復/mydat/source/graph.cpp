#include "../include/GV.h"

extern void graph_back_main();

void graph_effect(int knd){
	for(int i=0;i<EFFECT_MAX;i++){
		if(effect[i].flag>0 && effect[i].knd==knd){
			if(effect[i].eff==1)//エフェクトが光エフェクトなら
				SetDrawBlendMode( DX_BLENDMODE_ADD, effect[i].brt) ;
			if(effect[i].eff==2)//エフェクトがαエフェクトなら
				SetDrawBlendMode( DX_BLENDMODE_ALPHA, effect[i].brt) ;
			DrawRotaGraphF(effect[i].x+FIELD_X+dn.x,effect[i].y+FIELD_Y+dn.y,effect[i].r,effect[i].ang,effect[i].img,TRUE);
			if(effect[i].eff==1 || effect[i].eff==2)
				SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0) ;
		}
	}
}

//敵描画
void graph_enemy(){
	int i;
	for(i=0;i<ENEMY_MAX;i++){
		if(enemy[i].flag==1){
			DrawRotaGraphF(enemy[i].x+FIELD_X+dn.x,enemy[i].y+FIELD_Y+dn.y,1.0f,0.0f,img_enemy[0][enemy[i].img],TRUE);
		}
	}
}

//自機描画
void graph_ch(){
	if(ch.mutekicnt%2==0)
		DrawRotaGraphF(ch.x+FIELD_X+dn.x,ch.y+FIELD_Y+dn.y,1.0f,0.0f,img_ch[0][ch.img],TRUE);
}

//自機ショット描画
void graph_cshot(){
	for(int i=0;i<CSHOT_MAX;i++){
		if(cshot[i].flag>0){
			DrawRotaGraphF(cshot[i].x+FIELD_X+dn.x,cshot[i].y+FIELD_Y+dn.y,1,0,img_cshot[cshot[i].knd],TRUE);
		}
	}
}

//弾丸の描画
void graph_bullet(){
	int i,j;
	SetDrawMode( DX_DRAWMODE_BILINEAR ) ;//線形補完描画
	for(i=0;i<SHOT_MAX;i++){//敵の弾幕数分ループ
		if(shot[i].flag>0){//弾幕データがオンなら
			for(j=0;j<SHOT_BULLET_MAX;j++){//その弾幕が持つ弾の最大数分ループ
				if(shot[i].bullet[j].flag!=0){//弾データがオンなら
					if(shot[i].bullet[j].eff==1)
						SetDrawBlendMode( DX_BLENDMODE_ADD, 255) ;

					DrawRotaGraphF(
						shot[i].bullet[j].x+FIELD_X+dn.x, shot[i].bullet[j].y+FIELD_Y+dn.y,
						1.0, shot[i].bullet[j].angle+PI/2,
						img_bullet[shot[i].bullet[j].knd][shot[i].bullet[j].col],TRUE);

					if(shot[i].bullet[j].eff==1)
						SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0) ;
				}
			}
		}
	}
	SetDrawMode(DX_DRAWMODE_NEAREST);//描画形式を戻す
}

//ボード描画
void graph_board(){
	DrawGraph(	0,	0,img_board[10],FALSE);
	DrawGraph(	0, 16,img_board[11],FALSE);
	DrawGraph(	0,464,img_board[12],FALSE);
	DrawGraph(416,	0,img_board[20],FALSE);
}

void graph_main(){
	if(bright_set.brt!=255)SetDrawBright(bright_set.brt,bright_set.brt,bright_set.brt);

	graph_back_main();//背景描画メイン
	graph_effect(0);//敵が死ぬエフェクト

	if(bright_set.brt!=255)SetDrawBright(255,255,255);

	graph_effect(4);//喰らいボムのエフェクト

	if(bright_set.brt!=255)SetDrawBright(bright_set.brt,bright_set.brt,bright_set.brt);

	graph_enemy();//敵の描画
	graph_cshot();//自機ショットの描画

	if(bright_set.brt!=255)SetDrawBright(255,255,255);

	graph_ch();//自機の描画

	if(bright_set.brt!=255)SetDrawBright(bright_set.brt,bright_set.brt,bright_set.brt);
	
	graph_bullet();//弾の描画

	if(bright_set.brt!=255)SetDrawBright(255,255,255);

	graph_effect(1);//ボムのエフェクト
	graph_effect(2);//ボム線のエフェクト
	graph_effect(3);//ボムキャラのエフェクト
	graph_board();//ボードの描画
}