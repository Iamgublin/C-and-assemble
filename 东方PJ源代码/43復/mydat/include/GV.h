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
GLOBAL int img_chetc[10];	//キャラクタに関する他の画像
GLOBAL int img_enemy[3][9];	//敵画像9枚分
GLOBAL int img_board[40];	//ボードにつかう画像
GLOBAL int img_bullet[20][10];//弾用画像
GLOBAL int img_lazer[10][10],img_lazer_moto[10][10];//レーザー画像
GLOBAL int img_cshot[2];	//自機ショット用画像
GLOBAL int img_del_effect[5];//消滅エフェクト用画像
GLOBAL int img_back[20];//背景用画像
GLOBAL int img_eff_bom[5];	//ボムのエフェクト画像
GLOBAL int img_dot_riria[8];//リリアのドット絵画像
GLOBAL int img_etc[50];//その他の画像
GLOBAL int img_item[6][2];//アイテムの画像
GLOBAL int img_num[3][12];//数字の画像(41)

//音楽ファイル用変数宣言部
GLOBAL int sound_se[SE_MAX];

//フラグ・ステータス変数
GLOBAL int func_state,stage_count,count,stage;	//関数制御用変数
GLOBAL int se_flag[SE_MAX];			//サウンドフラグ

//設定用変数
GLOBAL int stage_title_count[STAGE_NUM];

//その他の変数
GLOBAL int color[20];//色(41)

//構造体変数宣言部
GLOBAL ch_t ch;			//キャラクタデータ宣言
GLOBAL enemy_t enemy[ENEMY_MAX];//敵情報
GLOBAL configpad_t configpad;//コンフィグで設定したキー情報
GLOBAL enemy_order_t enemy_order[ENEMY_ORDER_MAX];//敵の出現情報
GLOBAL shot_t shot[SHOT_MAX];//ショット情報
GLOBAL boss_shot_t boss_shot;//ボスショット情報
GLOBAL cshot_t cshot[CSHOT_MAX];//自機ショット
GLOBAL del_effect_t del_effect[DEL_EFFECT_MAX];//消滅エフェクト
GLOBAL effect_t effect[EFFECT_MAX];//エフェクト
GLOBAL bullet_info_t bullet_info[20];//弾情報
GLOBAL bom_t bom;				//ボム
GLOBAL bright_set_t bright_set; //描画の輝度
GLOBAL dn_t dn;					//ドンと揺れる
GLOBAL boss_t boss;				//ボス情報
GLOBAL lazer_t lazer[LAZER_MAX];//レーザー
GLOBAL child_t child[CHILD_MAX];//弾幕の発射位置に表示するエフェクト
GLOBAL stage_title_t stage_title;//ステージタイトル
GLOBAL item_t item[ITEM_MAX];//アイテム
GLOBAL flash_t flash;//(42)

