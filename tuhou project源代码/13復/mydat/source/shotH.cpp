#include "../include/GV.h"

//1”­‚¾‚¯A©‹@‚ÉŒü‚©‚Á‚Ä’¼üˆÚ“®
void shot_bullet_H000(int n){
	int k;
	if(shot[n].cnt==0){
		if(shot[n].flag!=2 && (k=shot_search(n))!=-1){
			shot[n].bullet[k].knd	=enemy[shot[n].num].blknd2;
			shot[n].bullet[k].angle	=shotatan2(n);
			shot[n].bullet[k].flag	=1;
			shot[n].bullet[k].x		=enemy[shot[n].num].x;
			shot[n].bullet[k].y		=enemy[shot[n].num].y;
			shot[n].bullet[k].col	=enemy[shot[n].num].col;
			shot[n].bullet[k].cnt	=0;
			shot[n].bullet[k].spd	=3;
			se_flag[0]=1;
		}
	}
}
