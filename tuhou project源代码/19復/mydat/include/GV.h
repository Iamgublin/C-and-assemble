#include "../../../include/DxLib.h"
#include "define.h"
#include "math.h"

#ifdef GLOBAL_INSTANCE
#define GLOBAL
#else
#define GLOBAL extern 
#endif

#include "function.h"

//画像用変数宣言部
GLOBAL int img_ch[2][12];	//キャラクタ画像12枚分　X2(変身用)
GLOBAL int img_enemy[3][9];	//敵画像9枚分
GLOBAL int img_board[40];	//ボードにつかう画像
GLOBAL int img_bullet[10][10];
GLOBAL int img_cshot[2];	//自機ショット用画像
GLOBAL int img_del_effect[5];

//音楽ファイル用変数宣言部
GLOBAL int sound_se[SE_MAX];

//フラグ・ステータス変数
GLOBAL int func_state,stage_count;	//関数制御用変数
GLOBAL int se_flag[SE_MAX];			//サウンドフラグ

//構造体変数宣言部
GLOBAL ch_t ch;			//キャラクタデータ宣言
GLOBAL enemy_t enemy[ENEMY_MAX];//敵情報
GLOBAL configpad_t configpad;//コンフィグで設定したキー情報
GLOBAL enemy_order_t enemy_order[ENEMY_ORDER_MAX];//敵の出現情報
GLOBAL shot_t shot[SHOT_MAX];//ショット情報
GLOBAL cshot_t cshot[CSHOT_MAX];//自機ショット
GLOBAL del_effect_t del_effect[DEL_EFFECT_MAX];//消滅エフェクト
GLOBAL effect_t effect[EFFECT_MAX];//エフェクト
