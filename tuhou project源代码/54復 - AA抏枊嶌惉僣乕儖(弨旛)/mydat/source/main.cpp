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

/* TOTAL分データを用意 */
unsigned char data[TOTAL];
/* イメージ画像のピクセル数だけ色格納用構造体を用意 */
img_t img[BMP_TATE][BMP_YOKO];
/* 2値化した情報を入れるための配列を用意 */
BYTE Pixel[BMP_TATE][BMP_YOKO];

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

/* 描画 */
void Graph(){
    int x,y;
    static int Black = GetColor(0,0,0);
    DrawBox(0,0,BMP_YOKO,BMP_TATE,GetColor(255,255,255),TRUE);
    for(y=0;y<BMP_TATE;y++){
        for(x=0;x<BMP_YOKO;x++){
            if( Pixel[y][x] == 1){
                DrawPixel(x,y,Black);
            }
        }
    }
}
 
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){

    SetGraphMode(BMP_YOKO,BMP_TATE,16);//400x400に
    SetWindowSizeChangeEnableFlag(TRUE);//画面の大きさを変更できる
    ChangeWindowMode(TRUE);//ウィンドウモード
    if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1;//初期化と裏画面化
 
    ReadBmp();//ビットマップ読み込み
    ConvData();//画像の色情報構造体に格納
    Binarization();//2値化

    while(ProcessMessage()==0 && ClearDrawScreen()==0 && GetHitKeyStateAll_2(Key)==0 && Key[KEY_INPUT_ESCAPE]==0){
          //↑ﾒｯｾｰｼﾞ処理       　  ↑画面をｸﾘｱ    　　　    ↑入力状態を保存　　　　　　　↑ESCが押されていない
        Graph();//描画
        ScreenFlip();
    }
 
    DxLib_End();
    return 0;
}