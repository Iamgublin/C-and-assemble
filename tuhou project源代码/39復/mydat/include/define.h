//円周率
#define PI 3.1415926535898
#define PI2 (PI*2)
//フィールドの広さ
#define FMX 384
#define FMY 448
//フィールドの左上の座標
#define FX 32
#define FY 16
//ボスの定位置
#define BOSS_POS_X (FMX/2)
//#define BOSS_POS_Y 120.0
#define BOSS_POS_Y (FMY/2)

//効果音の種類の最大数
#define SE_MAX 100

//パッドキーの最大数
#define PAD_MAX 16

//敵の同時表示最大数
#define ENEMY_MAX 30
//敵の出現情報最大数
#define ENEMY_ORDER_MAX 500
//敵の行動パターンの最大数
#define ENEMY_PATTERN_MAX 11
//自機の当たり判定範囲
#define CRANGE 2.0
#define CRANGE11 5.0
//敵１匹が持つ弾の最大数
#define SHOT_BULLET_MAX 1000
//ボスが持つ弾の最大数
#define BOSS_BULLET_MAX 3000
//一度に画面に表示できる敵の弾幕の最大数
#define SHOT_MAX 30
//ショットの種類の最大数
#define SHOT_KND_MAX 18
//自機ショットの登録最大数
#define CSHOT_MAX 200
//エフェクト登録最大数
#define EFFECT_MAX 100
//消滅エフェクト登録最大数
#define DEL_EFFECT_MAX 30
//弾幕最大数
#define DANMAKU_MAX 50
//レーザー
#define LAZER_MAX 100
//
#define CHILD_MAX 20
//ステージ数
#define STAGE_NUM 5
//アイテムの表示最大数
#define ITEM_MAX 100
//アイテムゲットボーダーライン
#define ITEM_GET_BORDER_LINE 100
//アイテムの吸収範囲
#define ITEM_INDRAW_RANGE 70

#include "struct.h"