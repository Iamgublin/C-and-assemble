#include "../include/GV.h"

extern int search_boss_shot();
extern double bossatan2();
extern double bossatan3(int k,double x,double y);//指定座標と弾との成す角
extern int move_boss_pos(double x1,double y1,double x2,double y2,double dist, int t);
extern void input_phy_pos(double x,double y,int t);

//ただの円形発射
void boss_shot_bulletH000(){
#define TM000 120
    int i,k,t=boss_shot.cnt%TM000;
    double angle;
    //60カウント以下で10カウントに1回
    if(t<60 && t%10==0){
        angle=bossatan2();//自機とボスとの成す角
        for(i=0;i<30;i++){
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].col      = 0;//弾の色
                boss_shot.bullet[k].x     = boss.x;//座標
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 8;//弾の種類
                boss_shot.bullet[k].angle = angle+PI2/30*i;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt      = 0;
                boss_shot.bullet[k].spd      = 3;//スピード
                se_flag[0]=1;
            }
        }
    }
//    for(i=0;i<BOSS_BULLET_MAX;i++){
//        if(boss_shot.bullet[i].flag>0){
//
//        }
//    }
}

//サイレントセレナ
void boss_shot_bulletH001(){
#define TM001 60
    int i,k,t=boss_shot.cnt%TM001,t2=boss_shot.cnt;
    static int cnum;
    double angle;
    if(t2==0)//最初なら
        cnum=0;
    if(t==0){//1周期の最初なら
        boss_shot.base_angle[0]=bossatan2();//基準と成る角度をセット
        if(cnum%4==3){//4回中4回目なら
            //40＜ｘ＜FMX-40、30＜y＜120の範囲を60の距離、60カウントで移動
            move_boss_pos(40,30,FMX-40,120,60, 60);
        }
    }
    if(t==TM001/2-1)//周期の半分で角度を変える
        boss_shot.base_angle[0]+=PI2/20/2;
    //円形
    if(t%(TM001/10)==0){//10カウントに1回ずつ
        angle=bossatan2();//ボスと自機との成す角
        for(i=0;i<20;i++){//PI2/20度ずつ1周
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].col   = 4;
                boss_shot.bullet[k].x     = boss.x;
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 8;
                boss_shot.bullet[k].angle = boss_shot.base_angle[0]+PI2/20*i;
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 2.7;
                se_flag[0]=1;
            }
        }
    }
    //ランダム直下落下
    if(t%4==0){
        if((k=search_boss_shot())!=-1){
            boss_shot.bullet[k].col   = 0;
            boss_shot.bullet[k].x     = GetRand(FMX);
            boss_shot.bullet[k].y     = GetRand(200);
            boss_shot.bullet[k].knd   = 8;
            boss_shot.bullet[k].angle = PI/2;
            boss_shot.bullet[k].flag  = 1;
            boss_shot.bullet[k].cnt   = 0;
            boss_shot.bullet[k].spd   = 1+rang(0.5);
            se_flag[0]=1;
        }
    }
