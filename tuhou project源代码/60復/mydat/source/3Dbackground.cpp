#include "../include/GV.h"

//少なくしすぎるとだめ。-1で使用している。
#define OBCHILD_MAX 11
#define OBJECT_NUM_MAX 10

//三角形のポリゴン２つで四角形を描画する為の値。数値固定なので、覚える必要なし
typedef struct{
    float x,y;
    float u,v;
}VtPm_t;
VtPm_t VtPm[6]={{-1,1,0,0},{1,1,1,0},{-1,-1,0,1},{1,-1,1,1},{-1,-1,0,1},{1,1,1,0}};

//一つのテクスチャーについての構造体
typedef struct{
    float x,y,z;//中心点
    VERTEX_3D Vertex[6] ;        //描画用頂点6個
} ObChild_t;

//沢山のテクスチャーが集まった一つの情報。
//例えば左にダーっと連続で表示される壁は沢山ののObChild_tをここで管理している。
//Obchild_tの集合
typedef struct{
    int Type;    // 0:画面に平行、1:画面に垂直
    int Img;    //画像
    int ImgSize;
    int ImgX1,ImgX2,ImgY1,ImgY2;
    float LargeX,LargeY;//縦横の大きさ(Typeが1の時はLargeXがLargeZの役割をする)
    float Zhaba;
    float FromZ,ToZ;    //どこからどこまで奥行きを設定するか
    float FadeFromZ,FadeToZ;    //どこからどこまでフェードを設定するか(消える瞬間フェードアウト、現れる瞬間フェードインする)
    int ObchindMax;
    ObChild_t ObChild[ OBCHILD_MAX ];
} Object_t;

int ObjectNum;
Object_t Object[OBJECT_NUM_MAX];
/*
int ImgHandle   : 画像ハンドル
int ImgSize     : 画像サイズ
int ImgX1       : 画像の使用する部分左上座標
int ImgY1       : 画像の使用する部分左上座標
int ImgX2       : 画像の使用する部分右下座標
int ImgY2       : 画像の使用する部分右下座標
float LargeX    : 描画する大きさ（横）
float LargeY    : 描画する大きさ（縦）
int Type        : 描画タイプ　0:画面と同じ向き　1:画面に垂直（壁）　2:画面に垂直（地面）
float FromZ     : 描画を始める奥行き
float FadeFromZ : フェードインを始める奥行き
float FadeToZ   : フェードアウトを始める奥行き
float ToZ       : 描画を終わる奥行き
float GraphX    : 描画する中心点
float GraphY    : 描画する中心点
int ObchildMax  : typeが0の場合のみ、同時にいくつ表示するか
*/
void IniObj(Object_t *Ob, int ImgHandle, int ImgSize, int ImgX1, int ImgY1, int ImgX2, int ImgY2, float LargeX, float LargeY,
              int Type, float FromZ, float FadeFromZ, float FadeToZ, float ToZ, float GraphX, float GraphY, int ObchildMax){
    int i,s;

    if( ObjectNum >= OBJECT_NUM_MAX-1 ){
        printfDx("オブジェクト登録オーバー\n");
        return ;
    }
    ObjectNum++;//オブジェクトの登録数加算

    Ob->Img = ImgHandle;//画像ハンドル
    Ob->ImgSize = ImgSize;//画像サイズ
    Ob->ImgX1 = ImgX1;
    Ob->ImgY1 = ImgY1;
    Ob->ImgX2 = ImgX2;
    Ob->ImgY2 = ImgY2;
    Ob->LargeX = LargeX;//とりあえず描画する大きさを適当に設定。縦・横比は素材の通りにする
    Ob->LargeY = LargeY;
    Ob->Type = Type;//タイプを垂直に
    Ob->FromZ     =  FromZ;//描画開始地点
    Ob->FadeFromZ =  FadeFromZ;//描画フェードイン開始地点
    Ob->FadeToZ   =  FadeToZ;//描画フェードアウト開始地点
    Ob->ToZ       =  ToZ;//描画終了地点
    Ob->ObchindMax = OBCHILD_MAX;
    if( Ob->Type == 0 ){
        Ob->ObchindMax = ObchildMax;
    }
    if( Ob->ObchindMax - 1 <= 0 ){
        printfDx("表示数の設定が異常です\n");
        return ;
    }
    //Zの幅計算
    Ob->Zhaba = (Ob->FromZ - Ob->ToZ) / (Ob->ObchindMax-1);

    float ou1 = (float)Ob->ImgX1 / Ob->ImgSize, ou2 = (float)(Ob->ImgX2 - Ob->ImgX1) / Ob->ImgSize;
    float ov1 = (float)Ob->ImgY1 / Ob->ImgSize, ov2 = (float)(Ob->ImgY2 - Ob->ImgY1) / Ob->ImgSize;
    for(s=0; s<Ob->ObchindMax; s++){
        Ob->ObChild[s].x = GraphX;
        Ob->ObChild[s].y = GraphY;
        Ob->ObChild[s].z = Ob->ToZ - Ob->Zhaba + Ob->Zhaba * s;;
        for(i=0; i<6; i++){
            Ob->ObChild[s].Vertex[i].r = Ob->ObChild[s].Vertex[i].g = Ob->ObChild[s].Vertex[i].b = Ob->ObChild[s].Vertex[i].a = 255;
            Ob->ObChild[s].Vertex[i].u = ou1 + ou2 * VtPm[i].u;
            Ob->ObChild[s].Vertex[i].v = ov1 + ov2 * VtPm[i].v;
        }
    }
}

