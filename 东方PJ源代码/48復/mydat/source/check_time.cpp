#include "../include/GV.h"
//名前の最大文字数
#define STR_MAX 64
//調べられる関数の最大数
#define FUNC_MAX 30
//今何個目かをカウントする
int func_count;
//前回測定した時間を保存する
LONGLONG lt;
//測定データ格納用変数
typedef struct{
	int tm;
	char str[STR_MAX];
}func_tm_t;
func_tm_t func_tm[FUNC_MAX];

//stという名前で受け取ったデータを登録する。flagが1ならリセット
void enter_func_tm(char st[], int flag){
	int i;
	LONGLONG nowtm;
	if(func_count>=FUNC_MAX){
		printfDx("func_countの値%dが異常です\n",func_count);
		return ;
	}
	nowtm=GetNowHiPerformanceCount() ;
	if(nowtm-lt<INT_MAX){//intの扱える範囲なら
		func_tm[func_count].tm=(int)(nowtm-lt);//処理時間格納
		memcpy(func_tm[func_count].str,st,STR_MAX-1);//文字列コピー
		func_tm[func_count].str[STR_MAX-1]=0;//終端記号入れる
	}
	else//扱えなかったら
		func_tm[func_count].tm=-1;//エラー

	lt=nowtm ;//前回記録した時刻として保存

	if(flag==1){//リセット
		for(i=func_count+1;i<FUNC_MAX;i++)
			func_tm[i].str[0]=0;
		func_count=0;
	}
	else
		func_count++;
}

//データを描画
void draw_func_tm(int x, int y){
	int i;
	unsigned int total=0;
	for(i=0;i<FUNC_MAX;i++){
		if(func_tm[i].str[0]==0)break;
		DrawFormatStringToHandle(x,y+14*i,color[0],font[0],"%05.2f:%s",func_tm[i].tm/1000.0,func_tm[i].str );
		total+=func_tm[i].tm;
	}
	DrawFormatStringToHandle(x,y+14*i,color[0],font[0],"合計:%05.2f",total/1000.0);
}