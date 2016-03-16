#include "../../../include/DxLib.h"
#include "math.h"
#include "Key_Mouse.h"

#define PI2 (3.141562f*2)    //円周率*2

#define PMAX 1000    //登録する弾の最大数

#define WINDOW_SIZE_X 640
#define WINDOW_SIZE_Y 640

//float型の座標構造体
typedef struct{
    float x,y;
}fPt_t;

//弾1つ分の情報
typedef struct{
    int Knd;//弾の種類
    int Col;//弾の色
    float Angle;//弾の角度
    float x,y;
}Bl_t;

//弾全体の情報
typedef struct{
    int Num;//登録した個数
    Bl_t Bl[ PMAX ];    //登録する弾情報
}BlPoint_t;
BlPoint_t BlPoint;

//操作設定系情報
typedef struct{
    int State;
    int Knd;
    int Col;
    int Space;
    float Angle;
    int flag;
    fPt_t fPt1;
    fPt_t fPt2;
}Operate_t;

int Key[256];//キー
int Red,White,Blue;//色
int ImgBullet[14][10],ImgBack;//弾の画像と背景画像
Mouse_t Mouse;//マウス
Operate_t Operate;//操作設定

//初期化
void ini(){
    Operate.Knd=7;//弾の初期種類を7に
    Operate.Space=20;//スペースは最初20
    Operate.flag=1;
}

//ロード
void load(){
    White=GetColor(255,255,255);
    Red = GetColor(255,0,0);
    Blue = GetColor(0,255,255);
    ImgBack = LoadGraph("mydat/img/ryu.png");
    LoadDivGraph( "../dat/img/bullet/b0.png" ,  5 ,  5 , 1 , 76 ,  76 , ImgBullet[0] ) ;
    LoadDivGraph( "../dat/img/bullet/b1.png" ,  6 ,  6 , 1 , 22 ,  22 , ImgBullet[1] ) ;
    LoadDivGraph( "../dat/img/bullet/b2.png" , 10 , 10 , 1 ,  5 , 120 , ImgBullet[2] ) ;
    LoadDivGraph( "../dat/img/bullet/b3.png" ,  5 ,  5 , 1 , 19 ,  34 , ImgBullet[3] ) ;
    LoadDivGraph( "../dat/img/bullet/b4.png" , 10 , 10 , 1 , 38 ,  38 , ImgBullet[4] ) ;
    LoadDivGraph( "../dat/img/bullet/b5.png" ,  3 ,  3 , 1 , 14 ,  16 , ImgBullet[5] ) ;
    LoadDivGraph( "../dat/img/bullet/b6.png" ,  3 ,  3 , 1 , 14 ,  18 , ImgBullet[6] ) ;
    LoadDivGraph( "../dat/img/bullet/b7.png" , 10 , 10 , 1 , 16 ,  16 , ImgBullet[7] ) ;
    LoadDivGraph( "../dat/img/bullet/b8.png" , 10 , 10 , 1 , 12 ,  18 , ImgBullet[8] ) ;
    LoadDivGraph( "../dat/img/bullet/b9.png" ,  3 ,  3 , 1 , 13 ,  19 , ImgBullet[9] ) ;
    LoadDivGraph( "../dat/img/bullet/b10.png",  8 ,  8 , 1 ,  8 ,   8 , ImgBullet[10]) ;
    LoadDivGraph( "../dat/img/bullet/b11.png",  8 ,  8 , 1 , 35 ,  32 , ImgBullet[11]) ;
    LoadDivGraph( "../dat/img/bullet/b12.png", 10 , 10 , 1 , 12 ,  12 , ImgBullet[12]) ;
    LoadDivGraph( "../dat/img/bullet/b13.png", 10 , 10 , 1 , 22 ,  22 , ImgBullet[13]) ;
}

