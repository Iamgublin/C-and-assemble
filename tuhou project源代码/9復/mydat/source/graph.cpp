#include "../include/GV.h"

void graph_ch(){
	DrawRotaGraphF(ch.x+FIELD_X,ch.y+FIELD_Y,1.0f,0.0f,img_ch[0][ch.img],TRUE);
}

void graph_board(){
	DrawGraph(	0,	0,img_board[10],FALSE);
	DrawGraph(	0, 16,img_board[11],FALSE);
	DrawGraph(	0,464,img_board[12],FALSE);
	DrawGraph(416,	0,img_board[20],FALSE);
}

void graph_main(){
	graph_ch();
	graph_board();
}