//    for(i=0;i<BOSS_BULLET_MAX;i++){
//        if(boss_shot.bullet[i].flag>0){
//
//        }
//    }
    if(t==TM001-1)
        cnum++;
}
//パーフェクトフリーズ
void boss_shot_bulletH002(){
#define TM002 650
    int i,k,t=boss_shot.cnt%TM002;
    double angle;
    if(t==0 || t==210){
        //40<x<FMX-40  50<y<150　の範囲で100離れた位置に80カウントで移動する
        move_boss_pos(40,50,FMX-40,150,100, 80);
    }
    //最初のランダム発射
    if(t<180){
        for(i=0;i<2;i++){
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].col   = GetRand(6);
                boss_shot.bullet[k].x     = boss.x;
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 7;//弾の種類
                boss_shot.bullet[k].angle = rang(PI2/20)+PI2/10*t;
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 3.2+rang(2.1);
                boss_shot.bullet[k].state = 0;//弾のステータス
                boss_shot.bullet[k].kaiten= 1;//弾を回転するかどうかのフラグ
            }
        }
        if(t%10==0)
            se_flag[0]=1;
    }
    if(210<t && t<270 && t%3==0){
        angle=bossatan2();
        for(i=0;i<8;i++){
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].col   = 0;
                boss_shot.bullet[k].x     = boss.x;
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 7;
                //自機とボスとの成す角を基準に８方向に発射する
                boss_shot.bullet[k].angle = angle-PI/2*0.8+PI*0.8/7*i+rang(PI/180);
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 3.0+rang(0.3);
                boss_shot.bullet[k].state = 2;
                boss_shot.bullet[k].kaiten= 1;
            }
        }
        if(t%10==0)
            se_flag[0]=1;
    }
    for(i=0;i<BOSS_BULLET_MAX;i++){
        if(boss_shot.bullet[i].flag>0){
            //tが190の時に全てストップさせ、白くし、カウントリセット
            if(boss_shot.bullet[i].state==0){
                if(t==190){
                    boss_shot.bullet[i].kaiten=0;//弾の回転を止める
                    boss_shot.bullet[i].spd   =0;
                    boss_shot.bullet[i].col   =9;
                    boss_shot.bullet[i].cnt   =0;
                    boss_shot.bullet[i].state =1;
                }
            }
            //ランダムな方向に移動し始める
            if(boss_shot.bullet[i].state==1){
                if(boss_shot.bullet[i].cnt==200){
                    boss_shot.bullet[i].angle=rang(PI);
                    boss_shot.bullet[i].kaiten=1;
                }
                if(boss_shot.bullet[i].cnt>200)
                    boss_shot.bullet[i].spd+=0.01;
            }
        }
    }
}
//恋の迷路
void boss_shot_bulletH003(){
#define TM003 600
#define DF003 20 
    int i,j,k,t=boss_shot.cnt%TM003,t2=boss_shot.cnt;
    static int tcnt,cnt,cnum;
    double angle;
    if(t2==0){//最初なら
        //40<x<FMX-40  50<y<150　の範囲で100離れた位置に80カウントで移動する
        input_phy_pos(FMX/2,FMY/2, 50);
        cnum=0;
    }
    if(t==0){//１周期の最初なら
        boss_shot.base_angle[0]=bossatan2();
        cnt=0;
        tcnt=2;
    }
    if(t<540 && t%3){
        angle=bossatan2();
        //撃たない方向なら撃たない
        if(tcnt-2==cnt || tcnt-1==cnt){
            if(tcnt-1==cnt){
                //ベースとなる角度をセット
                boss_shot.base_angle[1]=boss_shot.base_angle[0]+PI2/DF003*cnt*(cnum?-1:1)-PI2/(DF003*6)*3;
                tcnt+=DF003-2;
            }
        }
        //それじゃなければうつ
        else{
            for(i=0;i<6;i++){//１回に６発ずつうつ
                if((k=search_boss_shot())!=-1){
                    boss_shot.bullet[k].col      = cnum?1:4;
                    boss_shot.bullet[k].x     = boss.x;
                    boss_shot.bullet[k].y     = boss.y;
                    boss_shot.bullet[k].knd   = 8;
                    boss_shot.bullet[k].angle 
                        = boss_shot.base_angle[0]+PI2/DF003*cnt*(cnum?-1:1)+PI2/(DF003*6)*i*(cnum?-1:1);
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt      = 0;
                    boss_shot.bullet[k].spd      = 2;
                    se_flag[0]=1;
                }
            }
        }
        cnt++;
    }
    //少し大きな弾で円形発射
    if(40<t&&t<540 && t%30==0){
        for(j=0;j<3;j++){
            angle=boss_shot.base_angle[1]-PI2/36*4;
            for(i=0;i<27;i++){
                if((k=search_boss_shot())!=-1){
                    boss_shot.bullet[k].col   = cnum?6:0;
                    boss_shot.bullet[k].x     = boss.x;
                    boss_shot.bullet[k].y     = boss.y;
                    boss_shot.bullet[k].knd   = 7;
                    boss_shot.bullet[k].angle = angle;
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    boss_shot.bullet[k].spd   = 4-1.6/3*j;
                    se_flag[0]=1;
                }
                angle-=PI2/36;
            }
        }
    }
//    for(i=0;i<BOSS_BULLET_MAX;i++){
//        if(boss_shot.bullet[i].flag>0){
//
//        }
//    }
    if(t==TM003-1)
        cnum++;
}

