#include "../include/GV.h"

extern void enter_char_item();//(39章)

void calc_ch(){
    if(ch.flag==1){//喰らいボム受付中
        bright_set.brt=80;//暗く
        if(ch.cnt>20){//0.33秒受け付ける
            ch.flag   =2;    //1:喰らいボム受付中　2:死んで浮き上がり中
            ch.cnt    =0;
            bright_set.brt=255;
        }
    }
    if(ch.cnt==0 && ch.flag==2){//今の瞬間死んだら
		enter_char_item();//キャラのアイテム登録(39章)
        ch.x=FMX/2;//座標セット
        ch.y=FMY-130;//(b1)
        ch.mutekicnt++;//無敵状態へ
    }
    if(ch.flag==2){//死んで浮上中なら
        unsigned int push=CheckStatePad(configpad.left)+CheckStatePad(configpad.right)
            +CheckStatePad(configpad.up)+CheckStatePad(configpad.down);
        ch.y-=1.5;//キャラを上に上げる
        //１秒以上か、キャラがある程度上にいて、何かおされたら
        if(ch.cnt>60 || (ch.y<FMY-20 && push)){
            ch.cnt=0;
            ch.flag=0;//キャラステータスを元に戻す
        }
    }
    if(ch.mutekicnt>0){//無敵カウントが0じゃなければ
        ch.mutekicnt++;
        if(ch.mutekicnt>150)//150以上たったら
            ch.mutekicnt=0;//戻す
    }
    ch.cnt++;//キャラクタカウントアップ
    ch.img=(ch.cnt%24)/6;//現在の画像決定
}

void ch_move(){
    int i,sayu_flag=0,joge_flag=0;
    double x,y,mx,my,naname=1;
    double move_x[4]={-4.0,4.0,0,0},move_y[4]={0,0,4.0,-4.0};
    int inputpad[4];
    inputpad[0]=CheckStatePad(configpad.left); inputpad[1]=CheckStatePad(configpad.right);
    inputpad[2]=CheckStatePad(configpad.down); inputpad[3]=CheckStatePad(configpad.up);

    if(ch.flag==1)return;//喰らいボム受付中は動かせない

    if(CheckStatePad(configpad.left)>0)//左キーが押されていたら
        ch.img+=4*2;//画像を左向きに
    else if(CheckStatePad(configpad.right)>0)//右キーが押されていたら
        ch.img+=4*1;//画像を右向きに

    for(i=0;i<2;i++)//左右分
        if(inputpad[i]>0)//左右どちらかの入力があれば
            sayu_flag=1;//左右入力フラグを立てる
    for(i=2;i<4;i++)//上下分
        if(inputpad[i]>0)//上下どちらかの入力があれば
            joge_flag=1;//上下入力フラグを立てる
    if(sayu_flag==1 && joge_flag==1)//左右、上下両方の入力があれば斜めだと言う事
        naname=sqrt(2.0);//移動スピードを1/ルート2に

    for(int i=0;i<4;i++){//4方向分ループ
        if(inputpad[i]>0){//i方向のキーボード、パッドどちらかの入力があれば
            x=ch.x , y=ch.y;//今の座標をとりあえずx,yに格納
            mx=move_x[i];    my=move_y[i];//移動分をmx,myに代入
            if(CheckStatePad(configpad.slow)>0){//低速移動なら
                mx=move_x[i]/3;    my=move_y[i]/3;//移動スピードを1/3に
            }
            x+=mx/naname , y+=my/naname;//今の座標と移動分を足す
            if(!(x<10 || x>FMX-10 || y<5 || y>FMY-120)){//計算結果移動可能範囲内なら(b1)
                ch.x=x , ch.y=y;//実際に移動させる
            }
        }
    }
}

