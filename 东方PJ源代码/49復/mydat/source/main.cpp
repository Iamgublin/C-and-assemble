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
	ChangeWindowMode(TRUE);//ウィンドウモード
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
								 enter_func_tm("最初");
				calc_ch();       enter_func_tm("キャラ計算");
				ch_move();       enter_func_tm("キャラ移動");
				cshot_main();    enter_func_tm("自機ショットメイン");
				enemy_main();	 enter_func_tm("敵処理メイン");
				boss_shot_main();enter_func_tm("ボスショットメイン");
				shot_main();	 enter_func_tm("ショットメイン");
				out_main();  	 enter_func_tm("当たり判定");
				effect_main();   enter_func_tm("エフェクトメイン");
				calc_main();	 enter_func_tm("計算メイン");
				graph_main();    enter_func_tm("描画メイン");
				bgm_main();		 enter_func_tm("BGMメイン");
				if(boss.flag==0)
					stage_count++;
				break;
			default:
				printfDx("不明なfunc_state\n");
				break;
		}
		music_play();			 enter_func_tm("音楽再生");
		fps_wait();				 enter_func_tm("待機した時間",1);
		if(CheckStateKey(KEY_INPUT_ESCAPE)==1)break;//エスケープが入力されたらブレイク
		ScreenFlip();//裏画面反映
		count++;

	}
	DxLib_End();//ＤＸライブラリ終了処理
	return 0;
}
