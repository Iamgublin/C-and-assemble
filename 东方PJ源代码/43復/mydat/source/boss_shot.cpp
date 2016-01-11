#include "../include/GV.h"
#include "../include/func.h"

#define V0 10.0
 
//物理的計算を点と距離指定で登録をする(指定時間tで定位置に戻す)
void input_phy_pos(double x,double y,int t){//t=移動にかける時間
	double ymax_x,ymax_y;
	if(t==0)t=1;
	boss.phy.flag=1;//登録オン
	boss.phy.cnt=0;//カウンタ初期化
	boss.phy.set_t=t;//移動にかける時間をセット
	ymax_x=boss.x-x;//移動したい水平距離
	boss.phy.v0x=2*ymax_x/t;//水平成分の初速度
	boss.phy.ax =2*ymax_x/(t*t);//水平成分の加速度
	boss.phy.prex=boss.x;//初期x座標
	ymax_y=boss.y-y;//移動したい鉛直距離
	boss.phy.v0y=2*ymax_y/t;//鉛直成分の初速度
	boss.phy.ay =2*ymax_y/(t*t);//鉛直成分の加速度
	boss.phy.prey=boss.y;//初期y座標
}

//レーザーの物理的計算登録
void input_lphy(lazer_t *laz, int time, double angle){
	laz->lphy.angle   =angle;
	laz->lphy.base_ang=laz->angle;
	laz->lphy.time    =time;
}

//今いる位置からdist離れた位置にtカウントで移動する
int move_boss_pos(double x1,double y1,double x2,double y2,double dist, int t){
	int i=0;
	double x,y,angle;
	for(i=0;i<1000;i++){
		x=boss.x,y=boss.y;//今のボスの位置をセット
		angle=rang(PI);//適当に向かう方向を決める
		x+=cos(angle)*dist;//そちらに移動させる
		y+=sin(angle)*dist;
		if(x1<=x&&x<=x2 && y1<=y&&y<=y2){//その点が移動可能範囲なら
			input_phy_pos(x,y,t);
			return 0;
		}
	}
	return -1;//1000回試してダメならエラー
}

int search_child(){//空き番号を返す
	for(int i=0;i<CHILD_MAX;i++){
		if(child[i].flag==0)
			return i;
	}
	return -1;
}

