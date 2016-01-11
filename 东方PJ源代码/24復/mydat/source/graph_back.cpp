#include "../include/GV.h"

void graph_back00(){
	SetDrawArea( 32 , 16 , 416 , 464 ) ;//描画可能エリアを設定
	DrawGraph(FIELD_X,count%700+FIELD_Y-700,img_back[0],FALSE);
	DrawGraph(FIELD_X,count%700+FIELD_Y    ,img_back[0],FALSE);
	SetDrawArea( 0, 0, 640, 480);//エリアを戻す
}

void graph_back_main(){
	graph_back00();
}