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
            effect[i].x+=cos(effect[i].mvang)*effect[i].spd;
            effect[i].y+=sin(effect[i].mvang)*effect[i].spd;
            switch(effect[i].knd){//エフェクトの種類によって分岐
                case 0://敵の消滅エフェクト
                    effect[i].cnt++;
                    effect[i].r+=0.08;//エフェクトの大きさを段々大きくする
                    if(effect[i].cnt>10)//10カウント以上になったら
                        effect[i].brt-=25;//明るさを少なくする
                    if(effect[i].cnt>20)//20カウント以上になったら
                        effect[i].flag=0;//消す
                    break;
                case 1://ボムのエフェクト
                    //スピード計算
                    if(effect[i].cnt<60)
                        effect[i].spd-=(0.2+effect[i].cnt*effect[i].cnt/3000.0);
                    if(effect[i].cnt==60){
                        effect[i].spd=0;
                        se_flag[15]=1;
                        dn.flag=1;
                        dn.cnt=0;
                        dn.size=11;
                        dn.time=20;
                    }
                    //明るさと大きさ計算
                    effect[i].r+=0.015;
                    if(effect[i].cnt<51)
                        effect[i].brt+=5;
                    if(effect[i].cnt>=60){
                        effect[i].r+=0.04;
                        effect[i].brt-=255/30.0;
                    }
                    //カウントアップと消去計算
                    effect[i].cnt++;
                    if(effect[i].cnt>=90)
                        effect[i].flag=0;
                    break;
                case 2://ボムエフェクト（キャラ）
                    //明るさ計算
                    if(effect[i].cnt<51)
                        effect[i].brt+=4;
                    if(effect[i].cnt>130-51)
                        effect[i].brt-=4;
                    //カウントアップと消去計算
                    effect[i].cnt++;
                    if(effect[i].cnt>=130)
                        effect[i].flag=0;
                    break;
                case 3://ボムのエフェクト（ライン）
                    //明るさ計算
                    if(effect[i].cnt<51)
                        effect[i].brt+=2;
                    if(effect[i].cnt>130-51)
                        effect[i].brt-=2;
                    //カウントアップと消去計算
                    effect[i].cnt++;
                    if(effect[i].cnt>=130)
                        effect[i].flag=0;
                    break;                
                case 4://喰らいボムエフェクト
                    //明るさ計算
                    if(effect[i].cnt>=6)
                        effect[i].brt-=255/6;
                    effect[i].r+=0.12;
                    effect[i].cnt++;
                    if(effect[i].cnt>=12 || ch.flag!=1)
                        effect[i].flag=0;
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
                    effect[k].spd=0;
                }
            }
            if(del_effect[i].cnt>8){
                del_effect[i].flag=0;
            }
            del_effect[i].cnt++;
        }
    }
}

//ボムを登録
void enter_bom(){
    int k;
    bom.flag=1;
    bom.cnt=0;
    bom.knd=0;
    ch.flag=0;
    ch.mutekicnt=1;//無敵に
    se_flag[14]=1;//キュイーン音
    //縦線
    if((k=search_effect())!=-1){
        effect[k].flag=1;
        effect[k].cnt=0;
        effect[k].knd=3;//ボムの線のエフェクト
        effect[k].brt=0;
        effect[k].ang=PI/2;
        effect[k].mvang=-PI/2;
        effect[k].spd=1;
        effect[k].r=1;
        effect[k].eff=2;
        effect[k].img=img_eff_bom[3];
        effect[k].x=70;
        effect[k].y=300;
    }
    //横線
    if((k=search_effect())!=-1){
        effect[k].flag=1;
        effect[k].cnt=0;
        effect[k].knd=3;//ボムの線のエフェクト
        effect[k].brt=0;
        effect[k].ang=0;
        effect[k].mvang=0;
        effect[k].spd=1;
        effect[k].r=1;
        effect[k].eff=2;
        effect[k].img=img_eff_bom[3];
        effect[k].x=100;
        effect[k].y=350;
    }
    //キャラ
    if((k=search_effect())!=-1){
        effect[k].flag=1;
        effect[k].cnt=0;
        effect[k].knd=2;//ボムのキャラ・線のエフェクト
        effect[k].brt=0;
        effect[k].ang=0;
        effect[k].mvang=-PI/2;
        effect[k].spd=0.7;
        effect[k].r=1;
        effect[k].eff=2;
        effect[k].img=img_eff_bom[2];
        effect[k].x=260;
        effect[k].y=300;
    }
}
//ドガーンとゆれる画面の処理
void dn_calc(){
    if(dn.flag==1){
        dn.x=(int)rang(dn.size);
        dn.y=(int)rang(dn.size);
        dn.cnt++;
        if(dn.cnt>dn.time){//指定された時間がたつと終わる
            dn.flag=0;
            dn.x=0;
            dn.y=0;
        }
    }
}

//ボム計算
void bom_calc(){
    int n,k;
	double shot_angle[4]={0,PI,PI/2,PI*1.5};//4発エフェクトが飛ぶ角度
    if((ch.flag==0||ch.flag==1) && bom.flag==0){//キャラが通常か喰らいボム状態で、ボムがオフなら
        if(CheckStatePad(configpad.bom)==1){//ボムボタンが押されたら
            enter_bom();
        }
    }
    if(bom.flag==1){//ボムが登録されていたら
        if(bom.cnt%10==0){//1/6秒に１回
            n=(bom.cnt/10);
            if(n<4 && (k=search_effect())!=-1){
                effect[k].flag=1;
                effect[k].cnt=0;
                effect[k].knd=1;//エフェクト番号１：ボムのエフェクト
                effect[k].brt=0;
                effect[k].ang=rang(PI);//画像の向きはランダム
                effect[k].mvang=shot_angle[n]-PI/4;//飛ぶ方向
                effect[k].spd=13+rang(2);//スピード
                effect[k].r=0.5;//大きさ
                effect[k].eff=2;//αブレンド
                effect[k].img=img_eff_bom[(bom.cnt/10)/3];//画像
                effect[k].x=ch.x;//座標
                effect[k].y=ch.y;
            }
        }
        bom.cnt++;
        if(bom.cnt<40)
            bright_set.brt=255-bom.cnt*5;//画面の明るさ設定(暗く)
        if(bom.cnt>90)
            bright_set.brt=255-40*5+(bom.cnt-90)*5;//画面の明るさ設定(明るく)
        if(bom.cnt>130){
            bom.flag=0;
            bright_set.brt=255;
        }
    }
}


void enter_crybom_effect(){
    int k;
    if(ch.flag==1){
        if(ch.cnt%2==0){
            if((k=search_effect())!=-1){
                effect[k].flag=1;
                effect[k].cnt=0;
                effect[k].knd=4;//喰らいボムエフェクト
                effect[k].brt=255;
                effect[k].ang=rang(PI);
                effect[k].spd=0;
                effect[k].r=0;
                effect[k].eff=2;
                effect[k].img=img_del_effect[GetRand(4)];
                effect[k].x=ch.x;
                effect[k].y=ch.y;
            }
        }
    }
}

void effect_main(){
    dn_calc();//ドガーンとゆれる画面の処理
    calc_del_effect();//消滅エフェクトの計算
    calc_effect();//エフェクトの計算
    bom_calc();//ボム計算
    enter_crybom_effect();
}