//弾を登録
void InputBlData(float x, float y, int Knd, int Col, float Angle){
    BlPoint.Bl[BlPoint.Num].x = x;//今の場所を記録
    BlPoint.Bl[BlPoint.Num].y = y;
    BlPoint.Bl[BlPoint.Num].Knd = Knd;//弾の種類
    BlPoint.Bl[BlPoint.Num].Col = Col;//色
    BlPoint.Bl[BlPoint.Num].Angle = Angle;//角度
    BlPoint.Num++;//今何個目かをカウントアップ
}

//弾をおいていくときの計算
void CalcBullet(){
    float x = Operate.fPt1.x, y = Operate.fPt1.y;//最初の場所
    //最初クリックした場所と最後クリックした場所との角度
    float Angle = atan2( Operate.fPt2.y - Operate.fPt1.y, Operate.fPt2.x - Operate.fPt1.x );
    float xlen = Operate.fPt2.x - Operate.fPt1.x;//xの距離
    float ylen = Operate.fPt2.y - Operate.fPt1.y;//yの距離
    float Length = sqrt( xlen * xlen + ylen * ylen );//点と点との距離
    float Proceeded = 0;//今現在進んだ距離

    //今現在進んだ距離が進むべき距離以内の間、かつ登録出来る個数の間ループ
    while( BlPoint.Num < PMAX ){
        InputBlData(x,y,Operate.Knd,Operate.Col,Operate.Angle);
        x += cos( Angle ) * Operate.Space;//進むべき方向へ進む
        y += sin( Angle ) * Operate.Space;
        Proceeded += Operate.Space;//進んだ距離を加算
        if(Length < Proceeded) break;
    }
}

//マウス部の計算
void CalcMouse(){
    if( Mouse.Button[0]==1 ){//左クリックされたら
        switch( Operate.State ){
            case 0://1回目押した時
                //その時の位置を記録
                Operate.fPt1.x = (float)Mouse.x;
                Operate.fPt1.y = (float)Mouse.y;
                Operate.State = 1;
                break;
            case 1://2回目押した時
                //その時の位置を記録
                Operate.fPt2.x = (float)Mouse.x;
                Operate.fPt2.y = (float)Mouse.y;
                Operate.State = 0;
                //1回目押したところから2回目おしたところまでに弾を登録
                CalcBullet();
                break;
        }
    }
}

void CalcOperate(){
    //各弾の色数
    int Col[14]={5,6,10,5,10,3,3,10,10,3,8,8,10,10};
    //左コントロールキーが押されている時
    if( Key[KEY_INPUT_LCONTROL]>0 ){
        //Zキーが押されたら
        if( Key[KEY_INPUT_Z]==1 || Key[KEY_INPUT_Z]>30){
            //操作を一つ前に戻す
            if( BlPoint.Num>0 ){
                BlPoint.Num--;
            }
        }
        //Yキーが押されたら
        if( Key[KEY_INPUT_Y]==1 || Key[KEY_INPUT_Y]>30 ){
            //以前セットしたところまでY押した数だけ戻す
            if( BlPoint.Num<PMAX-1 && !(BlPoint.Bl[BlPoint.Num].x==0 && BlPoint.Bl[BlPoint.Num].y==0) ){
                BlPoint.Num++;
            }
        }
    }
    //Sキーが押されているとき
    if( Key[KEY_INPUT_S]>0 ){
        //左キー押されたら
        if( Key[KEY_INPUT_LEFT]==1 || Key[KEY_INPUT_LEFT]>30){
            //スペース減らす
            if( Operate.Space>2 ){
                Operate.Space--;
            }
        }
        //右キー
        if( Key[KEY_INPUT_RIGHT]==1 || Key[KEY_INPUT_RIGHT]>30){
            //スペース増やす
            if( Operate.Space<300 ){
                Operate.Space++;
            }
        }
    }
    else{
        if( Key[KEY_INPUT_LEFT]>0 )//左キーで
            Operate.Angle-=PI2/360;//左回り
        if( Key[KEY_INPUT_RIGHT]>0 )//右キーで
            Operate.Angle+=PI2/360;//右回り
    }
    if( Key[KEY_INPUT_C]==1 )//色
        Operate.Col = (++Operate.Col)%Col[Operate.Knd];
    if( Key[KEY_INPUT_SPACE]==1 )//表示フラグ
        Operate.flag*=-1;
    if( Key[KEY_INPUT_K]==1 ){//種類
        Operate.Knd = (++Operate.Knd)%14;
        Operate.Col = 0;
    }
}

