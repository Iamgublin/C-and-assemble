//キャラクターに関する構造体
typedef struct{
	int flag;		//フラグ
	int cnt;		//カウンタ
	int power;		//パワー
	int point;		//ポイント
	int score;		//スコア
	int num;		//残機数
	int mutekicnt;	//無敵状態とカウント
	int shot_mode;	//ショットモード
	int money;		//お金
	int img;
	int slow;		//スローかどうか
	double x,y;		//座標
	int shot_cnt;	//ショットカウント
	int graze;		//グレイズ(41)
}ch_t;

typedef struct{
	double x,y;
}pt_t;

//敵に関する構造体
typedef struct{
	int flag,cnt,pattern,muki,knd,hp,hp_max,item_n[6],img,back_col;
	//フラグ、カウンタ、移動パターン、向き、敵の種類、HP最大値、落とすアイテム、背景色
	double x,y,vx,vy,sp,ang;
	//座標、速度x成分、速度y成分、スピード、角度
	int bltime,blknd,blknd2,col,state,wtime,wait;
	//弾幕開始時間、弾幕の種類、弾の種類、色、状態、待機時間、停滞時間
}enemy_t;

typedef struct{
	//カウンタ、移動パターン、敵の種類
	int cnt,pattern,knd;
	//初期座標と移動スピード
	double x,y,sp;
	//弾幕開始時間、弾幕の種類、色、体力、弾の種類、停滞時間、アイテム(6種類)
	int bltime,blknd,col,hp,blknd2,wait,item_n[6];
}enemy_order_t;

//パッドに関する構造体
typedef struct{
	int key[PAD_MAX];
}pad_t;

//コンフィグに関する構造体
typedef struct{
	int left,up,right,down,shot,bom,slow,start,change;
}configpad_t;

//弾に関する構造体
typedef struct{
	//フラグ、種類、カウンタ、色、状態、少なくとも消さない時間、エフェクトの種類
	int flag,knd,cnt,col,state,till,eff,kaiten,eff_detail;
	int cnt_till,cnt_stt;
	//座標、角度、速度、ベースの角度、一時記憶スピード
	double x,y,vx,vy,angle,spd,base_angle[1],rem_spd[1];
	int c_flag;
	double cx,cy;
}bullet_t;

//ショットに関する構造体
typedef struct{
	//フラグ、種類、カウンタ、どの敵から発射されたかの番号、色
	int flag,knd,cnt,num;
	//ベース角度、ベーススピード
	double base_angle[1],base_spd[1];
	bullet_t bullet[SHOT_BULLET_MAX];
}shot_t;

//ショットに関する構造体
typedef struct{
	//フラグ、種類、カウンタ、どの敵から発射されたかの番号、色
	int flag,knd,cnt,num;
	//ベース角度、ベーススピード
	double base_angle[10],base_spd[1];
	bullet_t bullet[BOSS_BULLET_MAX];
	bullet_t assist[6];
}boss_shot_t;

//キャラクターショットに関する構造体
typedef struct{
	int flag;		//フラグ
	int power;		//パワー
	int cnt;		//カウンタ
	int knd;		//種類
	int img;		//画像(49)
	double x,y;		//座標
	double angle;	//角度
	double spd;		//スピード
}cshot_t;

//ショットのオプション発射元ボンボン
typedef struct{
	int flag;	//フラグ
	int img;	//画像
	int cnt;	//カウンタ
	double x,y;	//座標
}option_bb_t;

//エフェクト
typedef struct{
	int flag,cnt,col,knd,img,eff;
	double x,y,r,ang,spd,mvang,brt;
}effect_t;

//消滅エフェクト
typedef struct{
	int flag,cnt,col;
	double x,y;
}del_effect_t;

//弾の情報
typedef struct{
	int size_x,size_y,col_num,kaiten;
	double range;
}bullet_info_t;

//ボムの情報
typedef struct{
	int flag,cnt,knd;
	double x,y;
}bom_t;

//画面の明るさ情報
typedef struct{
	unsigned char brt;
}bright_set_t;

//ドン！と揺れる画面の情報
typedef struct{
	int flag,cnt,time,size;
	int x,y;
}dn_t;

//物理的計算を行う為の構造体
typedef struct{
	int flag,cnt,set_t;
	double ax,v0x,ay,v0y,vx,vy,prex,prey;
}phy_t;

//レーザーの物理的計算を行う為の構造体
typedef struct{
	int conv_flag;
	double time,base_ang,angle;
	double conv_x,conv_y,conv_base_x,conv_base_y;
}lphy_t;

//ボスの情報
typedef struct{
	//フラグ、カウンタ、種類、待機時間、状態、弾幕終了までのカウンタ、後ろの羽衣、表示フラグ
	int flag,cnt,knd,wtime,state,endtime,hagoromo,graph_flag;
	//HP,最大HP
	int hp,hp_max;
	//出現カウンタ数、各弾幕のHP、背景の種類、中ボス-ラスボスの弾幕数(45)
	int appear_count[2],set_hp[DANMAKU_MAX],back_knd[DANMAKU_MAX],danmaku_num[2];
	//座標
	double x,y,dx,dy,ang,spd;
	//物理移動のための変数
	phy_t phy;
}boss_t;

//レーザーの構造体
typedef struct{
	int flag,cnt,knd,col,state;//フラグ、カウンタ、種類、色
	double haba,angle,length,hantei;//幅、角度、長さ、判定範囲(表示幅に対して0~1で指定)、回転速度
	pt_t startpt,disppt[4],outpt[4];//レーザーを発射する始点、表示座標、当たり判定範囲
	lphy_t lphy;
}lazer_t;

//弾幕の発射位置に表示するエフェクト
typedef struct{
	int flag,cnt,knd,col,state;
	double x,y,angle,spd,range;
}child_t;

//ゲームステージタイトル
typedef struct{
	//フラグ、画像ハンドル、カウンタ、輝度、出現カウント
	int flag,img,cnt,brt,appear_cnt;
}stage_title_t;

//アイテムの構造体
typedef struct{
	//フラグ、カウンタ、種類、状態
	int flag,cnt,knd,state;
	//速度、座標、大きさ
	double v,x,y,r;
}item_t;

//フラッシュ(42)
typedef struct{
	//フラグ、カウンタ、フラッシュの種類、何カウントで表現するか
	int flag,cnt,knd,to_cnt;
	//明るさ
	double brt;
}flash_t;

//BGM情報(area_tで使用
typedef struct{
	//BGMの再生フラグ、再生ハンドル、いつならすか再生種類番号をいれる、ループ位置
	int flag,handle,knd[2],loop_pos[2];
}bgm_t;

//そのエリアが持っている情報(48)
typedef struct{
	bgm_t bgm;
}area_t;

//漢字弾1つ分の情報(53)
typedef struct{
    int Knd;//弾の種類
    int Col;//弾の色
    float Angle;//弾の角度
    float x,y;
}Bl_t;

//漢字弾全体の情報(53)
typedef struct{
    int Num;//登録した個数
    Bl_t Bl[ FONT_BULLET_MAX ];//登録する弾情報
}BlPoint_t;

//AA弾1つ分の情報(56)
typedef struct{
    float x,y;
}BlAA_t;

//漢字弾全体の情報(56)
typedef struct{
    int Num;//登録した個数
    BlAA_t BlAA[ AA_BULLET_MAX ];//登録する弾情報
}BlAAPoint_t;
