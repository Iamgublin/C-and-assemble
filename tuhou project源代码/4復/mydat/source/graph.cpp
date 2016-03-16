#include "../include/GV.h"

extern ch_t ch;
extern int img_ch[2][12];

void graph_ch(){
	DrawRotaGraphF(ch.x,ch.y,1.0f,0.0f,img_ch[0][ch.img],TRUE);
}

void graph_main(){
	graph_ch();
}