void Ini3Dbackground(){
    int ImgHandle;
    ObjectNum = 0;
    ImgHandle = LoadGraph( "../dat/img/back/3D/tex.png" );
    IniObj(&Object[0], ImgHandle, 512,  0,  0, 256, 128, 250, 50, 2, 1000, 400, -200, -400, 320, 240-90, OBCHILD_MAX);
    IniObj(&Object[1], ImgHandle, 512, 60,270, 405, 512, 180,125, 0, 1000, 400, -200, -400, 470, 275, 6);
    ImgHandle = LoadGraph( "../dat/img/back/3D/kabe.png" );
    IniObj(&Object[2], ImgHandle, 512,  0,  0, 390, 512,  73, 90, 1, 1000, 400, -200, -400, 170, 240, OBCHILD_MAX);
}

void ClacObject(){
    int t,s,i;
    for(t=0; t<ObjectNum; t++){
        for(s=0; s<Object[t].ObchindMax; s++){
            Object[t].ObChild[s].z-=3;
            for(i=0;i<6;i++){
                switch(Object[t].Type){
                    case 0://画面に平行
                        Object[t].ObChild[s].Vertex[i].pos.x = Object[t].ObChild[s].x + Object[t].LargeX * VtPm[i].x ;    
                        Object[t].ObChild[s].Vertex[i].pos.y = Object[t].ObChild[s].y + Object[t].LargeY * VtPm[i].y ;
                        Object[t].ObChild[s].Vertex[i].pos.z = Object[t].ObChild[s].z ;
                        break;
                    case 1://画面に垂直(壁)
                        Object[t].ObChild[s].Vertex[i].pos.x = Object[t].ObChild[s].x;    
                        Object[t].ObChild[s].Vertex[i].pos.y = Object[t].ObChild[s].y + Object[t].LargeY * VtPm[i].y ;
                        Object[t].ObChild[s].Vertex[i].pos.z = Object[t].ObChild[s].z + Object[t].Zhaba/2* VtPm[i].x ;
                        break;
                    case 2://画面に垂直(床)
                        Object[t].ObChild[s].Vertex[i].pos.x = Object[t].ObChild[s].x + Object[t].LargeX * VtPm[i].x;    
                        Object[t].ObChild[s].Vertex[i].pos.y = Object[t].ObChild[s].y ;
                        Object[t].ObChild[s].Vertex[i].pos.z = Object[t].ObChild[s].z + Object[t].Zhaba/2* VtPm[i].y;
                        break;
                }
            }
        }

        if( Object[t].FromZ - Object[t].FadeFromZ <= 0 ){
            printfDx("Object[%d].Fromの設定がおかしい\n",t);
        }
        else if( Object[t].FadeToZ - Object[t].ToZ <= 0 ){
            printfDx("Object[%d].Toの設定がおかしい\n",t);
        }
        else{
            for(s=0; s<Object[t].ObchindMax; s++){
                //近づいて見えなくなったら
                if(Object[t].ObChild[s].z < Object[t].ToZ - Object[t].Zhaba*0.5f){
                    //一番向こう側へ
                    float sub = (Object[t].ToZ - Object[t].Zhaba*0.5f)- Object[t].ObChild[s].z;
                    Object[t].ObChild[s].z = Object[t].FromZ + Object[t].Zhaba*0.5f - sub;
                }
                //遠ざかって見えなくなったら
                else if(Object[t].ObChild[s].z > Object[t].FromZ + Object[t].Zhaba*0.5f){
                    //一番こちら側へ
                    float sub = Object[t].ObChild[s].z - (Object[t].FromZ + Object[t].Zhaba*0.5f);
                    Object[t].ObChild[s].z = Object[t].ToZ - Object[t].Zhaba*0.5f + sub;
                }
                //透過度設定
                for(i=0; i<6; i++){
                    float z = Object[t].ObChild[s].Vertex[i].pos.z;
                    //位置が描画する範囲より近かったら透過0
                    if     (z <= Object[t].ToZ){
                        Object[t].ObChild[s].Vertex[i].a = 0;
                    }
                    //(近づいている場合)フェードアウトする位置だったら
                    else if(Object[t].ToZ < z && z < Object[t].FadeToZ){
                        Object[t].ObChild[s].Vertex[i].a = (unsigned char)(255.0f / (Object[t].FadeToZ - Object[t].ToZ) * (z - Object[t].ToZ)) ;
                    }
                    //通常描画する位置なら
                    else if(Object[t].FadeToZ <= z && z <= Object[t].FadeFromZ){
                        Object[t].ObChild[s].Vertex[i].a = 255;
                    }
                    //(近づいてる場合)フェードインする位置だったら
                    else if(Object[t].FadeFromZ < z && z < Object[t].FromZ){
                        Object[t].ObChild[s].Vertex[i].a = (unsigned char)(255.0f / (Object[t].FromZ - Object[t].FadeFromZ) * (Object[t].FromZ - z)) ; 
                    }
                    //描画する範囲より遠かったら透過0
                    else if(Object[t].FromZ <= z){
                        Object[t].ObChild[s].Vertex[i].a = 0;
                    }
                }
            }
        }
    }
}

