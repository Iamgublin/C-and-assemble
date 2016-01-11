#include "../include/GV.h"

extern int search_boss_shot();
extern int search_lazer();
extern int search_child();
extern double bossatan2();
extern double bossatan3(int k,double x,double y);//指定座標と弾との成す角
extern double bossatan4(int k);//指定弾と自機との成す角度
extern int move_boss_pos(double x1,double y1,double x2,double y2,double dist, int t);
extern void input_phy_pos(double x,double y,int t);
extern void input_lphy(lazer_t *laz, int time, double angle);
extern int search_boss_shot_n(int from, int to);//指定の空き番号を返す

//曲のリズムデータ
int tm12[1000]={
    459,509,559,605,653,703,751,798,848,893,941,990,1038,1086,1134,1183,1231,1277,1325,1374,1420,1469,1520,
    1571,1620,1679,1738,1814,1837,1858,1878,1897,1915,1934,1955,1974,1996,2015,2037,2057,2076,2097,2117,2137,
    2157,2176,2198,2218,2239,2258,2278,2297,2318,2337,2358,2380,2400,2421,2440,2460,2481,2500,2520,2540,2559,
    2579,2599,2621,2640,2660,2680,2701,2721,2742,2762,2782,2802,2822,2842,2863,2884,2904,2924,2944,2964,2984,
    3005,3025,3045,3065,3085,3105,3124,3144,3165,3226,3246,3265,3286,3348,3368,3387,3408,3469,3490,3508,3528,
    3592,3611,3630,3650,3710,3730,3749,3770,3832,3852,3871,3893,3913,3933,3954,3973,3993,4014,4075,4096,4116,
    4136,4156,4174,4195,4215,4237,4258,4278,4298,4318,4337,4358,4378,4398,4418,4438,4458,4477,4498,4520,4540,
    4559,4580,4601,4622,4642,4662,4680,4701,4720,4740,4760,4780,4801,4822,4842,4863,4883,4903,4924,4944,4965,
    4985,5005,5024,5043,5064,5086,5106,5127,5150,5170,5188,5208,5227,5248,5267,5288,5308,5328,5404
};

//音楽の視覚エフェクト用データ
#define VMX 52
#define VMY 30
BYTE signal[10][VMX]={
    {2,3,4,5,6,5,4,3,2,3,4,3,2,4,6,5,4,3,2,3,4,5,6,5,4,7,10,7,4,4,4,6,8,6,4,3,2,5,8,7,6,4,2,4,6,7,8,6,4,6,8,8},
    {4,5,6,8,10,7,4,6,8,5,2,4,6,8,10,9,8,7,6,7,8,9,10,11,12,8,4,4,4,6,8,6,4,7,10,8,6,7,8,9,10,7,4,4,4,5,6,5,4,5,6,6,},
    {8,7,6,4,2,3,4,6,8,6,4,4,4,5,6,6,6,8,10,7,4,3,2,3,4,7,10,9,8,6,4,5,6,8,10,7,4,6,8,9,10,11,12,10,8,7,6,6,6,5,4,4,},
    {6,5,4,7,10,7,4,4,4,3,2,6,10,7,4,3,2,4,6,5,4,5,6,8,10,7,4,3,2,3,4,3,2,6,10,6,2,3,4,3,2,3,4,3,2,5,8,5,2,4,6,6,},
    {2,3,4,7,10,9,8,5,2,3,4,3,2,3,4,6,8,9,10,6,2,4,6,8,10,8,6,5,4,5,6,5,4,7,10,7,4,3,2,2,2,5,8,7,6,5,4,3,2,3,4,4,},

    {2,3,4,5,6,8,10,11,12,15,18,13,8,7,6,7,8,11,14,18,22,20,18,20,22,21,20,18,16,17,18,20,22,17,12,15,18,16,14,15,16,13,10,8,6,5,4,6,8,5,2,2,},
    {2,6,10,7,4,7,10,18,26,18,10,9,8,7,6,9,12,17,22,22,22,16,10,9,8,6,4,7,10,12,14,19,24,19,14,13,12,9,6,8,10,14,18,22,26,23,20,16,12,8,4,4,},
    {4,7,10,14,18,13,8,14,20,18,16,17,18,14,10,15,20,14,8,8,8,7,6,7,8,9,10,13,16,15,14,12,10,9,8,9,10,14,18,12,6,8,10,9,8,13,18,12,6,5,4,4,},
    {6,8,10,11,12,16,20,17,14,11,8,6,4,5,6,8,10,14,18,14,10,14,18,14,10,12,14,18,22,19,16,13,10,8,6,7,8,9,10,8,6,7,8,14,20,17,14,15,16,15,14,14,},
    {2,5,8,12,16,10,4,5,6,6,6,11,16,11,6,11,16,10,4,7,10,14,18,17,16,13,10,8,6,8,10,8,6,6,6,7,8,15,22,15,8,7,6,6,6,5,4,9,14,9,4,4,},
};
//視覚エフェクト用構造体
typedef struct{
    int cnt;
    BYTE num[VMX];
}visual_effect_t;
visual_effect_t visual_effect;

//視覚エフェクトの四角1辺の長さ
#define VWIDE 8

