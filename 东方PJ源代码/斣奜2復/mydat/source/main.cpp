#define GLOBAL_INSTANCE 
#include "../include/GV.h"

//ループで必ず行う３大処理
int ProcessLoop(){
	if(ProcessMessage()!=0)return -1;//プロセス処理がエラーなら-1を返す
	if(ClearDrawScreen()!=0)return -1;//画面クリア処理がエラーなら-1を返す
	GetHitKeyStateAll_2();//現在のキー入力処理を行う
	GetHitPadStateAll();  //現在のパッド入力処理を行う
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	//ChangeWindowMode(TRUE);//ウィンドウモード
	if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1;//初期化と裏画面化
	while(ProcessLoop()==0){//メインループ
		music_ini();
		switch(func_state){
			case 0://初回のみ入る処理
				load();		//データロード
				first_ini();//初回の初期化
				func_state=99;
				break;
			case 99://STGを始める前に行う初期化
				ini();
				load_story();
				func_state=100;
				break;
			case 100://通常処理
				calc_ch();   //キャラクタ計算
				ch_move();   //キャラクタの移動制御
				cshot_main();//自機ショットメイン
				enemy_main();//敵処理メイン
				boss_shot_main();
				shot_main(); //ショットメイン
				out_main();  //当たり計算
				effect_main();//エフェクトメイン
				calc_main();//ゲームタイトル表示計算
				graph_main();//描画メイン
				if(boss.flag==0)
					stage_count++;
				break;
			default:
				printfDx("不明なfunc_state\n");
				break;
		}
		music_play();
		wait_fanc();//fps計算
		fps();//fps表示		
		if(CheckStateKey(KEY_INPUT_ESCAPE)==1)break;//エスケープが入力されたらブレイク
		ScreenFlip();//裏画面反映
		count++;
	}
	DxLib_End();//ＤＸライブラリ終了処理
	return 0;
}
