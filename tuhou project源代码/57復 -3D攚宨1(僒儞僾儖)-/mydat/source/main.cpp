#include "../../../include/DxLib.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
             LPSTR lpCmdLine, int nCmdShow )
{
    int GrHandle ;
    float Z, ZAdd ; 
    VERTEX_3D Vertex[6] ;    // ポリゴン２枚なので、頂点は６個
    
    ChangeWindowMode(TRUE);

    // ＤＸライブラリ初期化処理
    if( DxLib_Init() == -1 )    
         return -1 ;    // エラーが起きたら直ちに終了

    // 描画先を裏画面にする
    SetDrawScreen( DX_SCREEN_BACK ) ;

    // テクスチャの読み込み
    GrHandle = LoadGraph( "mydat/img/kabe.png" ) ;

    // Ｚ値の初期化
    Z = 0.0F ;

    // Ｚ値の加算値を初期化(最初は近づける)
    ZAdd = -1.0F ;

    // 何かキーが押されるまでループ
    while( CheckHitKeyAll() == 0 )
    {
        // メッセージ処理
        if( ProcessMessage() != 0 ) break ;

        // 画面の初期化
        ClearDrawScreen() ;

        // Ｚ値の処理
        Z += ZAdd ;

        // 一定のラインを越えていたら進行方向を反転する
        if( Z < -300.0F || Z > 300.0F ) ZAdd = -ZAdd ;

        // 頂点情報のセット
        {
            // 画面の中央に幅・高さ100で描画
            Vertex[0].pos.x = 320.0F - 50.0F ;    Vertex[0].pos.y = 240.0F + 50.0F ;    Vertex[0].pos.z = Z ;
            Vertex[0].u = 0.0F ;
            Vertex[0].v = 0.0F ;

            Vertex[1].pos.x = 320.0F + 50.0F ;    Vertex[1].pos.y = 240.0F + 50.0F ;    Vertex[1].pos.z = Z ;
            Vertex[1].u = 1.0F ;
            Vertex[1].v = 0.0F ;
    
            Vertex[2].pos.x = 320.0F - 50.0F ;    Vertex[2].pos.y = 240.0F - 50.0F ;    Vertex[2].pos.z = Z ;
            Vertex[2].u = 0.0F ;
            Vertex[2].v = 1.0F ;

            Vertex[3].pos.x = 320.0F + 50.0F ;    Vertex[3].pos.y = 240.0F - 50.0F ;    Vertex[3].pos.z = Z ;
            Vertex[3].u = 1.0F ;
            Vertex[3].v = 1.0F ;

            Vertex[4].pos.x = 320.0F - 50.0F ;    Vertex[4].pos.y = 240.0F - 50.0F ;    Vertex[4].pos.z = Z ;
            Vertex[4].u = 0.0F ;
            Vertex[4].v = 1.0F ;

            Vertex[5].pos.x = 320.0F + 50.0F ;    Vertex[5].pos.y = 240.0F + 50.0F ;    Vertex[5].pos.z = Z ;
            Vertex[5].u = 1.0F ;
            Vertex[5].v = 0.0F ;

            // 輝度は全要素100%
            Vertex[0].r = Vertex[0].g = Vertex[0].b = 255 ;
            Vertex[1].r = Vertex[1].g = Vertex[1].b = 255 ;
            Vertex[2].r = Vertex[2].g = Vertex[2].b = 255 ;
            Vertex[3].r = Vertex[3].g = Vertex[3].b = 255 ;
            Vertex[4].r = Vertex[4].g = Vertex[4].b = 255 ;
            Vertex[5].r = Vertex[5].g = Vertex[5].b = 255 ;

            // a も最大値
            Vertex[0].a = 255 ;
            Vertex[1].a = 255 ;
            Vertex[2].a = 255 ;
            Vertex[3].a = 255 ;
            Vertex[4].a = 255 ;
            Vertex[5].a = 255 ;
        }

        // ポリゴンを透過色無しで２枚描画
        DrawPolygon3D( Vertex, 2, GrHandle, TRUE ) ;

        // 裏画面の内容を表画面に反映
        ScreenFlip() ;
    }

    // ＤＸライブラリ使用の終了処理
    DxLib_End() ;

    // ソフトの終了
    return 0 ;
}