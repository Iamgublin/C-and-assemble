#include "../include/GV.h"

extern void graph_back_main();

void graph_effect(int knd){
	for(int i=0;i<EFFECT_MAX;i++){
		if(effect[i].flag>0 && effect[i].knd==knd){
			if(effect[i].eff==1)//エフェクトが光エフェクトなら
				SetDrawBlendMode( DX_BLENDMODE_ADD, effect[i].brt) ;
			if(effect[i].eff==2)//エフェクトがαエフェクトなら
				SetDrawBlendMode( DX_BLENDMODE_ALPHA, effect[i].brt) ;
			DrawRotaGraphF(effect[i].x+FX+dn.x,effect[i].y+FY+dn.y,effect[i].r,effect[i].ang,effect[i].img,TRUE);
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
			DrawRotaGraphF(enemy[i].x+FX+dn.x,enemy[i].y+FY+dn.y,1.0f,0.0f,img_enemy[0][enemy[i].img],TRUE);
		}
	}
}

void graph_boss(){
	int i;
	if(boss.flag==0)return;
	DrawRotaGraphF(boss.x+FX+dn.x,boss.y+FY+dn.y,1.0f,0.0f,img_dot_riria[0],TRUE);
	if(boss.hp_max==0){printfDx("graph_boss内0割り\n");return;}
	for(i=0;i<FMX*0.98*boss.hp/boss.hp_max;i++){
		if(boss.back_knd[boss.knd]==1)
			DrawGraph(3+FX+i+dn.x,2+FY+dn.y,img_etc[7],FALSE);
		else
			DrawGraph(3+FX+i+dn.x,2+FY+dn.y,img_etc[1],FALSE);
	}
}

//自機描画
void graph_ch(){
	double sx,sy,ny=(sin(2.0*PI*(count%50)/50)*3),ang=2.0*PI*(count%120)/120;

	if(CheckStatePad(configpad.slow)>0)//低速移動中なら
		sx=15,sy=15+ny;//引き寄せる
	else
		sx=30,sy=30+ny;//普通の位置に

	DrawRotaGraphF( ch.x-sx+FX, ch.y+sy+FY, 1.0f,  ang, img_chetc[2], TRUE );
	DrawRotaGraphF( ch.x+sx+FX, ch.y+sy+FY, 1.0f, -ang, img_chetc[2], TRUE );

	if(ch.mutekicnt%2==0){//無敵中なら点滅
		//自機表示
		DrawRotaGraphF(ch.x+FX+dn.x,ch.y+FY+dn.y,1.0f,0.0f,img_ch[0][ch.img],TRUE);
		if(CheckStatePad(configpad.slow)>0)//低速移動中なら当たり判定表示
			DrawRotaGraphF( ch.x+FX, ch.y+FY, 1.0f, 2.0*PI*(count%120)/120, img_chetc[0], TRUE );
	}
}

//自機ショット描画
void graph_cshot(){
	for(int i=0;i<CSHOT_MAX;i++){
		if(cshot[i].flag>0){
			DrawRotaGraphF(cshot[i].x+FX+dn.x,cshot[i].y+FY+dn.y,1,0,img_cshot[cshot[i].knd],TRUE);
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
						shot[i].bullet[j].x+FX+dn.x, shot[i].bullet[j].y+FY+dn.y,
						1.0, shot[i].bullet[j].angle+PI/2,
						img_bullet[shot[i].bullet[j].knd][shot[i].bullet[j].col],TRUE);

					if(shot[i].bullet[j].eff==1)
						SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0) ;
				}
			}
		}
	}
	//ボス
	if(boss_shot.flag>0){//弾幕データがオンなら
		for(j=0;j<BOSS_BULLET_MAX;j++){//その弾幕が持つ弾の最大数分ループ
			if(boss_shot.bullet[j].flag!=0){//弾データがオンなら
				if(boss_shot.bullet[j].eff==1)
					SetDrawBlendMode( DX_BLENDMODE_ADD, 255) ;

				DrawRotaGraphF(
					boss_shot.bullet[j].x+FX+dn.x, boss_shot.bullet[j].y+FY+dn.y,
					1.0, boss_shot.bullet[j].angle+PI/2,
					img_bullet[boss_shot.bullet[j].knd][boss_shot.bullet[j].col],TRUE);

				if(boss_shot.bullet[j].eff==1)
					SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0) ;
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

void graph_develop(){
	DrawFormatString(0,0,GetColor(255,255,255),"%d",stage_count);
}

void graph_main(){
	if(bright_set.brt!=255)SetDrawBright(bright_set.brt,bright_set.brt,bright_set.brt);

	graph_back_main();//背景描画メイン
	graph_effect(0);//敵が死ぬエフェクト

	if(bright_set.brt!=255)SetDrawBright(255,255,255);

	graph_effect(4);//喰らいボムのエフェクト

	if(bright_set.brt!=255)SetDrawBright(bright_set.brt,bright_set.brt,bright_set.brt);

	graph_boss();
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

	graph_develop();
}