//データを見せる
void Show(){
    int i;
    //背景を描画
    DrawGraph(0,0,ImgBack,FALSE);
    //弾を描画
    for(i=0; i<BlPoint.Num; i++){
        DrawRotaGraphF( BlPoint.Bl[ i ].x, BlPoint.Bl[ i ].y,1.0,BlPoint.Bl[i].Angle+PI2/4, 
            ImgBullet[BlPoint.Bl[i].Knd][BlPoint.Bl[i].Col], TRUE );
    }
    //ピーと引っ張る線を描画
    if( Operate.State==1 ){
        DrawLine( (int)Operate.fPt1.x, (int)Operate.fPt1.y, Mouse.x, Mouse.y, Red );
    }
    //マウスポインタ部に弾を表示する
    DrawRotaGraph( Mouse.x, Mouse.y, 1.0, Operate.Angle, ImgBullet[Operate.Knd][Operate.Col], TRUE );
    //弾の上に、今設定されているスペースがどれ位か表示する
    DrawLine(Mouse.x,Mouse.y,Mouse.x+Operate.Space,Mouse.y,Blue);
    //表示フラグがオンなら現在の操作設定内容を表示
    if(Operate.flag==1){
        SetDrawBlendMode( DX_BLENDMODE_ALPHA  , 128 ) ;
        DrawBox(0,0,230,120,0,TRUE);
        SetDrawBlendMode( DX_BLENDMODE_NOBLEND,   0 ) ;
        DrawFormatString(0,  0,White,"座標[%3d,%3d]",Mouse.x,Mouse.y);
        DrawFormatString(0, 20,White,"種類     [%2d] : Kキー",Operate.Knd);
        DrawFormatString(0, 40,White,"色　     [%2d] : Cキー",Operate.Col);
        DrawFormatString(0, 60,White,"角度[%5.1f°] : ←→キー",Operate.Angle/PI2*360.0f);
        DrawFormatString(0, 80,White,"空白    [%3d] : S+←→キー",Operate.Space);
        DrawFormatString(0,100,White,"スペースキーで非表示");
    }
}

//データを出力
void Output(){
	int i;
    FILE *fp;
    fp = fopen( "Output.dat" , "wb" );

	for(i=0; i<BlPoint.Num; i++){//座標データを-1～1に変換する
		BlPoint.Bl[i].x -= WINDOW_SIZE_X/2;
		BlPoint.Bl[i].x /= WINDOW_SIZE_X/2;
		BlPoint.Bl[i].y -= WINDOW_SIZE_Y/2;
		BlPoint.Bl[i].y /= WINDOW_SIZE_Y/2;
    }

	if( fp == NULL )
        return;
    fwrite( &BlPoint, sizeof(BlPoint_t), 1, fp );
    fclose(fp);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
    ChangeWindowMode(TRUE);//ウィンドウモード
    SetGraphMode(WINDOW_SIZE_X,WINDOW_SIZE_Y,32);//画面サイズ変更
    SetWindowSizeChangeEnableFlag(TRUE);//画面の大きさ調整可能
    if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1;//初期化と裏画面化
    SetMouseDispFlag( TRUE ) ;//マウス表示オン
    ini();//初期化
    load();//ロード
    while(ProcessMessage()==0 && ClearDrawScreen()==0 && GetHitKeyStateAll_2(Key)==0 && Key[KEY_INPUT_ESCAPE]==0){
          //↑ﾒｯｾｰｼﾞ処理       　  ↑画面をｸﾘｱ    　　　    ↑入力状態を保存　　　　　　↑ESCが押されていない
        GetHitMouseStateAll_2(&Mouse);
        CalcMouse();//マウス部の計算
        CalcOperate();//操作系の計算
        Show();//見せる
        ScreenFlip();
    }
    Output();//データを出力
    DxLib_End();
    return 0;
}