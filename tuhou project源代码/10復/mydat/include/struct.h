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
}ch_t;

//パッドに関する構造体
typedef struct{
	int key[PAD_MAX];
}pad_t;

//コンフィグに関する構造体
typedef struct{
	int left,up,right,down,shot,bom,slow,start,change;
}configpad_t;

//敵に関する構造体
typedef struct{
        int flag,cnt,pattern,muki,knd,hp,hp_max,item_n[6],img;
        //フラグ、カウンタ、移動パターン、向き、敵の種類、HP最大値、落とすアイテム
        double x,y,vx,vy,sp,ang;
        //座標、速度x成分、速度y成分、スピード、角度
        int bltime,blknd,blknd2,col,state,wtime,wait;
        //弾幕開始時間、弾幕の種類、弾の種類、色、状態、待機時間、停滞時間
}enemy_t;
