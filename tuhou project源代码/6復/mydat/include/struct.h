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