void boss_shot_bulletElse(){
    int i,j,k,s,n,t=boss_shot.cnt,t2=0;
    static int num,sound_handle,base_col;
    static double lzv,base_angle;
    if(t==0){
        //この合計３行を有効にすると、numに設定した音楽の場所から始まる
        //numはtm12で設定した音楽リズムデータで、188まである。
//        num=20;
//        t=t2=tm12[num];
        boss.endtime=tm12[188]+800;
        lzv=0;
        base_col=0;
        boss.graph_flag=1;
        sound_handle=LoadSoundMem("../dat/music/Rage.wav");
//        SetSoundCurrentTime( (int)(t2*16.666666-100) , sound_handle);
        PlaySoundMem(sound_handle, DX_PLAYTYPE_BACK, FALSE);
        boss_shot.cnt=t2;
        base_angle=-PI/2;
        memset(&visual_effect,0,sizeof(visual_effect_t));
    }
    if(t==tm12[num]){//リズムをカウント
        num++;
    }
    //音楽と同期を取る処理
    if(t%60==59 && t<tm12[188]){
        int gettm=GetSoundCurrentTime(sound_handle);
        int nowflame=(int)(t*16.6666);
//        printfDx("%d\n",gettm-nowflame);
        if(abs(gettm-nowflame)>100){
            StopSoundMem(sound_handle);
            SetSoundCurrentTime( (int)(t*16.666666) , sound_handle);
            PlaySoundMem(sound_handle, DX_PLAYTYPE_BACK,FALSE);
//            printfDx("調整後%d\n",GetSoundCurrentTime(sound_handle));
        }
    }
    //最初のレーザーのダラララ～。
    if(t<180){
        for(i=0;i<2;i++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = 0;//弾の色
                boss_shot.bullet[k].x     = FMX/2;//座標
                boss_shot.bullet[k].y     = 0;
                boss_shot.bullet[k].knd   = 2;//弾の種類
                boss_shot.bullet[k].angle = PI/120*t;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 1.5;//スピード
                boss_shot.bullet[k].eff   = 1;
                boss_shot.bullet[k].state = 0;
            }
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = 4;//弾の色
                boss_shot.bullet[k].x     = FMX/2;//座標
                boss_shot.bullet[k].y     = 0;
                boss_shot.bullet[k].knd   = 2;//弾の種類
                boss_shot.bullet[k].angle = (PI+PI/240)-PI/120*t;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 2;//スピード
                boss_shot.bullet[k].eff   = 1;
                boss_shot.bullet[k].state = 0;
            }
        }
    }
    //次に青い蝶々が上からバサー
    if(t==240){
        for(j=0;j<15;j++){
            for(i=0;i<30;i++){
                if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                    boss_shot.bullet[k].col   = 1;//弾の色
                    boss_shot.bullet[k].x     = FMX/2;//座標
                    boss_shot.bullet[k].y     = 0;
                    boss_shot.bullet[k].knd   = 11;//弾の種類
                    boss_shot.bullet[k].angle = PI/30*i;//角度
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    boss_shot.bullet[k].spd   = 3-0.2*j;//スピード
                    boss_shot.bullet[k].eff   = 1;
                    boss_shot.bullet[k].state = 0;
                }
            }
        }
    }
    //フラッシュ
    if(t==tm12[0]){
        flash.flag=1;
        flash.knd=1;//フラッシュ！
        flash.to_cnt=90;//90カウントで変化
        flash.cnt=0;
    }
    //音楽とともに押し寄せるレーザー
    if(t==tm12[0]+1){
        {
            k=0;
            lazer[k].col      = 0;//弾の色
            lazer[k].knd      = 0;//弾の種類
            lazer[k].angle    = PI;//角度
            lazer[k].startpt.x= FMX;//座標
            lazer[k].startpt.y= 0;
            lazer[k].flag     = 1;
            lazer[k].cnt      = 0;
            lazer[k].haba     = 20;//幅
            lazer[k].state    = 0;//ステータス
            lazer[k].length   = FMX+60;//レーザーの長さ
            lazer[k].hantei      = 0.6;
            lazer[k].lphy.conv_flag=0;//回転フラグ
            lazer[k].lphy.conv_base_x=0;//回転基準位置
            lazer[k].lphy.conv_base_y=0;
            lazer[k].lphy.conv_x=0;//回転元の位置
            lazer[k].lphy.conv_y=0;
            input_lphy(&lazer[k],0,0);//代入
        }
        {
            k=1;
            lazer[k].col      = 0;//弾の色
            lazer[k].knd      = 0;//弾の種類
            lazer[k].angle    = 0;//角度
            lazer[k].startpt.x= 0;//座標
            lazer[k].startpt.y= FMY;
            lazer[k].flag     = 1;
            lazer[k].cnt      = 0;
            lazer[k].haba     = 20;//幅
            lazer[k].state    = 0;//ステータス
            lazer[k].length   = FMX+60;//レーザーの長さ
            lazer[k].hantei      = 0.6;
            lazer[k].lphy.conv_flag=0;//回転フラグ
            lazer[k].lphy.conv_base_x=0;//回転基準位置
            lazer[k].lphy.conv_base_y=0;
            lazer[k].lphy.conv_x=0;//回転元の位置
            lazer[k].lphy.conv_y=0;
            input_lphy(&lazer[k],0,0);//代入
        }
    }
    //対角線飛び散り
    if(tm12[1]-15<=t && t<=tm12[7]){
        for(i=1;i<=8;i++){
            if(t==tm12[i]){
                for(s=0;s<2;s++){
                    for(j=0;j<3;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 2;//弾の色
                            boss_shot.bullet[k].x     = boss.x;//座標
                            boss_shot.bullet[k].y     = boss.y;
                            boss_shot.bullet[k].knd   = 0;//弾の種類
                            boss_shot.bullet[k].angle = PI*0.75+rang(PI/10)+PI*s+PI/2*(num%2);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 6-0.7*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<5;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 1;//弾の色
                            boss_shot.bullet[k].x     = boss.x;//座標
                            boss_shot.bullet[k].y     = boss.y;
                            boss_shot.bullet[k].knd   = 1;//弾の種類
                            boss_shot.bullet[k].angle = PI*0.75+rang(PI/8)+PI*s+PI/2*(num%2);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 5.5-0.6*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<7;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 0;//弾の色
                            boss_shot.bullet[k].x     = boss.x;//座標
                            boss_shot.bullet[k].y     = boss.y;
                            boss_shot.bullet[k].knd   = 11;//弾の種類
                            boss_shot.bullet[k].angle = PI*0.75+rang(PI/6)+PI*s+PI/2*(num%2);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 5-0.6*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<15;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 0;//弾の色
                            boss_shot.bullet[k].x     = boss.x;//座標
                            boss_shot.bullet[k].y     = boss.y;
                            boss_shot.bullet[k].knd   = 10;//弾の種類
                            boss_shot.bullet[k].angle = PI*0.75+rang(PI/4)+PI*s+PI/2*(num%2);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 4-0.2*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                }
            }
        }
        for(i=1;i<=8;i++){
            if(tm12[i]-15<=t && t<=tm12[i]){
                lzv+=0.2;
                lazer[0].startpt.y+=lzv;
                lazer[1].startpt.y-=lzv;
            }
            if(t==tm12[i])
                lzv=0;
        }
    }
    //フラッシュ！
    if(t==tm12[7]){
        flash.flag=1;
        flash.knd=1;//フラッシュ！
        flash.to_cnt=90;//90カウントで変化
        flash.cnt=0;
        lazer[0].flag=0;
        lazer[1].flag=0;
    }
    //押し寄せるレーザー
    if(t==tm12[7]){
        for(i=0;i<6;i++){
            k=i;
            lazer[k].col      = 1;//弾の色
            lazer[k].knd      = 0;//弾の種類
            lazer[k].angle    = PI/2+PI*i;//角度
            lazer[k].startpt.x= 160*i+40;//座標
            lazer[k].startpt.y= FMY*(i%2);
            lazer[k].flag     = 1;
            lazer[k].cnt      = 0;
            lazer[k].haba     = 2;//幅
            lazer[k].state    = 0;//ステータス
            lazer[k].length   = FMY+60;//レーザーの長さ
            lazer[k].hantei      = 0;
            lazer[k].lphy.conv_flag=0;//回転フラグ
            lazer[k].lphy.conv_base_x=0;//回転基準位置
            lazer[k].lphy.conv_base_y=0;
            lazer[k].lphy.conv_x=0;//回転元の位置
            lazer[k].lphy.conv_y=0;
            input_lphy(&lazer[k],0,0);//代入
        }
    }
    if(tm12[8]-15<=t && t<=tm12[14]){
        for(i=8;i<=14;i++){
            if(tm12[i]-15<=t && t<=tm12[i]){
                lzv+=0.2;
                for(j=0;j<6;j++){
                    lazer[j].haba+=0.7;
                    lazer[j].startpt.x-=lzv;
                    lazer[j].hantei      = 0.6;
                }
            }
            if(t==tm12[i])
                lzv=0;
        }
    }
    //対角線飛び散り
    if(tm12[8]-15<=t && t<=tm12[14]){
        for(i=8;i<=14;i++){
            if(t==tm12[i]){
                for(s=0;s<2;s++){
                    for(j=0;j<3;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 1;//弾の色
                            boss_shot.bullet[k].x     = boss.x;//座標
                            boss_shot.bullet[k].y     = boss.y;
                            boss_shot.bullet[k].knd   = 0;//弾の種類
                            boss_shot.bullet[k].angle = PI*0.75+rang(PI/10)+PI*s+PI/2*(num%2);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 5-0.6*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<5;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 0;//弾の色
                            boss_shot.bullet[k].x     = boss.x;//座標
                            boss_shot.bullet[k].y     = boss.y;
                            boss_shot.bullet[k].knd   = 1;//弾の種類
                            boss_shot.bullet[k].angle = PI*0.75+rang(PI/8)+PI*s+PI/2*(num%2);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 4.5-0.5*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<7;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 3;//弾の色
                            boss_shot.bullet[k].x     = boss.x;//座標
                            boss_shot.bullet[k].y     = boss.y;
                            boss_shot.bullet[k].knd   = 11;//弾の種類
                            boss_shot.bullet[k].angle = PI*0.75+rang(PI/6)+PI*s+PI/2*(num%2);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 4-0.5*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<15;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 1;//弾の色
                            boss_shot.bullet[k].x     = boss.x;//座標
                            boss_shot.bullet[k].y     = boss.y;
                            boss_shot.bullet[k].knd   = 10;//弾の種類
                            boss_shot.bullet[k].angle = PI*0.75+rang(PI/4)+PI*s+PI/2*(num%2);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 3-0.1*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                }
            }
        }
    }
    if(t==tm12[14]){
        flash.flag=1;
        flash.knd=1;//フラッシュ！
        flash.to_cnt=90;//90カウントで変化
        flash.cnt=0;
        for(i=0;i<6;i++)
            lazer[i].flag=0;
    }
    //レーザー
    if(t==tm12[14]){
        for(i=0;i<6;i++){
            k=i;
            lazer[k].col      = i%2;//弾の色
            lazer[k].knd      = 0;//弾の種類
            lazer[k].angle    = PI*i;//角度
            lazer[k].startpt.x= FMX*(i%2);//座標
            lazer[k].startpt.y= 160*i;
            lazer[k].flag     = 1;
            lazer[k].cnt      = 0;
            lazer[k].haba     = 2;//幅
            lazer[k].state    = 0;//ステータス
            lazer[k].length   = FMX+60;//レーザーの長さ
            lazer[k].hantei      = 0;
            lazer[k].lphy.conv_flag=0;//回転フラグ
            lazer[k].lphy.conv_base_x=0;//回転基準位置
            lazer[k].lphy.conv_base_y=0;
            lazer[k].lphy.conv_x=0;//回転元の位置
            lazer[k].lphy.conv_y=0;
            input_lphy(&lazer[k],0,0);//代入
        }
    }
    if(tm12[15]-15<=t && t<=tm12[20]){
        for(i=14;i<=20;i++){
            if(tm12[i]-15<=t && t<=tm12[i]){
                lzv+=0.2;
                for(j=0;j<6;j++){
                    lazer[j].haba+=0.4;
                    lazer[j].startpt.y-=lzv;
                    lazer[j].hantei    = 0.6;
                }
            }
            if(t==tm12[i])
                lzv=0;
        }
    }
    //対角線飛び散り
    if(tm12[15]-15<=t && t<=tm12[20]){
        for(i=14;i<=20;i++){
            if(t==tm12[i]){
                for(s=0;s<2;s++){
                    for(j=0;j<1;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = (num+1)%2;//弾の色
                            boss_shot.bullet[k].x     = FMX*((s+num)%2);//座標
                            boss_shot.bullet[k].y     = FMY*(s);
                            boss_shot.bullet[k].knd   = 0;//弾の種類
                            boss_shot.bullet[k].angle = bossatan4(k);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 4-0.6*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<3;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = num%2;//弾の色
                            boss_shot.bullet[k].x     = FMX*((s+num)%2);//座標
                            boss_shot.bullet[k].y     = FMY*(s);
                            boss_shot.bullet[k].knd   = 1;//弾の種類
                            boss_shot.bullet[k].angle = bossatan4(k)+rang(PI/28);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 3.5-0.6*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<5;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = 3*(num%2);//弾の色
                            boss_shot.bullet[k].x     = FMX*((s+num)%2);//座標
                            boss_shot.bullet[k].y     = FMY*(s);
                            boss_shot.bullet[k].knd   = 11;//弾の種類
                            boss_shot.bullet[k].angle = bossatan4(k)+rang(PI/26);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 3-0.5*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                    for(j=0;j<10;j++){
                        if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                            boss_shot.bullet[k].col   = num%2;//弾の色
                            boss_shot.bullet[k].x     = FMX*((s+num)%2);//座標
                            boss_shot.bullet[k].y     = FMY*(s);
                            boss_shot.bullet[k].knd   = 10;//弾の種類
                            boss_shot.bullet[k].angle = bossatan4(k)+rang(PI/20);//角度
                            boss_shot.bullet[k].flag  = 1;
                            boss_shot.bullet[k].cnt   = 0;
                            boss_shot.bullet[k].spd   = 3-0.1*j;//スピード
                            boss_shot.bullet[k].eff   = 1;
                            boss_shot.bullet[k].state = 0;
                        }
                    }
                }
            }
        }
    }
    if(t==tm12[20]){
        flash.flag=1;
        flash.knd=1;//フラッシュ！
        flash.to_cnt=90;//90カウントで変化
        flash.cnt=0;
        for(i=0;i<6;i++)
            lazer[i].flag=0;
    }
    //レーザーがだららら～～
    if(tm12[21]-20<=t && t<=tm12[21]+30 && t%2){
        for(i=0;i<2;i++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = 0;//弾の色
                boss_shot.bullet[k].x     = FMX;//座標
                boss_shot.bullet[k].y     = 0;
                boss_shot.bullet[k].knd   = 2;//弾の種類
                boss_shot.bullet[k].angle = PI-PI/2/50*(t-(tm12[21]-20));//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 3;//スピード
                boss_shot.bullet[k].eff   = 1;
                boss_shot.bullet[k].state = 0;
            }
        }
    }
    if(tm12[23]-20<=t && t<=tm12[23]+30 && t%2){
        for(i=0;i<2;i++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = 4;//弾の色
                boss_shot.bullet[k].x     = 0;//座標
                boss_shot.bullet[k].y     = FMY;
                boss_shot.bullet[k].knd   = 2;//弾の種類
                boss_shot.bullet[k].angle = -PI/2/50*(t-(tm12[23]-20));//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 3;//スピード
                boss_shot.bullet[k].eff   = 1;
                boss_shot.bullet[k].state = 0;
            }
        }
    }
    //4四方から発射
    double x4[4]={0,FMX,FMX,0};
    double y4[4]={0,0,FMY,FMY};
    if(t==tm12[25]){
        for(j=0;j<4;j++){
            for(i=0;i<90;i++){
                if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                    boss_shot.bullet[k].col   = 1;//弾の色
                    boss_shot.bullet[k].x     = x4[j];//座標
                    boss_shot.bullet[k].y     = y4[j];
                    boss_shot.bullet[k].knd   = 2;//弾の種類
                    boss_shot.bullet[k].angle = PI/2*j+PI/2/90*i;//角度
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    boss_shot.bullet[k].spd   = 3;//スピード
                    boss_shot.bullet[k].eff   = 1;
                    boss_shot.bullet[k].state = 1;
                }
            }
        }
    }
    if(t==tm12[27]){
        flash.flag=1;
        flash.knd=1;//フラッシュ！
        flash.to_cnt=90;//90カウントで変化
        flash.cnt=0;
        for(i=0;i<6;i++)
            lazer[i].flag=0;
    }
    //区切り。視覚エフェクト
    if(t>=tm12[27] && t<tm12[180] && (t-tm12[28])%20==0){
        visual_effect.cnt=0;
        int r=GetRand(4);
        if(num<100 && (num-27)%16==0)
            r+=5;
        for(j=0;j<VMX;j++){
            n=signal[r][j]*0.7-visual_effect.num[j];
            for(i=0;i<n;i++){
                k=VMY*j+visual_effect.num[j]+i;
                boss_shot.bullet[k].col   = 0;//弾の色
                boss_shot.bullet[k].x     = k/VMY*VWIDE;//座標
                boss_shot.bullet[k].y     = (k%VMY)*VWIDE;
                boss_shot.bullet[k].knd   = 12;//弾の種類
                boss_shot.bullet[k].angle = 0;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].eff  = 1;
                boss_shot.bullet[k].state = 0;//状態
                boss_shot.bullet[k].spd   = 0;//スピード
            }
            if(n>0)
                visual_effect.num[j]+=n;
        }
    }
    visual_effect.cnt++;
    for(j=0;j<VMX;j++){
        if(visual_effect.cnt>=5 && GetRand(3)==0){
            if(visual_effect.num[j]==0)continue;
            if(visual_effect.num[j]<=2 && visual_effect.cnt%2==0)continue;
            if(visual_effect.num[j]<=4 && visual_effect.cnt%4==0)continue;
            if(visual_effect.num[j]<=6 && visual_effect.cnt%6==0)continue;
            k=VMY*j+visual_effect.num[j];
            boss_shot.bullet[k].flag=0;
            visual_effect.num[j]--;
        }
    }
    //レーザー
    if(t>=tm12[26] && t<tm12[90]){
        for(i=26;i<=90;i++){
            if(t==tm12[i] && (num-26+6)%8==0 && num<=57 ||
                t==tm12[i] && (num-58+2)%4==0 && num>=58 && num<=90 ){
//                double angle=atan2(ch.y,ch.x-FMX/2);
                if(num<85){
                    double angle=rang(PI);
                    for(j=0;j<15;j++){
                        for(s=0;s<40;s++){
                            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                                boss_shot.bullet[k].col   = 0;//弾の色
                                boss_shot.bullet[k].x     = FMX/2;//座標
                                boss_shot.bullet[k].y     = 0;
                                boss_shot.bullet[k].knd   = 11;//弾の種類
                                boss_shot.bullet[k].angle = angle+PI2/40*s;//角度
                                boss_shot.bullet[k].flag  = 1;
                                boss_shot.bullet[k].cnt   = 0;
                                boss_shot.bullet[k].spd   = 5-3.0/15.0*j;//スピード
                                boss_shot.bullet[k].eff   = 1;
                                boss_shot.bullet[k].state = 0;
                            }
                        }
                    }
                }
                for(j=0;j<11;j++){
                    if((k=search_lazer())!=-1){
                        lazer[k].col      = 0;//弾の色
                        lazer[k].knd      = 0;//弾の種類
                        lazer[k].angle    = -PI/2+PI/10*j+rang(PI/20);//角度
                        lazer[k].startpt.x= 0;//座標
                        lazer[k].startpt.y= FMY/2+rang(50);
                        lazer[k].flag     = 1;
                        lazer[k].cnt      = 0;
                        lazer[k].haba     = 1;//幅
                        lazer[k].state    = 1;//ステータス
                        if(num>=58)
                            lazer[k].state=2;
                        lazer[k].length   = FMX*1.2;//レーザーの長さ
                        lazer[k].hantei      = 0;
                        lazer[k].lphy.conv_flag=0;//回転フラグ
                        lazer[k].lphy.conv_base_x=0;//回転基準位置
                        lazer[k].lphy.conv_base_y=0;
                        lazer[k].lphy.conv_x=0;//回転元の位置
                        lazer[k].lphy.conv_y=0;
                        input_lphy(&lazer[k],0,0);//代入
                    }
                }
                for(j=11;j<22;j++){
                    if((k=search_lazer())!=-1){
                        lazer[k].col      = 1;//弾の色
                        lazer[k].knd      = 0;//弾の種類
                        lazer[k].angle    = PI+PI/2+PI/10*j+rang(PI/20);//角度
                        lazer[k].startpt.x= FMX;//座標
                        lazer[k].startpt.y= FMY/2+rang(50);
                        lazer[k].flag     = 1;
                        lazer[k].cnt      = 0;
                        lazer[k].haba     = 1;//幅
                        lazer[k].state    = 1;//ステータス
                        if(num>=58)
                            lazer[k].state=2;
                        lazer[k].length   = FMX*1.2;//レーザーの長さ
                        lazer[k].hantei      = 0;
                        lazer[k].lphy.conv_flag=0;//回転フラグ
                        lazer[k].lphy.conv_base_x=0;//回転基準位置
                        lazer[k].lphy.conv_base_y=0;
                        lazer[k].lphy.conv_x=0;//回転元の位置
                        lazer[k].lphy.conv_y=0;
                        input_lphy(&lazer[k],0,0);//代入
                    }
                }
            }
        }
    }
    //91から たーたーたーた～～↑♪
    int st,ed;
    st=91; ed=94;
    if(tm12[st]<=t && t<=tm12[ed]){
        for(i=st;i<=ed;i++){
            if(tm12[i]==t){
                for(s=0;s<3;s++){
                    int setx[3]={0,-30,30};
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 0;//弾の色
                        boss_shot.bullet[k].x     = setx[s]+50+(FMX-100)/3*(num-1-st);//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 0;//弾の種類
                        boss_shot.bullet[k].angle = PI/2;//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 6-s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
                for(s=0;s<6;s++){
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 1;//弾の色
                        boss_shot.bullet[k].x     = rang(30)+50+(FMX-100)/3*(num-1-st);//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 1;//弾の種類
                        boss_shot.bullet[k].angle = PI/2;//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 5.5-3.0/6*s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
                for(s=0;s<10;s++){
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 0;//弾の色
                        boss_shot.bullet[k].x     = rang(20)+50+(FMX-100)/3*(num-1-st);//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 10;//弾の種類
                        boss_shot.bullet[k].angle = PI/2;//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 5.5-4.0/10*s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
            }
        }
    }
    //た～た～た～た～↑♪2回目
    st=95; ed=98;
    if(tm12[st]<=t && t<=tm12[ed]){
        for(i=st;i<=ed;i++){
            if(tm12[i]==t){
                for(s=0;s<3;s++){
                    int setx[3]={0,-30,30};
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 1;//弾の色
                        boss_shot.bullet[k].x     = setx[s]+FMX-50-(FMX-100)/3*(num-1-st);//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 0;//弾の種類
                        boss_shot.bullet[k].angle = PI/2;//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 6-s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
                for(s=0;s<6;s++){
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 0;//弾の色
                        boss_shot.bullet[k].x     = rang(30)+FMX-50-(FMX-100)/3*(num-1-st);//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 1;//弾の種類
                        boss_shot.bullet[k].angle = PI/2;//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 5.5-3.0/6*s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
                for(s=0;s<10;s++){
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 1;//弾の色
                        boss_shot.bullet[k].x     = rang(20)+FMX-50-(FMX-100)/3*(num-1-st);//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 10;//弾の種類
                        boss_shot.bullet[k].angle = PI/2;//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 5.5-4.0/10*s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
            }
        }
    }
    //斜めに たーたーたーた～～↑♪
    st=99; ed=102;
    if(tm12[st]<=t && t<=tm12[ed]){
        for(i=st;i<=ed;i++){
            if(tm12[i]==t){
                for(s=0;s<3;s++){
                    double setx[3]={0,-PI/20,PI/20};
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 0;//弾の色
                        boss_shot.bullet[k].x     = 0;//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 0;//弾の種類
                        boss_shot.bullet[k].angle = setx[s]+PI/2-(PI/2-PI/10)/3*(num-1-st);//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 6-s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
                for(s=0;s<6;s++){
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 1;//弾の色
                        boss_shot.bullet[k].x     = 0;//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 1;//弾の種類
                        boss_shot.bullet[k].angle = PI/2-(PI/2-PI/10)/3*(num-1-st)+rang(PI/20);//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 5.5-3.0/6*s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
                for(s=0;s<10;s++){
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 0;//弾の色
                        boss_shot.bullet[k].x     = 0;//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 10;//弾の種類
                        boss_shot.bullet[k].angle = PI/2-(PI/2-PI/10)/3*(num-1-st)+rang(PI/30);//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 5.5-4.0/10*s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
            }
        }
    }
    st=103; ed=106;
    if(tm12[st]<=t && t<=tm12[ed]){
        for(i=st;i<=ed;i++){
            if(tm12[i]==t){
                for(s=0;s<3;s++){
                    double setx[3]={0,-PI/20,PI/20};
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 1;//弾の色
                        boss_shot.bullet[k].x     = FMX;//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 0;//弾の種類
                        boss_shot.bullet[k].angle = setx[s]+PI/2+(PI/2-PI/10)/3*(num-1-st);//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 6-s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
                for(s=0;s<6;s++){
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 0;//弾の色
                        boss_shot.bullet[k].x     = FMX;//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 1;//弾の種類
                        boss_shot.bullet[k].angle = PI/2+(PI/2-PI/10)/3*(num-1-st)+rang(PI/20);//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 5.5-3.0/6*s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
                for(s=0;s<10;s++){
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = 1;//弾の色
                        boss_shot.bullet[k].x     = FMX;//座標
                        boss_shot.bullet[k].y     = 0;
                        boss_shot.bullet[k].knd   = 10;//弾の種類
                        boss_shot.bullet[k].angle = PI/2+(PI/2-PI/10)/3*(num-1-st)+rang(PI/30);//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 5.5-4.0/10*s;//スピード
                        boss_shot.bullet[k].eff   = 0;
                        boss_shot.bullet[k].state = 0;
                    }
                }
            }
        }
    }
    //た～た～た～ラストカーテン
    st=106;
    if(tm12[st]==t){
        for(n=st;n<st+3;n++){
            int setx[3]={0,-30,30};
            for(s=0;s<3;s++){
                if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                    boss_shot.bullet[k].col   = n%2 ? 1 : 0;//弾の色
                    boss_shot.bullet[k].x     = setx[s]+FMX-50-(FMX-100)/2*(n-st);//座標
                    boss_shot.bullet[k].y     = 0;
                    boss_shot.bullet[k].knd   = 0;//弾の種類
                    boss_shot.bullet[k].angle = PI/2;//角度
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    boss_shot.bullet[k].spd   = 6-s;//スピード
                    boss_shot.bullet[k].eff   = 0;
                    boss_shot.bullet[k].state = 0;
                }
            }
            for(s=0;s<6;s++){
                if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                    boss_shot.bullet[k].col   = n%2 ? 0 : 1;//弾の色
                    boss_shot.bullet[k].x     = rang(30)+FMX-50-(FMX-100)/2*(n-st);//座標
                    boss_shot.bullet[k].y     = 0;
                    boss_shot.bullet[k].knd   = 1;//弾の種類
                    boss_shot.bullet[k].angle = PI/2;//角度
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    boss_shot.bullet[k].spd   = 5.5-3.0/6*s;//スピード
                    boss_shot.bullet[k].eff   = 0;
                    boss_shot.bullet[k].state = 0;
                }
            }
            for(s=0;s<10;s++){
                if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                    boss_shot.bullet[k].col   = n%2 ? 1 : 0;//弾の色
                    boss_shot.bullet[k].x     = rang(20)+FMX-50-(FMX-100)/2*(n-st);//座標
                    boss_shot.bullet[k].y     = 0;
                    boss_shot.bullet[k].knd   = 10;//弾の種類
                    boss_shot.bullet[k].angle = PI/2;//角度
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    boss_shot.bullet[k].spd   = 5.5-4.0/10*s;//スピード
                    boss_shot.bullet[k].eff   = 0;
                    boss_shot.bullet[k].state = 0;
                }
            }
        }
    }
    //夢終劇参考
    st=107;
    if(tm12[st]<=t && t<=tm12[st]+60 && t%5==0){
        for(s=0;s<=150;s++){
            if(s%6<4)continue;
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = (s/6)%9;//弾の色
                boss_shot.bullet[k].x     = boss.x;//座標
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 2;//弾の種類
                boss_shot.bullet[k].angle = 0;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 0;//スピード
                boss_shot.bullet[k].eff   = 0;
                boss_shot.bullet[k].state = 2;
                boss_shot.bullet[k].vx    = -2.5+5.0/150*s;
                boss_shot.bullet[k].vy    = -8;
            }
        }
    }
    //両端下
/*    st=107;
    if(tm12[111]-(tm12[111]-tm12[110])/3==t){
        for(s=0;s<=30;s++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = 0;//弾の色
                boss_shot.bullet[k].x     = 0;//座標
                boss_shot.bullet[k].y     = FMY;
                boss_shot.bullet[k].knd   = 11;//弾の種類
                boss_shot.bullet[k].angle = -PI/4+rang(PI/4);//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 1.5+rang(1);//スピード
                boss_shot.bullet[k].eff   = 1;
                boss_shot.bullet[k].state = 0;
            }
        }
        for(s=0;s<=30;s++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = 0;//弾の色
                boss_shot.bullet[k].x     = FMX;//座標
                boss_shot.bullet[k].y     = FMY;
                boss_shot.bullet[k].knd   = 11;//弾の種類
                boss_shot.bullet[k].angle = PI+PI/4+rang(PI/4);//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 1.5+rang(1);//スピード
                boss_shot.bullet[k].eff   = 1;
                boss_shot.bullet[k].state = 0;
            }
        }
    }*/
    //夢終劇参考
    st=111; ed=114;
    if(tm12[st]<=t && t<=tm12[ed]){
        for(i=st;i<=ed;i++){
            if(tm12[i]<=t && t<tm12[i]+60){
                for(s=150/4*(num-st-1);s<=150/4*(num-st);s++){
                    if(s%6<4)continue;
                    if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                        boss_shot.bullet[k].col   = (s/6)%9;//弾の色
                        boss_shot.bullet[k].x     = boss.x;//座標
                        boss_shot.bullet[k].y     = boss.y;
                        boss_shot.bullet[k].knd   = 2;//弾の種類
                        boss_shot.bullet[k].angle = 0;//角度
                        boss_shot.bullet[k].flag  = 1;
                        boss_shot.bullet[k].cnt   = 0;
                        boss_shot.bullet[k].spd   = 0;//スピード
                        boss_shot.bullet[k].eff   = 1;
                        boss_shot.bullet[k].state = 2;
                        boss_shot.bullet[k].vx    = -2.5+5.0/150*s;
                        boss_shot.bullet[k].vy    = -8;
                    }
                }
            }
        }
    }
    //レーザー
    st=115; ed=123;
    if(tm12[st]<=t && t<=tm12[ed]){
        for(i=st;i<=ed;i++){
            if(tm12[i]==t){
                for(s=0;s<2;s++){
                    if((k=search_lazer())!=-1){
                        lazer[k].col      = 0;//弾の色
                        lazer[k].knd      = 0;//弾の種類
                        lazer[k].angle    = PI/2+rang(PI/2)-PI/4+PI/2*s;//角度
                        lazer[k].startpt.x= FMX/2;//座標
                        lazer[k].startpt.y= 0;
                        lazer[k].flag     = 1;
                        lazer[k].cnt      = 0;
                        lazer[k].haba     = 1;//幅
                        lazer[k].state    = 3;//ステータス
                        lazer[k].length   = FMY*1.2;//レーザーの長さ
                        lazer[k].hantei      = 0;
                        lazer[k].lphy.conv_flag=0;//回転フラグ
                        lazer[k].lphy.conv_base_x=0;//回転基準位置
                        lazer[k].lphy.conv_base_y=0;
                        lazer[k].lphy.conv_x=0;//回転元の位置
                        lazer[k].lphy.conv_y=0;
                        input_lphy(&lazer[k],0,0);//代入
                    }
                    if((k=search_lazer())!=-1){
                        lazer[k].col      = 1;//弾の色
                        lazer[k].knd      = 0;//弾の種類
                        lazer[k].angle    = -PI/2+rang(PI/2)-PI/4+PI/2*s;//角度
                        lazer[k].startpt.x= FMX/2;//座標
                        lazer[k].startpt.y= FMY;
                        lazer[k].flag     = 1;
                        lazer[k].cnt      = 0;
                        lazer[k].haba     = 1;//幅
                        lazer[k].state    = 3;//ステータス
                        lazer[k].length   = FMX*1.2;//レーザーの長さ
                        lazer[k].hantei      = 0;
                        lazer[k].lphy.conv_flag=0;//回転フラグ
                        lazer[k].lphy.conv_base_x=0;//回転基準位置
                        lazer[k].lphy.conv_base_y=0;
                        lazer[k].lphy.conv_x=0;//回転元の位置
                        lazer[k].lphy.conv_y=0;
                        input_lphy(&lazer[k],0,0);//代入
                    }
                }
            }
        }
    }
    if(tm12[124]==t){
        for(i=0;i<8;i++){
            for(s=0;s<=40;s++){
                if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                    boss_shot.bullet[k].col   = 0;//弾の色
                    boss_shot.bullet[k].x     = FMX/2;//座標
                    boss_shot.bullet[k].y     = 0;
                    boss_shot.bullet[k].knd   = 11;//弾の種類
                    boss_shot.bullet[k].angle = PI/40*s+rang(PI/80);//角度
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    boss_shot.bullet[k].spd   = 4.5-3.5/8*i+rang(0.2);//スピード
                    boss_shot.bullet[k].eff   = 1;
                    boss_shot.bullet[k].state = 0;
                }
            }
        }
    }
    if(t==tm12[125]){
        flash.flag=1;
        flash.knd=1;//フラッシュ！
        flash.to_cnt=50;//90カウントで変化
        flash.cnt=0;
    }

    //ふらあぁっしゅ

    if(tm12[133]<=t && t<tm12[153] && t%3==0){
        for(i=0;i<2;i++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = base_col;//弾の色
                boss_shot.bullet[k].x     = boss.x;//座標
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 1;//弾の種類
                boss_shot.bullet[k].angle = base_angle-PI*0.15+PI*0.3*i;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 9;//スピード
                boss_shot.bullet[k].eff   = 0;
                boss_shot.bullet[k].state = 3;
            }
        }
    }
    if(tm12[133]==t||tm12[137]==t||tm12[141]==t){
        if((k=search_lazer())!=-1){
            lazer[k].col      = 1;//弾の色
            lazer[k].knd      = 0;//弾の種類
            lazer[k].angle    = bossatan2();//角度
            lazer[k].startpt.x= boss.x;//座標
            lazer[k].startpt.y= boss.y;
            lazer[k].flag     = 1;
            lazer[k].cnt      = 0;
            lazer[k].haba     = 1;//幅
            lazer[k].state    = 4;//ステータス
            lazer[k].length   = FMX;//レーザーの長さ
            lazer[k].hantei   = 0;
            lazer[k].lphy.conv_flag=0;//回転フラグ
            lazer[k].lphy.conv_base_x=0;//回転基準位置
            lazer[k].lphy.conv_base_y=0;
            lazer[k].lphy.conv_x=0;//回転元の位置
            lazer[k].lphy.conv_y=0;
            input_lphy(&lazer[k],0,0);//代入
        }
    }
    if(tm12[137]==t){
        base_angle+=PI/6+rang(PI/20);
        base_col=1;
    }
    if(tm12[141]==t){
        base_angle-=PI/6*2+rang(PI/20);
        base_col=0;
    }
    if(tm12[145]==t){
        base_angle+=PI/4+rang(PI/20);
        base_col=1;
    }
    if(tm12[147]==t){
        base_angle=-PI/2;
        base_col=0;
    }
    if(tm12[147]<=t && t<tm12[161] && t%3==0){
        for(i=0;i<3;i++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = base_col;//弾の色
                boss_shot.bullet[k].x     = boss.x;//座標
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 1;//弾の種類
                boss_shot.bullet[k].angle = base_angle-PI*0.15+PI*0.15*i;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 9;//スピード
                boss_shot.bullet[k].eff   = 0;
                boss_shot.bullet[k].state = 3;
            }
        }
    }
    if(tm12[147]==t||tm12[151]==t||tm12[155]==t){
        for(i=0;i<3;i++){
            if((k=search_lazer())!=-1){
                lazer[k].col      = i%2;//弾の色
                lazer[k].knd      = 0;//弾の種類
                lazer[k].angle    = bossatan2()-PI/6+PI/6*i;//角度
                lazer[k].startpt.x= boss.x;//座標
                lazer[k].startpt.y= boss.y;
                lazer[k].flag     = 1;
                lazer[k].cnt      = 0;
                lazer[k].haba     = 1;//幅
                lazer[k].state    = 4;//ステータス
                lazer[k].length   = FMX;//レーザーの長さ
                lazer[k].hantei   = 0;
                lazer[k].lphy.conv_flag=0;//回転フラグ
                lazer[k].lphy.conv_base_x=0;//回転基準位置
                lazer[k].lphy.conv_base_y=0;
                lazer[k].lphy.conv_x=0;//回転元の位置
                lazer[k].lphy.conv_y=0;
                input_lphy(&lazer[k],0,0);//代入
            }
        }
    }
    if(tm12[151]==t){
        base_angle-=PI/10+rang(PI/20);
        base_col=1;
    }
    if(tm12[155]==t){
        base_angle+=PI/5+rang(PI/20);
        base_col=0;
    }
    if(tm12[159]==t){
        base_angle-=PI/4+rang(PI/20);
        base_col=1;
    }
    if(tm12[161]==t){
        base_angle=-PI/2;
        base_col=0;
    }
    if(tm12[161]<=t && t<tm12[173] && t%3==0){
        for(i=0;i<4;i++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = base_col;//弾の色
                boss_shot.bullet[k].x     = boss.x;//座標
                boss_shot.bullet[k].y     = boss.y;
                boss_shot.bullet[k].knd   = 1;//弾の種類
                boss_shot.bullet[k].angle = base_angle-PI*0.18+PI*0.12*i;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 9;//スピード
                boss_shot.bullet[k].eff   = 0;
                boss_shot.bullet[k].state = 3;
            }
        }
    }
    if(tm12[161]==t||tm12[165]==t||tm12[169]==t){
        for(i=0;i<6;i++){
            if((k=search_lazer())!=-1){
                lazer[k].col      = (i+1)%2;//弾の色
                lazer[k].knd      = 0;//弾の種類
                lazer[k].angle    = bossatan2()+PI2/6*i;//角度
                lazer[k].startpt.x= boss.x;//座標
                lazer[k].startpt.y= boss.y;
                lazer[k].flag     = 1;
                lazer[k].cnt      = 0;
                lazer[k].haba     = 1;//幅
                lazer[k].state    = 4;//ステータス
                lazer[k].length   = FMX;//レーザーの長さ
                lazer[k].hantei   = 0;
                lazer[k].lphy.conv_flag=0;//回転フラグ
                lazer[k].lphy.conv_base_x=0;//回転基準位置
                lazer[k].lphy.conv_base_y=0;
                lazer[k].lphy.conv_x=0;//回転元の位置
                lazer[k].lphy.conv_y=0;
                input_lphy(&lazer[k],0,0);//代入
            }
        }
    }
    if(tm12[165]==t){
        base_angle-=PI/10+rang(PI/20);
        base_col=1;
    }
    if(tm12[169]==t){
        base_angle+=PI/5+rang(PI/20);
        base_col=0;
    }
    if(tm12[161]==t||tm12[165]==t||tm12[169]==t){
        for(i=0;i<6;i++){
            if((k=search_lazer())!=-1){
                lazer[k].col      = (i+1)%2;//弾の色
                lazer[k].knd      = 0;//弾の種類
                lazer[k].angle    = bossatan2()+PI2/6*i;//角度
                lazer[k].startpt.x= boss.x;//座標
                lazer[k].startpt.y= boss.y;
                lazer[k].flag     = 1;
                lazer[k].cnt      = 0;
                lazer[k].haba     = 1;//幅
                lazer[k].state    = 4;//ステータス
                lazer[k].length   = FMX;//レーザーの長さ
                lazer[k].hantei   = 0;
                lazer[k].lphy.conv_flag=0;//回転フラグ
                lazer[k].lphy.conv_base_x=0;//回転基準位置
                lazer[k].lphy.conv_base_y=0;
                lazer[k].lphy.conv_x=0;//回転元の位置
                lazer[k].lphy.conv_y=0;
                input_lphy(&lazer[k],0,0);//代入
            }
        }
    }
    if(tm12[175]==t){
        for(i=0;i<6;i++){
            if((k=search_lazer())!=-1){
                lazer[k].col      = 0;//弾の色
                lazer[k].knd      = 0;//弾の種類
                lazer[k].angle    = base_angle+PI2/6*i;//角度
                lazer[k].startpt.x= boss.x;//座標
                lazer[k].startpt.y= boss.y;
                lazer[k].flag     = 1;
                lazer[k].cnt      = 0;
                lazer[k].haba     = 1;//幅
                lazer[k].state    = 5;//ステータス
                lazer[k].length   = FMX;//レーザーの長さ
                lazer[k].hantei   = 0;
                lazer[k].lphy.conv_flag=0;//回転フラグ
                lazer[k].lphy.conv_base_x=0;//回転基準位置
                lazer[k].lphy.conv_base_y=0;
                lazer[k].lphy.conv_x=0;//回転元の位置
                lazer[k].lphy.conv_y=0;
                input_lphy(&lazer[k],0,0);//代入
            }
        }
    }
    if(tm12[179]==t){
        for(i=0;i<6;i++){
            if((k=search_lazer())!=-1){
                lazer[k].col      = 1;//弾の色
                lazer[k].knd      = 0;//弾の種類
                lazer[k].angle    = base_angle+PI2/6*i;//角度
                lazer[k].startpt.x= boss.x;//座標
                lazer[k].startpt.y= boss.y;
                lazer[k].flag     = 1;
                lazer[k].cnt      = 0;
                lazer[k].haba     = 1;//幅
                lazer[k].state    = 6;//ステータス
                lazer[k].length   = FMX;//レーザーの長さ
                lazer[k].hantei   = 0;
                lazer[k].lphy.conv_flag=0;//回転フラグ
                lazer[k].lphy.conv_base_x=0;//回転基準位置
                lazer[k].lphy.conv_base_y=0;
                lazer[k].lphy.conv_x=0;//回転元の位置
                lazer[k].lphy.conv_y=0;
                input_lphy(&lazer[k],0,0);//代入
            }
        }
    }
    if(tm12[183]-60==t){
        double sx=FMX/2,sy=420;
        for(i=0;i<20;i++){
            if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                boss_shot.bullet[k].col   = i%9;//弾の色
                boss_shot.bullet[k].x     = sx+cos(PI2/20*i)*400;//座標
                boss_shot.bullet[k].y     = sy+sin(PI2/20*i)*400;
                boss_shot.bullet[k].knd   = 4;//弾の種類
                boss_shot.bullet[k].angle = PI2/20*i+PI;//角度
                boss_shot.bullet[k].flag  = 1;
                boss_shot.bullet[k].cnt   = 0;
                boss_shot.bullet[k].spd   = 4;//スピード
                boss_shot.bullet[k].eff   = 1;
                boss_shot.bullet[k].state = 4;
                boss_shot.bullet[k].till  = 1000;
            }
        }
    }
    if(t==tm12[187]){
        flash.flag=1;
        flash.knd=1;//フラッシュ！
        flash.to_cnt=120;//90カウントで変化
        flash.cnt=0;
    }
    int rflag=0;
    double rx,ry;
    for(i=VMX*VMY;i<BOSS_BULLET_MAX;i++){
        if(boss_shot.bullet[i].flag>0){
            int cnt=boss_shot.bullet[i].cnt;
            int state=boss_shot.bullet[i].state;
            double x=boss_shot.bullet[i].x,y=boss_shot.bullet[i].y;
            if(t==tm12[0]||t==tm12[7]||t==tm12[14]||t==tm12[20]||t==tm12[27])
                boss_shot.bullet[i].flag=0;
            if(state==1){
                if(cnt>35)
                    boss_shot.bullet[i].spd*=0.983;
            }
            if(state==2){
                if(boss_shot.bullet[i].vy<8)
                    boss_shot.bullet[i].vy+=0.15;
                boss_shot.bullet[i].x+=boss_shot.bullet[i].vx;
                boss_shot.bullet[i].y+=boss_shot.bullet[i].vy;
                boss_shot.bullet[i].angle=atan2(boss_shot.bullet[i].vy,boss_shot.bullet[i].vx);
            }
            if(state==3){
                if(x<0+9 || FMX-9<x || y<0+9){
                    if(x<0+9 || FMX-9<x )
                        boss_shot.bullet[i].angle=PI-boss_shot.bullet[i].angle;
                    if(y<0+9)
                        boss_shot.bullet[i].angle=PI2-boss_shot.bullet[i].angle;
                }
            }
            if(state==4){
                if(cnt==100){
                    boss_shot.bullet[i].spd=0;
                }
                if(tm12[187]==t){
                    boss_shot.bullet[i].vy=10;
                    boss_shot.bullet[i].state=5;
                }
            }
            if(state==5){
                if(boss_shot.bullet[i].vy>0){
                    boss_shot.bullet[i].vy-=0.15;
                    if(boss_shot.bullet[i].vy<0)
                        boss_shot.bullet[i].vy=0;
                }
                boss_shot.bullet[i].y-=boss_shot.bullet[i].vy;
                if(t==tm12[188]){
                    rx=boss_shot.bullet[i].x;
                    ry=boss_shot.bullet[i].y;
                    rflag=1;
                    boss_shot.bullet[i].flag=0;
                }
            }
            if(state==6){
                if(cnt<180)
                    boss_shot.bullet[i].spd*=0.98;
                if(cnt>180)
                    boss_shot.bullet[i].spd/=0.99;
                if(cnt>480)
                    boss_shot.bullet[i].spd/=0.99;
            }
        }
    }
    if(rflag==1){
        for(j=0;j<=20;j++){
            for(i=0;i<100-j*4;i++){
                if((k=search_boss_shot_n(VMX*VMY,BOSS_BULLET_MAX))!=-1){
                    boss_shot.bullet[k].col   = GetRand(8);//弾の色
                    boss_shot.bullet[k].x     = rx;//座標
                    boss_shot.bullet[k].y     = ry;
                    boss_shot.bullet[k].knd   = 11;//弾の種類
                    boss_shot.bullet[k].angle = PI2/(100-j*4)*i+rang(PI/200);//角度
                    boss_shot.bullet[k].flag  = 1;
                    boss_shot.bullet[k].cnt   = 0;
                    boss_shot.bullet[k].spd   = 9-9/20.0*j+rang(0.2);//スピード
                    if(boss_shot.bullet[k].spd<0.1)
                        boss_shot.bullet[k].spd=0.1;
                    boss_shot.bullet[k].eff   = 1;
                    boss_shot.bullet[k].state = 6;
                    boss_shot.bullet[k].till  = 0;
                }
            }
        }
    }
    for(i=0;i<LAZER_MAX;i++){
        if(lazer[i].flag>0){
            int cnt=lazer[i].cnt;
            int state=lazer[i].state;
            if(state==1){
                if(cnt>=20*4-7 && cnt<20*4){
                    lazer[i].haba+=1.5;
                }
                if(cnt==20*4+1)
                    lazer[i].hantei=0.6;
                if(cnt>=20*8-30-7 && cnt<20*8-30){
                    lazer[i].hantei=0;
                    lazer[i].haba-=1.5;
                }
                if(cnt==20*8-30){
                    lazer[i].flag=0;
                }
            }
            if(state==2){
                if(cnt>=20*2-5 && cnt<20*2){
                    lazer[i].haba+=1.5;
                }
                if(cnt==20*2+1)
                    lazer[i].hantei=0.6;
                if(cnt>=20*6-30-5 && cnt<20*6-30){
                    lazer[i].hantei=0;
                    lazer[i].haba-=1.5;
                }
                if(cnt==20*6-30){
                    lazer[i].flag=0;
                }
            }
            if(state==3){
                if(t>=tm12[124]-7 && t<tm12[124]){
                    lazer[i].haba+=1;
                }
                if(t==tm12[124]+2)
                    lazer[i].hantei=0.6;
                if(t==tm12[125]){
                    lazer[i].flag=0;
                }
            }
            if(state==4){
                if(cnt>=20*2-10 && cnt<20*2){
                    lazer[i].haba+=1;
                }
                if(cnt==20*2+2)
                    lazer[i].hantei=0.6;
                if(cnt>=20*4-10 && cnt<20*4){
                    lazer[i].hantei=0;
                    lazer[i].haba-=1;
                }
                if(cnt==20*4){
                    lazer[i].flag=0;
                }
            }
            if(state==5){
                lazer[i].angle+=PI/180;
                if(30<cnt && cnt<40){
                    lazer[i].haba+=3.0;
                }
                if(cnt==45)
                    lazer[i].hantei=0.6;
                if(cnt>=70 && cnt<80){
                    lazer[i].hantei=0;
                    lazer[i].haba-=3.0;
                }
                if(cnt==80){
                    lazer[i].flag=0;
                }
            }
            if(state==6){
                lazer[i].angle-=PI/180;
                if(30<cnt && cnt<40){
                    lazer[i].haba+=3.0;
                }
                if(cnt==45)
                    lazer[i].hantei=0.6;
                if(cnt>=70 && cnt<80){
                    lazer[i].hantei=0;
                    lazer[i].haba-=3.0;
                }
                if(cnt==80){
                    lazer[i].flag=0;
                }
            }
        }
    }
}