int search_boss_shot(){//空き番号を返す
	for(int i=0;i<BOSS_BULLET_MAX;i++){
		if(boss_shot.bullet[i].flag==0)
			return i;
	}
	return -1;
}
int search_lazer(){//空き番号を返す
	for(int i=0;i<LAZER_MAX;i++){
		if(lazer[i].flag==0)
			return i;
	}
	return -1;
}
double bossatan2(){//自機と敵との成す角度
	return atan2(ch.y-boss.y,ch.x-boss.x);
}
double bossatan3(int k,double x,double y){//指定座標と弾との成す角度
	return atan2(y-boss_shot.bullet[k].y,x-boss_shot.bullet[k].x);
}
//物理的計算をさせる登録をする(指定時間tで定位置に戻す)
void input_phy(int t){//t=移動にかける時間
	double ymax_x,ymax_y;
	if(t==0)t=1;
	boss.phy.flag=1;//登録オン
	boss.phy.cnt=0;//カウンタ初期化
	boss.phy.set_t=t;//移動にかける時間をセット
	ymax_x=boss.x-BOSS_POS_X;//移動したい水平距離
	boss.phy.v0x=2*ymax_x/t;//水平成分の初速度
	boss.phy.ax =2*ymax_x/(t*t);//水平成分の加速度
	boss.phy.prex=boss.x;//初期x座標
	ymax_y=boss.y-BOSS_POS_Y;//移動したい鉛直距離
	boss.phy.v0y=2*ymax_y/t;//鉛直成分の初速度
	boss.phy.ay =2*ymax_y/(t*t);//鉛直成分の加速度
	boss.phy.prey=boss.y;//初期y座標
}
//物理的キャラクタ移動計算
void calc_phy(){
	double t=boss.phy.cnt;
	boss.x=boss.phy.prex-((boss.phy.v0x*t)-0.5*boss.phy.ax*t*t);//現在いるべきx座標計算
	boss.y=boss.phy.prey-((boss.phy.v0y*t)-0.5*boss.phy.ay*t*t);//現在いるべきy座標計算
	boss.phy.cnt++;
	if(boss.phy.cnt>=boss.phy.set_t)//移動にかける時間分になったら
		boss.phy.flag=0;//オフ
}
//ボスの弾幕を計算する
void boss_shot_calc(){
	int i;
	boss.endtime--;
	if(boss.endtime<0)
		boss.hp=0;
	for(i=0;i<BOSS_BULLET_MAX;i++){
		if(boss_shot.bullet[i].flag>0){
			boss_shot.bullet[i].x+=cos(boss_shot.bullet[i].angle)*boss_shot.bullet[i].spd;
			boss_shot.bullet[i].y+=sin(boss_shot.bullet[i].angle)*boss_shot.bullet[i].spd;
			boss_shot.bullet[i].cnt++;
			if(boss_shot.bullet[i].cnt>boss_shot.bullet[i].till){
				if(boss_shot.bullet[i].x<-50 || boss_shot.bullet[i].x>FMX+50 ||
					boss_shot.bullet[i].y<-50 || boss_shot.bullet[i].y>FMY+50)
					boss_shot.bullet[i].flag=0;
			}
		}
	}
	boss_shot.cnt++;
}
//弾幕をセット
void enter_boss_shot(){
	memset(&boss_shot , 0, sizeof(boss_shot_t));//弾情報初期化
	boss_shot.flag=1;
	boss.wtime=0;//待機時間0
	boss.state=2;//弾幕中状態に
	boss.hp=boss.set_hp[boss.knd];//HP設定
	boss.hp_max=boss.hp;
}
//ボスをセット
void enter_boss(int num){
	if(num==0){//中ボス開始時の時は
		memset(enemy,0,sizeof(enemy_t)*ENEMY_MAX);//雑魚敵を消す
		memset(shot,0,sizeof(shot_t)*SHOT_MAX);//弾幕を消す
		boss.x=FMX/2;//ボスの初期座標
		boss.y=-30;
		boss.knd=-1;//弾幕の種類
	}
	boss.flag=1;
	boss.hagoromo=0;//扇を広げるかどうかのフラグ
	boss.endtime=99*60;//残り時間
	boss.state=1;//待機中状態に
	boss.cnt=0;
	boss.graph_flag=0;//描画フラグを戻す
	boss.knd++;
	boss.wtime=0;//待機時間を初期化
	memset(&boss_shot,0,sizeof(boss_shot_t));//ボスの弾幕情報を初期化
	memset(lazer,0,sizeof(lazer_t)*LAZER_MAX);//レーザー情報を初期化
	input_phy(60);//60カウントかけて物理的計算で定位置に戻す
}
//ボスの待機処理
void waitandenter(){
	int t=150;
	boss.wtime++;
	if(boss.wtime>t)//140カウント待機したら弾幕セット
		enter_boss_shot();
}

void calc_boss(){
	boss.dx=boss.x;
	boss.dy=boss.y+sin(PI2/130*(count%130))*10;
}

//ボスの弾幕メイン
void boss_shot_main(){
	if(stage_count==boss.appear_count[0] && boss.flag==0){//開始時間なら
		enter_boss(0);//開始
		//反魂蝶のBGMを流す場合はload.cppを変更後、ここの注釈を外してください。
//		PlaySoundMem(sound_se[99],DX_PLAYTYPE_BACK);
	}
	if(boss.flag==0)//ボスが登録されて無ければ戻る
		return;
	calc_boss();
	if(boss.phy.flag==1)//物理演算移動オンなら
		calc_phy();//物理計算を
	if(boss.state==2 && (boss.hp<=0 || boss.endtime<=0)){//弾幕中で体力が無くなったら
        se_flag[1]=1;//敵のピチュリ音
		se_flag[11]=1;
		enter_boss(1);//次の弾幕を登録
	}
	if(boss.state==1){//弾幕間の待機時間
		waitandenter();
	}
	if(boss.state==2){//弾幕中なら
		boss_shot_bullet[boss.knd]();//弾幕関数へ
		boss_shot_calc();//弾幕計算
	}
	boss.cnt++;
}


