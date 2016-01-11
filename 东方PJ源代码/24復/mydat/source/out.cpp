#include "../include/GV.h"

#define ENEMY_RANGE_MAX 4
#define CSHOT_RANGE_MAX 2
#define CRANGE 2.0

//敵の当たり判定範囲
int enemy_range[ENEMY_RANGE_MAX]={16,30,16,50};
//自機ショットの当たり判定範囲
int cshot_range[CSHOT_RANGE_MAX]={6,};

//敵と自機ショットが当たったかどうかを判定する
int out_judge_cshot(int i,int s){
    int j;
    if(cshot[i].cnt>0){//ショットの軌道が１度でも計算されていたら
        double x=cshot[i].x-enemy[s].x;//敵と自機ショットとの距離
        double y=cshot[i].y-enemy[s].y;
        //オーバーフロー対策
        if(cshot[i].knd>=CSHOT_RANGE_MAX || enemy[s].knd>=ENEMY_RANGE_MAX)
            printfDx("out_judge_cshot内オーバーフロー");
        //敵の当たり判定と自機ショットの当たり判定の合計範囲
        double r=cshot_range[cshot[i].knd]+enemy_range[enemy[s].knd];
        //中間を計算する必要があれば
        if(cshot[i].spd>r){
            //１フレーム前にいた位置を格納する
            double pre_x=cshot[i].x+cos(cshot[i].angle+PI)*cshot[i].spd;
            double pre_y=cshot[i].y+sin(cshot[i].angle+PI)*cshot[i].spd;
            double px,py;
            for(j=0;j<cshot[i].spd/r;j++){//進んだ分÷当たり判定分ループ
                px=pre_x-enemy[s].x;
                py=pre_y-enemy[s].y;
                if(px*px+py*py<r*r)
                    return 1;
                pre_x+=cos(cshot[i].angle)*r;
                pre_y+=sin(cshot[i].angle)*r;
            }
        }
        if(x*x+y*y<r*r)//当たり判定内なら
            return 1;//当たり
    }
    return 0;
}

//自機と敵ショットが当たったかどうかを判定する
int out_judge_enemyshot(int s,int n){
    int j;
    if(shot[s].bullet[n].cnt>0){//ショットの軌道が１度でも計算されていたら
        double x=shot[s].bullet[n].x-ch.x;//敵と自機ショットとの距離
        double y=shot[s].bullet[n].y-ch.y;
        //オーバーフロー対策
        if(shot[s].bullet[n].knd>=10)
            printfDx("out_judge_enemyshot内オーバーフロー\n");
        //敵の当たり判定と自機ショットの当たり判定の合計範囲
        double r=bullet_info[shot[s].bullet[n].knd].range+CRANGE;
        //中間を計算する必要があれば
        if(shot[s].bullet[n].spd>r){
            //１フレーム前にいた位置を格納する
            double pre_x=shot[s].bullet[n].x+cos(shot[s].bullet[n].angle+PI)*shot[s].bullet[n].spd;
            double pre_y=shot[s].bullet[n].y+sin(shot[s].bullet[n].angle+PI)*shot[s].bullet[n].spd;
            double px,py;
            for(j=0;j<shot[s].bullet[n].spd/r;j++){//進んだ分÷当たり判定分ループ
                px=pre_x-ch.x;
                py=pre_y-ch.y;
                if(px*px+py*py<r*r)
                    return 1;
                pre_x+=cos(shot[s].bullet[n].angle)*r;
                pre_y+=sin(shot[s].bullet[n].angle)*r;
            }
        }
        if(x*x+y*y<r*r)//当たり判定内なら
            return 1;//当たり
    }
    return 0;
}

extern void enter_del_effect(int);

//敵が死ぬかどうかの決定
void enemy_death_judge(int s){
    int i;
    se_flag[8]=1;//敵に当たった音
    if(enemy[s].hp<0){//敵のHPが０未満になったら
        enemy[s].flag=0;//敵を消滅させる
        se_flag[1]=1;//敵のピチュリ音
        enter_del_effect(s);
        for(i=0;i<SHOT_MAX;i++){//敵総数分
            if(shot[i].flag!=0){//登録されている弾幕データがあれば
                if(s==shot[i].num){//その敵が登録した弾幕があれば
                    shot[i].flag=2;//それ以上弾幕を続けないフラグを立てる
                    break;
                }
            }
        }
    }
}

//sの敵にpower食らわす
void hit_enemy(int s,int power){
    enemy[s].hp-=power;//弾の持つパワー分HPを減らす
    enemy_death_judge(s);//敵が死ぬかどうかを決める
}

void cbom_and_enemy(){
	int s;
	if(bom.flag!=1)return;
    for(s=0;s<ENEMY_MAX;s++){//敵総数
        if(enemy[s].flag>0)
			hit_enemy(s,ch.power/20);
    }
}

//自機ショットと敵との処理
void cshot_and_enemy(){
    int i,s;
    for(i=0;i<CSHOT_MAX;i++){//自機ショット総数
        if(cshot[i].flag>0){
            for(s=0;s<ENEMY_MAX;s++){//敵総数
                if(enemy[s].flag>0){
                    if(out_judge_cshot(i,s)){//自機ショットと敵が当たっていれば
                        cshot[i].flag=0;//その自機ショットを消す
						hit_enemy(s,cshot[i].power);
                    }
                }
            }
        }
    }
}

//敵ショットと自機との処理
void enemyshot_and_ch(){
    int s,n;
    for(s=0;s<SHOT_MAX;s++){//敵ショット総数
        if(shot[s].flag>0){//そのショットが登録されていたら
            for(n=0;n<SHOT_BULLET_MAX;n++){//弾総数
                if(shot[s].bullet[n].flag==1){//弾が登録されていたら
                    if(out_judge_enemyshot(s,n)==1){//自機にその弾が接触していたら
                        shot[s].bullet[n].flag=0;//弾をオフ
						if(ch.flag==0 && ch.mutekicnt==0){
							ch.cnt=0;
							ch.flag=1;
							se_flag[3]=1;
						}
                    }
                }
            }
        }
    }
}

//当たり判定メイン
void out_main(){
	cbom_and_enemy();
    cshot_and_enemy();
    enemyshot_and_ch();
}