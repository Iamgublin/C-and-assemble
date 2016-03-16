#include "../../../include/DxLib.h"
#include "Key_Mouse.h"

/* ビットマップ画像の横サイズ */
#define BMP_YOKO 400
/* ビットマップ画像の縦サイズ */
#define BMP_TATE 400

/* ビットマップのヘッダサイズ */
#define HEAD 54
/* トータルサイズ */
#define TOTAL (BMP_YOKO*BMP_TATE*3+HEAD)

/* AAデータを作成する合計枚数 */
#define BULLET_MAX 4000

/* ビットマップの色情報を格納するための構造体 */
typedef struct{
        unsigned char col[3];
}img_t;

int Key[256];

//弾の座標(55)
typedef struct{
    float x,y;
}Pt_t;
//置いた弾の構造体
typedef struct{
    int num;//現在置いた弾数
    Pt_t Pt[ BULLET_MAX ];
}Bl_t;

typedef struct{
    int state; //状態
    double len; //間隔
}Operate_t;

Bl_t Bl;
Operate_t Operate;

/* TOTAL分データを用意 */
unsigned char data[TOTAL];
/* イメージ画像のピクセル数だけ色格納用構造体を用意 */
img_t img[BMP_TATE][BMP_YOKO];
/* 2値化した情報を入れるための配列を用意 */
BYTE Pixel[BMP_TATE][BMP_YOKO];

int img_bullet[4] ; //(55)
int img_back;

/* ビットマップを読み込みdataに格納する関数 */
int ReadBmp(){
    char name[256]="../AA画像/aisha.bmp";
    FILE *fp;
    fp = fopen( name , "rb" );
    if( fp == NULL ){
        printfDx( "%sが見つかりません。",name);
        return -1;
    }
    fread( data, TOTAL, 1, fp );
    fclose(fp);
    return 0;
}

/* ビットマップの生データを各ピクセルの色格納用構造体に入れ直す */
void ConvData(){
    int x,y,c,t;
    t=HEAD;
    for(y=BMP_TATE-1;y>=0;y--){
        for(x=0;x<BMP_YOKO;x++){
            for(c=0;c<3;c++){
                img[y][x].col[c]=data[t];
                t++;
            }
        }
    }
}

/* 2値化 */
void Binarization(){
    int x,y,c;
    int sum;
    for(y=0;y<BMP_TATE;y++){
        for(x=0;x<BMP_YOKO;x++){
            sum = 0;
            //色の輝度平均を計算
            for(c=0;c<3;c++){
                sum += img[y][x].col[c];
            }
            sum /= 3;
            //0～255の輝度の平均が128以上なら(明るいなら)0、未満なら(暗いなら)1
            if( sum >= 128){
                Pixel[y][x] = 0;
            } else {
                Pixel[y][x] = 1;
            }
        }
    }
}

/* 弾置き計算 */
void CalcPut(){
    int i,x,y;
    double lx,ly;
    //現在登録されている弾を0個に
    Bl.num = 0;
    //ビットマップ画像サイズ分ループ
    for(y=0;y<BMP_TATE;y++){
        for(x=0;x<BMP_YOKO;x++){
            //もう登録できないなら抜ける
            if( Bl.num >= BULLET_MAX-1 ){
                break;
            }
            //その場が色つきなら
            if( Pixel[y][x] == 1){
                //今登録されている弾分ループ
                for(i=0; i<Bl.num; i++){
                    //その全ての弾と現在の場所との距離がOperate.len以上なら抜ける
                    lx = x - Bl.Pt[i].x;
                    ly = y - Bl.Pt[i].y;
                    if( lx*lx + ly*ly < Operate.len*Operate.len){
                        break;
                    }
                }
                //途中で抜けなかった=一つも近くに弾が無かったなら
                if( i == Bl.num ){
                    //登録
                    Bl.Pt[ Bl.num ].x = (float)x;
                    Bl.Pt[ Bl.num ].y = (float)y;
                    Bl.num++;
                }
            }
        }
    }
}

/* 操作計算 */
void CalcOperate(){
    //スペースが押されるたびに状態変化
    if( Key[KEY_INPUT_SPACE] == 1 ){
        Operate.state = ( Operate.state+1 )%3;
    }
    //左が押されると.lenを少なく
    if( Key[KEY_INPUT_LEFT] == 1 || Key[KEY_INPUT_LEFT] > 20 ){
        if( Operate.len > 1 ){
            Operate.len -= 0.2;
        }
        CalcPut();
    }
    //右が押されると.lenを多く
    if( Key[KEY_INPUT_RIGHT] == 1 || Key[KEY_INPUT_RIGHT] > 20 ){
        if( Operate.len < 50 ){
            Operate.len += 0.2;
        }
        CalcPut();
    }
}

/* 描画 */
void Graph(){
    int i;
    //状態によって描画パターンを変える
    if( Operate.state == 1 ||  Operate.state == 2 ){
        DrawGraph(0,0,img_back,FALSE);
    }
    if( Operate.state == 0 ||  Operate.state == 1 ){
        for(i=0; i<Bl.num; i++){
            DrawRotaGraph((int)Bl.Pt[i].x,(int)Bl.Pt[i].y,1.0,0.0,img_bullet[1],TRUE);
        }
    }
    SetDrawBlendMode( DX_BLENDMODE_ALPHA , 64 ) ;
    DrawBox(0,0,100,40,0,TRUE);
    SetDrawBlendMode( DX_BLENDMODE_NOBLEND , 0 ) ;
    DrawFormatString(0,0,GetColor(255,255,255),"間隔=%.1f",Operate.len);
    DrawFormatString(0,20,GetColor(255,255,255),"弾数=%d",Bl.num);
}
 
//データを出力
void Output(){
    int i;
    FILE *fp;
    fp = fopen( "Output.dat" , "wb" );

    for(i=0; i<Bl.num; i++){//座標データを-1～1に変換する
        Bl.Pt[i].x -= BMP_YOKO/2;
        Bl.Pt[i].y -= BMP_TATE/2;
        Bl.Pt[i].x /= BMP_YOKO/2;
        Bl.Pt[i].y /= BMP_TATE/2;
    }
    if( fp == NULL ){
        return;
    }
    fwrite( &Bl, sizeof(Bl_t), 1, fp );
    fclose(fp);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){

    SetGraphMode(BMP_YOKO,BMP_TATE,16);//400x400に
    SetWindowSizeChangeEnableFlag(TRUE);//画面の大きさを変更できる
    ChangeWindowMode(TRUE);//ウィンドウモード
    if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1;//初期化と裏画面化
 
    Operate.len = 5;//弾の間隔を5にセット
    img_back = LoadGraph( "../AA画像/aisha.bmp" );
    LoadDivGraph( "../dat/img/bullet/b14.png"   , 4 , 4 , 1 , 6 , 6 , img_bullet ) ;//弾読み込み(55)

    ReadBmp();//ビットマップ読み込み
    ConvData();//画像の色情報構造体に格納
    Binarization();//2値化
    CalcPut();//弾を置く計算

    while(ProcessMessage()==0 && ClearDrawScreen()==0 && GetHitKeyStateAll_2(Key)==0 && Key[KEY_INPUT_ESCAPE]==0){
          //↑ﾒｯｾｰｼﾞ処理       　  ↑画面をｸﾘｱ    　　　    ↑入力状態を保存　　　　　　　↑ESCが押されていない
        CalcOperate();//操作計算
        Graph();//描画
        ScreenFlip();
    }
 
    Output();//ファイル出力
    DxLib_End();
    return 0;
}