#include "../include/GV.h"

void load(){
	img_board[10] = LoadGraph("../dat/img/board/10.png");
	img_board[11] = LoadGraph("../dat/img/board/11.png");
	img_board[12] = LoadGraph("../dat/img/board/12.png");
	img_board[20] = LoadGraph("../dat/img/board/20.png");
	LoadDivGraph( "../dat/img/char/0.png" , 12 , 4 , 3 , 73 , 73 , img_ch[0] ) ;
}