//ケロちゃん風雨にも負けず
void boss_shot_bulletH004(){
#define TM004 200
    int i,j,k,n,t=boss_shot.cnt%TM004,t2=boss_shot.cnt;
    static int tm;
    double angle;
    if(t==0)
        tm=190+rang(30);
    //上からふらす弾を発射する基準の角度をセット
    angle=PI*1.5+PI/6*sin(PI2/tm*t2);
    //ギラギラ光る弾
    if(t2%4==0){
        for(n=0;n<8;n++){//8方向にふる
            if((k=search_boss_shot())!=-1){
                boss_shot.bullet[k].flag=1;
                boss_shot.bullet[k].state=0;
                boss_shot.bullet[k].cnt=0;
                boss_shot.bullet[k].knd=4;
                boss_shot.bullet[k].col=0;
                boss_shot.bullet[k].angle=0;
                boss_shot.bullet[k].eff_detail=1;//ピカピカ光らせる
                boss_shot.bullet[k].x=boss.x;
                boss_shot.bullet[k].y=boss.y;
                boss_shot.bullet[k].vx = cos(angle-PI/8*4+PI/8*n+PI/16)*3;
                boss_shot.bullet[k].vy = sin(angle-PI/8*4+PI/8*n+PI/16)*3;
            }
        }
        se_flag[0]=1;
    }
    //小さい玉
    if(t%1==0 && t2>80){
        int num=1;
        if(t%2)
            num=2;
        for(n=0;n<num;n++){
            if((k=search_boss_shot())!=-1){
                angle=PI*1.5-PI/2+PI/12*(t2%13)+rang(PI/15);
                boss_shot.bullet[k].flag=1;
                boss_shot.bullet[k].state=1;
                boss_shot.bullet[k].cnt=0;
                boss_shot.bullet[k].knd=8;
                boss_shot.bullet[k].eff_detail=0;//ピカピカは光らない
                boss_shot.bullet[k].col=4;
                boss_shot.bullet[k].angle=0;
                boss_shot.bullet[k].x=boss.x;
                boss_shot.bullet[k].y=boss.y;
                boss_shot.bullet[k].vx = cos(angle)*1.4*1.2;
                boss_shot.bullet[k].vy = sin(angle)*1.4;
            }
        }
        se_flag[7]=1;
    }
    for(i=0;i<BOSS_BULLET_MAX;i++){
        if(boss_shot.bullet[i].flag>0){
            if(boss_shot.bullet[i].state==0){//ステータスが０なら
                if(boss_shot.bullet[i].cnt<150)
                    boss_shot.bullet[i].vy+=0.03;//鉛直方向の下向きに加速する
                boss_shot.bullet[i].x+=boss_shot.bullet[i].vx;
                boss_shot.bullet[i].y+=boss_shot.bullet[i].vy;
            }
            if(boss_shot.bullet[i].state==1){//ステータスが１なら
                if(boss_shot.bullet[i].cnt<160)
                    boss_shot.bullet[i].vy+=0.03;//鉛直方向の下向きに加速する
                boss_shot.bullet[i].x+=boss_shot.bullet[i].vx;
                boss_shot.bullet[i].y+=boss_shot.bullet[i].vy;
                boss_shot.bullet[i].angle=atan2(boss_shot.bullet[i].vy,boss_shot.bullet[i].vx);
            }
        }
    }
}

