
typedef struct{
    int x;
    int y;          //座標
    unsigned int Button[8];  //ボタンの押した状態
    int WheelRotVol;//ホイールの回転量
}Mouse_t;

int GetHitKeyStateAll_2(int GetHitKeyStateAll_InputKey[]){
    char GetHitKeyStateAll_Key[256];
    GetHitKeyStateAll( GetHitKeyStateAll_Key );
    for(int i=0;i<256;i++){
        if(GetHitKeyStateAll_Key[i]==1) GetHitKeyStateAll_InputKey[i]++;
        else                            GetHitKeyStateAll_InputKey[i]=0;
    }
    return 0;
}

int GetHitMouseStateAll_2(Mouse_t *Nezumi){
    if(GetMousePoint( &Nezumi->x, &Nezumi->y ) == -1){ //マウスの位置取得
        return -1;
    }
    int MouseInput=GetMouseInput();    //マウスの押した状態取得
    for(int i=0; i<8; i++){            //マウスのキーは最大8個まで確認出来る
        if( (MouseInput & 1<<i ) != 0 ) Nezumi->Button[i]++;   //押されていたらカウントアップ
        else                            Nezumi->Button[i] = 0; //押されてなかったら0
    }
    Nezumi->WheelRotVol = GetMouseWheelRotVol() ;    //ホイール回転量取得
    return 0;
}
