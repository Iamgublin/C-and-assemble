#include "../../../include/DxLib.h"
#include "math.h"
#include "Key_Mouse.h"

#define PI2 (3.141562f*2)    //円周率*2

#define PMAX 1000    //登録する弾の最大数

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

//操作設定系情報
typedef struct{
    int State;//状態
    int Knd;//種類
    int Col;//色
    int Space;//スペース
    float Angle;//角度
    int flag;//フラグ
    fPt_t fPt1;//１つ目の座標
    fPt_t fPt2;//２つ目の座標
}Operate_t;

int Key[256];//キー
int Red,White,Blue;//色
int ImgBullet[14][10],ImgBack;//弾の画像と背景画像
Mouse_t Mouse;//マウス
Operate_t Operate;//操作設定
BlPoint_t BlPoint;//弾情報

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
}

//データを見せる
void Show(){
    int i;
    //背景を描画
    DrawGraph(0,0,ImgBack,FALSE);
    //弾を描画
    for(i=0; i<BlPoint.Num; i++){
        DrawCircle( (int)BlPoint.Bl[ i ].x, (int)BlPoint.Bl[ i ].y, 3, Red, TRUE );
    }
    //ピーと引っ張る線を描画
    if( Operate.State==1 ){
        DrawLine( (int)Operate.fPt1.x, (int)Operate.fPt1.y, Mouse.x, Mouse.y, Red );
    }
    //マウスポインタ部に弾を表示する
    DrawCircle( Mouse.x, Mouse.y, 3, Red, TRUE );
    //弾の上に、今設定されているスペースがどれ位か表示する
    DrawLine(Mouse.x,Mouse.y,Mouse.x+Operate.Space,Mouse.y,Blue);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
    ChangeWindowMode(TRUE);//ウィンドウモード
    SetGraphMode(640,640,32);//画面サイズ変更
    if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1;//初期化と裏画面化
    SetMouseDispFlag( TRUE ) ;//マウス表示オン
    ini();//初期化
    load();//ロード
    while(ProcessMessage()==0 && ClearDrawScreen()==0 && GetHitKeyStateAll_2(Key)==0 && Key[KEY_INPUT_ESCAPE]==0){
          //↑ﾒｯｾｰｼﾞ処理       　  ↑画面をｸﾘｱ    　　　    ↑入力状態を保存　　　　　　　↑ESCが押されていない
        GetHitMouseStateAll_2(&Mouse);
        CalcMouse();//マウス部の計算
        CalcOperate();//操作系の計算
        Show();//見せる
        ScreenFlip();
    }
    DxLib_End();
    return 0;
}