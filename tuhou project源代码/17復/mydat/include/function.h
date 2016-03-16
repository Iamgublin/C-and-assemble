//enemy.cpp
	GLOBAL void enemy_main();

//enemy_act_pattern.cpp
	GLOBAL double rang(double);

//cshot.cpp
	GLOBAL void cshot_main();

//char.cpp
	GLOBAL void calc_ch();
	GLOBAL void ch_move();

//graph.cpp
	//描画メイン
	GLOBAL void graph_main();

//ini.cpp
	GLOBAL void first_ini();
	GLOBAL void ini();

//shot.cpp
	GLOBAL double shotatan2(int n);
	GLOBAL int shot_search(int n);
	GLOBAL void shot_main();

//key.cpp
	//現在のキー入力処理を行う
	GLOBAL int GetHitKeyStateAll_2();
	//受け取ったキー番号の現在の入力状態を返す
	GLOBAL int CheckStateKey(unsigned char Handle);

	//現在のパッド入力処理を行う関数
	GLOBAL void GetHitPadStateAll();
	//受け取ったパッド番号の現在の入力状態を返す
	GLOBAL int CheckStatePad(unsigned int Handle);

//laod.cpp
	//データのロード
	GLOBAL void load();
	GLOBAL void load_story();

//music.cpp
	GLOBAL void music_ini();
	GLOBAL void music_play();