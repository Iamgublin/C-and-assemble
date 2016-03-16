#include "../../../include/DxLib.h"

//三角形のポリゴン２つで四角形を描画する為の値。数値固定なので、覚える必要なし
typedef struct{
    float x,y;
    float u,v;
}VtPm_t;
VtPm_t VtPm[6]={{-1,1,0,0},{1,1,1,0},{-1,-1,0,1},{1,-1,1,1},{-1,-1,0,1},{1,1,1,0}};

//一つのテクスチャーについての構造体
typedef struct{
    int Type;    // 0:画面に平行、1:画面に垂直
    int Img;    //画像
    float x,y,z;//中心点
    float LargeX,LargeY;//縦横の大きさ(Typeが1の時はLargeXがLargeZの役割をする)
    float u,v;            //使用する画像のどの部分を使うか
    float FromZ,ToZ;    //どこからどこまで奥行きを設定するか
    float FadeFromZ,FadeToZ;    //どこからどこまでフェードを設定するか(消える瞬間フェードアウト、現れる瞬間フェードインする)
    VERTEX_3D Vertex[6] ;        //描画用頂点6個
}Object_t;

Object_t Object;

void ini (){
    int i;
    Object.Img = LoadGraph( "mydat/img/kabe.png" );
    Object.LargeX = 48.0f;//とりあえず描画する大きさを適当に設定。縦・横比は素材の通りにする
    Object.LargeY = 60.0f;
    Object.Type = 1;//タイプを垂直に
    Object.x = 220.0f;//とりあえず描画する中心位置を中心よりちと左に
    Object.y = 240.0f;
    Object.z = 0.0f;
    Object.u = 0.763671875f;//画像のどの部分を使うか
    Object.v = 1.0f;
    Object.FromZ     =  200;//描画開始地点
    Object.FadeFromZ =  100;//描画フェードイン開始地点
    Object.FadeToZ   = -100;//描画フェードアウト開始地点
    Object.ToZ       = -200;//描画終了地点

    for(i=0; i<6; i++){
        Object.Vertex[i].r = 255;
        Object.Vertex[i].b = 255;
        Object.Vertex[i].g = 255;
        Object.Vertex[i].a = 255;
        Object.Vertex[i].u = Object.u * VtPm[i].u;
        Object.Vertex[i].v = Object.v * VtPm[i].v;
    }
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
             LPSTR lpCmdLine, int nCmdShow )
{
    int i;
    float z=0;
    ChangeWindowMode(TRUE);//ウィンドウモード
    if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1;//初期化と裏画面化

    ini();

    // 何かキーが押されるまでループ
    while(ProcessMessage()==0 && ClearDrawScreen()==0 && CheckHitKey(KEY_INPUT_ESCAPE)==0){

        Object.z=z;

        if(CheckHitKey(KEY_INPUT_Z)>0){
            z+=1.4f;
        }
        if(CheckHitKey(KEY_INPUT_Y)>0){
            z-=1.4f;
        }

        switch(Object.Type){
            case 0:
                for(i=0;i<6;i++){
                    Object.Vertex[i].pos.x = Object.x + Object.LargeX * VtPm[i].x ;    
                    Object.Vertex[i].pos.y = Object.y + Object.LargeY * VtPm[i].y ;
                    Object.Vertex[i].pos.z = Object.z ;
                }
                break;
            case 1:
                for(i=0;i<6;i++){
                    Object.Vertex[i].pos.x = Object.x;    
                    Object.Vertex[i].pos.y = Object.y + Object.LargeY * VtPm[i].y ;
                    Object.Vertex[i].pos.z = Object.z + Object.LargeX * VtPm[i].x ;
                }
                break;
        }
/*
z
Object.FromZ        200
z
Object.FadeFromZ    100
z
Object.FadeToZ        -100
z
Object.ToZ            -200
z
*/
        if( Object.FromZ - Object.FadeFromZ <= 0 ){
            printfDx(".Fromの設定がおかしい\n");
        }
        else if( Object.FadeToZ - Object.ToZ <= 0 ){
            printfDx(".Toの設定がおかしい\n");
        }
        else{
            for(i=0; i<6; i++){
                float z = Object.Vertex[i].pos.z;
                //位置が描画する範囲より遠かったら透過0
                if     (z < Object.ToZ){
                    Object.Vertex[i].a = 0;
                }
                //(近づいている場合)フェードインする位置だったら
                else if(Object.ToZ < z && z <=Object.FadeToZ){
                    Object.Vertex[i].a = (unsigned char)(255.0f / (Object.FadeToZ - Object.ToZ) * (z - Object.ToZ)) ;
                }
                //通常描画する位置なら
                else if(Object.FadeToZ <= z && z <= Object.FadeFromZ){
                    Object.Vertex[i].a = 255;
                }
                //(近づいてる場合)フェードアウトする位置だったら
                else if(Object.FadeFromZ <= z && z < Object.FromZ){
                    Object.Vertex[i].a = (unsigned char)(255.0f / (Object.FromZ - Object.FadeFromZ) * (Object.FromZ - z)) ; 
                }
                //描画する範囲より近かったら透過0
                else if(Object.FromZ < z){
                    Object.Vertex[i].a = 0;
                }
            }
        }

        // ポリゴンを透過色無しで２枚描画
        DrawPolygon3D( Object.Vertex, 2, Object.Img, TRUE ) ;

        DrawFormatString(0,0,GetColor(255,255,255),"%f",z);
        // 裏画面の内容を表画面に反映
        ScreenFlip() ;
    }

    // ＤＸライブラリ使用の終了処理
    DxLib_End() ;

    // ソフトの終了
    return 0 ;
}

