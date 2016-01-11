//円周率
#define PI 3.1415926535898
#define PI2 (PI*2)
//フィールドの広さ
#define FIELD_MAX_X 384
#define FIELD_MAX_Y 448
//フィールドの左上の座標
#define FIELD_X 32
#define FIELD_Y 16
//ボスの定位置
#define BOSS_POS_X (FIELD_MAX_X/2)
#define BOSS_POS_Y 100.0

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

#include "struct.h"