//☆弾
void boss_shot_bulletH005(){
#define TM005 820
#define RANGE005 100.0
#define LEM005 4
    int i,j,k,n,t=boss_shot.cnt%TM005,t2=boss_shot.cnt;
    static int sst,bnum;
    static double sx,sy,sangle;
    //1周期の最初なら
    if(t==0){
        sst=0;
        sx=boss.x;
        sy=boss.y-RANGE005;
        sangle=PI/5/2+PI/2;
        bnum=0;
    }
    //星を描く
    if(sst<=4){
        for(i=0;i<2;i++){
            sx+=cos(sangle)*LEM005;//座標を計算
            sy+=sin(sangle)*LEM005;
            //円と交わったら
            if((sx-boss.x)*(sx-boss.x)+(sy-boss.y)*(sy-boss.y)>RANGE005*RANGE005){
                sangle-=(PI-PI/5);//方向転換
                sst++;//ステータス変換
                if(sst==5)break;//5なら終わる
            }
            for(j=0;j<5;j++){//星を５つ描く
                if((k=search_boss_shot())!=-1){
                    boss_shot.bullet[k].flag=1;
                    boss_shot.bullet[k].state=j;
                    boss_shot.bullet[k].cnt=0;
                    boss_shot.bullet[k].spd=0;
                    boss_shot.bullet[k].knd=10;
                    boss_shot.bullet[k].col=j;
                    boss_shot.bullet[k].angle=-PI/2+PI2/5*j;
                    boss_shot.bullet[k].x=sx;
                    boss_shot.bullet[k].y=sy;
                    boss_shot.bullet[k].vx = cos(sangle)*1.4*1.2;
                    boss_shot.bullet[k].vy = sin(sangle)*1.4;
                    boss_shot.bullet[k].base_angle[0]=sangle-PI+PI/20*bnum;
                }
            }
            bnum++;
        }
        se_flag[0]=1;
    }
   for(i=0;i<BOSS_BULLET_MAX;i++){
        if(boss_shot.bullet[i].flag>0){
            int cnt=boss_shot.bullet[i].cnt;
            //ステータスが10未満なら
            if(boss_shot.bullet[i].state<10){
                if(t==150){//150なら星を５方向に発射
                    boss_shot.bullet[i].spd=4;//スピード
                    boss_shot.bullet[i].cnt=0;//カウンタリセット
                    boss_shot.bullet[i].state+=10;//ステータス番号変更
                }
            }
            else if(boss_shot.bullet[i].state<20){//10~19なら
                if(cnt<=80){//減速
                    boss_shot.bullet[i].spd-=0.05;
                }
                if(cnt==100){//セットした基準角度から綺麗な曲線を描く
                    boss_shot.bullet[i].angle=boss_shot.bullet[i].base_angle[0];
                }
                if(cnt>=100 && cnt<160){//加速
                    boss_shot.bullet[i].spd+=0.015;
                }
            }
        }
    }
}
//周りから中央への発射
void boss_shot_bulletH006(){
#define TM006 300
//横一列に置く弾の数
#define DNUMX 15
    int i,j,k,t=boss_shot.cnt%TM006,t2=boss_shot.cnt;
    double xlen=FMX/DNUMX;//画面にDNUM個並べるにはxlen間隔で並べればいい
    int dnumy=FMY/xlen;//xlenずつならべると鉛直方向にいくつはいるか
    int cnum;
    if(dnumy==0){printfDx("boss_shot_bulletH006で0割り\n"); return ;}
    double ylen=FMY/dnumy;//ylenずつおけばいい
    double x,y,angle;
    static int num;
    if(t2==0)num=0;
    if(t==0){
        x=0,y=0,angle=0,cnum=0;
        for(j=0;j<4;j++){
			//水平方向ならDNUMX、鉛直方向ならdnumy
            int to = j%2 ? dnumy: DNUMX ;
			//水平方向、鉛直方向では置く個数が違うので、to回置く
            for(i=0;i<=to;i++){
                if((k=search_boss_shot())!=-1){
                    boss_shot.bullet[k].x     = x;//座標
                    boss_shot.bullet[k].y     = y;
                    boss_shot.bullet[k].knd   = 4;//弾の種類
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    switch(num){
                        case 0://発射種類１
                    boss_shot.bullet[k].col   = 0;//弾の色
                    boss_shot.bullet[k].angle = bossatan3(k,FMX/2,FMY/2);//角度
                    boss_shot.bullet[k].spd   = 1.3;//スピード
                    boss_shot.bullet[k].state = 0;//ステータス
                        break;
                        case 1://発射種類２
                    boss_shot.bullet[k].col   = 3;//弾の色
                    boss_shot.bullet[k].angle = bossatan3(k,FMX/2,FMY/2);//角度
                    boss_shot.bullet[k].spd   = 1.4+ ((j%2 ? -1 : 1) * ((cos(PI2/to*i-PI)+1)/2))*0.4;//スピード
                    boss_shot.bullet[k].state = 1;//ステータス
                        break;
                        case 2://発射種類３
                    boss_shot.bullet[k].col   = 6;//弾の色
                    boss_shot.bullet[k].angle = bossatan3(k,FMX/2,FMY/2);//角度
                    boss_shot.bullet[k].spd   = 1.3;//スピード
                    boss_shot.bullet[k].state = 2;//ステータス
                    boss_shot.bullet[k].base_angle[0] 
                    = PI/1000 * (j%2 ? -1 : 1) * ((cos(PI2/to*i-PI)+1)/2);//加算する角度
                        break;
                    }
                    se_flag[0]=1;
                    cnum++;
                }
                x+=cos(angle)*xlen;
                y+=sin(angle)*ylen;
            }
            angle+=PI/2;
        }
    }
    for(i=0;i<BOSS_BULLET_MAX;i++){
        if(boss_shot.bullet[i].flag>0){
            int cnt=boss_shot.bullet[i].cnt;
            switch(boss_shot.bullet[i].state){
                case 2://発射種類３なら
					//角度を加算
                    boss_shot.bullet[i].angle+=boss_shot.bullet[i].base_angle[0];
                    break;
                default:
                    break;
            }
        }
    }
    if(t==TM006-1)
        num=(++num)%3;
}

