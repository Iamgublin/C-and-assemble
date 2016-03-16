#include "../include/GV.h"

extern void enemy_pattern0(int);extern void enemy_pattern1(int);extern void enemy_pattern2(int);
extern void enemy_pattern3(int);extern void enemy_pattern4(int);extern void enemy_pattern5(int);
extern void enemy_pattern6(int);extern void enemy_pattern7(int);extern void enemy_pattern8(int);
extern void enemy_pattern9(int);extern void enemy_pattern10(int);

void (*enemy_pattern[ENEMY_PATTERN_MAX])(int) = {
    enemy_pattern0,    enemy_pattern1,    enemy_pattern2,    enemy_pattern3,    enemy_pattern4,
    enemy_pattern5,    enemy_pattern6,    enemy_pattern7,    enemy_pattern8,    enemy_pattern9,
    enemy_pattern10,
};

//‹ó‚¢‚Ä‚¢‚é“G”Ô†‚ğŒŸõ
int enemy_num_search(){
    for(int i=0;i<ENEMY_MAX;i++){//ƒtƒ‰ƒO‚Ì‚½‚Á‚Ä–³‚¢enemy‚ğ’T‚·
        if(enemy[i].flag==0){
            return i;//g—p‰Â”\”Ô†‚ğ•Ô‚·
        }
    }
    return -1;//‘S•”–„‚Ü‚Á‚Ä‚¢‚½‚çƒGƒ‰[‚ğ•Ô‚·
}

//“Gî•ñ‚ğ“o˜^
void enemy_enter(){//“G‚Ìs“®‚ğ“o˜^E§Œä‚·‚éŠÖ”
    int i,j,t;
	if(boss.flag!=0)return;
    for(t=0;t<ENEMY_ORDER_MAX;t++){
        if(enemy_order[t].cnt==stage_count){//Œ»İ‚ÌuŠÔ‚ªƒI[ƒ_[‚ÌuŠÔ‚È‚ç
            if((i=enemy_num_search())!=-1){
                enemy[i].flag    =1;//ƒtƒ‰ƒO
                enemy[i].cnt    =0;//ƒJƒEƒ“ƒ^
                enemy[i].pattern=enemy_order[t].pattern;//ˆÚ“®ƒpƒ^[ƒ“
                enemy[i].muki    =1;//Œü‚«
                enemy[i].knd    =enemy_order[t].knd;//“G‚Ìí—Ş
                enemy[i].x        =enemy_order[t].x;//À•W
                enemy[i].y        =enemy_order[t].y;
                enemy[i].sp        =enemy_order[t].sp;//ƒXƒs[ƒh
                enemy[i].bltime    =enemy_order[t].bltime;//’e‚Ì”­ËŠÔ
                enemy[i].blknd    =enemy_order[t].blknd;//’e–‹‚Ìí—Ş
                enemy[i].blknd2    =enemy_order[t].blknd2;//’e‚Ìí—Ş
                enemy[i].col    =enemy_order[t].col;//F
                enemy[i].wait    =enemy_order[t].wait;//F
                enemy[i].hp        =enemy_order[t].hp;//‘Ì—Í
                enemy[i].hp_max =enemy[i].hp;//‘Ì—ÍÅ‘å’l
				enemy[i].back_col=GetRand(4);
                enemy[i].vx     =0;//…•½¬•ª‚Ì‘¬“x
                enemy[i].vy     =0;//‰”’¼¬•ª‚Ì‘¬“x
                enemy[i].ang    =0;//Šp“x
                for(j=0;j<6;j++)
                    enemy[i].item_n[j]=enemy_order[t].item_n[j];//—‚Æ‚·ƒAƒCƒeƒ€
            }
        }
    }
}

void enter_shot(int i){
	int j;
	for(j=0;j<SHOT_MAX;j++){//ƒtƒ‰ƒO‚Ì‚½‚Á‚Ä–³‚¢enemy‚ğ’T‚·
		if(shot[j].flag==0){//–¢g—p‚Ì’e–‹ƒf[ƒ^‚ª‚ ‚ê‚Î
			memset(&shot[j],0,sizeof(shot_t));//‰Šú‰»‚µ‚Ä“o˜^
			shot[j].flag=1;//ƒtƒ‰ƒO‚ğ‚½‚Ä‚é
			shot[j].knd=enemy[i].blknd;//’e‚Ìí—Ş
			shot[j].num=i;//num=‚Ç‚Ì“G‚©‚ç”­Ë‚³‚ê‚½‚à‚Ì‚©B
			shot[j].cnt=0;
			return ;
		}
	}
}

//“G‚Ìs“®§Œä
void enemy_act(){
    int i;
    for(i=0;i<ENEMY_MAX;i++){
        if(enemy[i].flag==1){//‚»‚Ì“G‚Ìƒtƒ‰ƒO‚ªƒIƒ“‚É‚È‚Á‚Ä‚½‚ç
            if(0<=enemy[i].pattern && enemy[i].pattern<ENEMY_PATTERN_MAX){
                enemy_pattern[enemy[i].pattern](i);
                enemy[i].x+=cos(enemy[i].ang)*enemy[i].sp;
                enemy[i].y+=sin(enemy[i].ang)*enemy[i].sp;
                enemy[i].x+=enemy[i].vx;
                enemy[i].y+=enemy[i].vy;
                enemy[i].cnt++;
                enemy[i].img=enemy[i].muki*3+(enemy[i].cnt%18)/6;
                //“G‚ª‰æ–ÊŠO‚ÉŠO‚ê‚½‚çÁ‚·
                if(enemy[i].x<-20 || FMX+20<enemy[i].x || enemy[i].y<-20 || FMY+20<enemy[i].y)
                    enemy[i].flag=0;
				if(enemy[i].bltime==enemy[i].cnt)
					enter_shot(i);
			}
            else
                printfDx("enemy[i].pattern‚Ì%d’l‚ª•s³‚Å‚·B",enemy[i].pattern);
        }
    }
}

//“Gˆ—ƒƒCƒ“
void enemy_main(){
    enemy_enter();
    enemy_act();
}