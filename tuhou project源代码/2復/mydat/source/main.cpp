#define GLOBAL_INSTANCE 
#include "../include/GV.h"

int Key[256];

int GetHitKeyStateAll_2(int GetHitKeyStateAll_InputKey[]){
    char GetHitKeyStateAll_Key[256];
    GetHitKeyStateAll( GetHitKeyStateAll_Key );
    for(int i=0;i<256;i++){
        if(GetHitKeyStateAll_Key[i]==1) GetHitKeyStateAll_InputKey[i]++;
        else                            GetHitKeyStateAll_InputKey[i]=0;
    }
    return 0;
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow ){
    ChangeWindowMode(TRUE);//ウィンドウモード
    if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1;//初期化と裏画面化

    while(ProcessMessage()==0 && ClearDrawScreen()==0 && GetHitKeyStateAll_2(Key)==0 && Key[KEY_INPUT_ESCAPE]==0){
          //↑ﾒｯｾｰｼﾞ処理       　  ↑画面をｸﾘ?   　　　    ↑入力状態を保存　　　　　　　↑ESCが押されていない


        //ココ！！



        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