//周りから直線発射
void boss_shot_bulletH007(){
#define TM007 300
#define DNUMX 9
    int i,j,k,s,t=boss_shot.cnt%TM007,t2=boss_shot.cnt;
    double xlen=FMX/DNUMX;//画面にDNUM個並べるにはxlen間隔で並べればいい
    int dnumy=FMY/xlen;//xlenずつならべると鉛直方向にいくつはいるか
    int cnum;
    if(dnumy==0){printfDx("boss_shot_bulletH006で0割り\n"); return ;}
    double ylen=FMY/dnumy;//ylenずつおけばいい
    double x,y,angle;
    static int num;
    if(t2==0)num=0;
    if(t==0){
        x=0,y=0,angle=0,cnum=0;
        for(j=0;j<4;j++){
            int to=j%2 ? dnumy: DNUMX ;
            for(i=0;i<=to;i++){
                for(s=0;s<2;s++){
                    if((k=search_boss_shot())!=-1){
                        boss_shot.bullet[k].x     = x;//座標
                        boss_shot.bullet[k].y     = y;
                        boss_shot.bullet[k].knd   = 4;//弾の種類
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        switch(num){
                            case 0:
                        boss_shot.bullet[k].col   = 0;//弾の色
                        boss_shot.bullet[k].angle = angle+PI/2;//角度
                        boss_shot.bullet[k].spd   = 1.1+0.5*s;//スピード
                        boss_shot.bullet[k].state = 0;//ステータス
                            break;
                            case 1:
                        boss_shot.bullet[k].col   = 3;//弾の色
                        boss_shot.bullet[k].angle = angle+PI/2-PI/14+PI/7*s;//角度
                        boss_shot.bullet[k].spd   = 1.3;//スピード
                        boss_shot.bullet[k].state = 1;//ステータス
                            break;
                        }
                        se_flag[0]=1;
                        cnum++;
                    }
                }
                x+=cos(angle)*xlen;
                y+=sin(angle)*ylen;
            }
            angle+=PI/2;
        }
    }
    if(t==TM006-1)
        num=(++num)%2;
}