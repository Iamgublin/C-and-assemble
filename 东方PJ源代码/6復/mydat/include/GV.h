#include "../../../include/DxLib.h"
#include "define.h"

#ifdef GLOBAL_INSTANCE
#define GLOBAL
#else
#define GLOBAL extern 
#endif

#include "function.h"

//画像用変数宣言部
GLOBAL int img_ch[2][12];	//キャラクタ画像9枚分　X2(変身用)

//フラグ・ステータス変数
GLOBAL int func_state;		//関数制御用変数

//構造体変数宣言部
GLOBAL ch_t ch;			//キャラクタデータ宣言