void SwapObChild(ObChild_t *Ob1,ObChild_t *Ob2){
    ObChild_t t = *Ob1;
    *Ob1 = *Ob2;
    *Ob2 = t;
}

//Zでテクスチャをソート
void SortObject(){
    int i,j,t;
    for(t=0; t<ObjectNum; t++){
        for (i = 0; i < Object[t].ObchindMax ; i++) {
            for (j = i + 1; j < Object[t].ObchindMax ; j++) {
                if ( Object[t].ObChild[i].z < Object[t].ObChild[j].z ) {
                    SwapObChild( &Object[t].ObChild[i],  &Object[t].ObChild[j] );
                }
            }
        }
    }
}

void Main3Dbackground(){//(60)
    int t,s;
    ClacObject();
    SortObject();
    SetDrawMode( DX_DRAWMODE_BILINEAR ) ;//ポリゴンが荒く見えないような描画の仕方「バイリニア法」
	SetDrawArea(FX,FY,FX+FMX,FY+FMY);//描画可能エリアを設定 
    for(t=0; t<ObjectNum; t++){
        for(s=0; s<Object[t].ObchindMax; s++){
            DrawPolygon3D( Object[t].ObChild[s].Vertex, 2, Object[t].Img, TRUE ) ;
        }
    }
    SetDrawArea(0,0,640,480);//描画可能エリアを戻す
    SetDrawMode(DX_DRAWMODE_NEAREST);//描画の仕方を元に戻す
}
