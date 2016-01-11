#include "../include/GV.h"

int fps_wait;

void wait_fanc(){//60fps以上にならないようにする関数
   LONGLONG miri_wait=0,micro_wait=0,wait=0,wait2=0,start=0;
   static LONGLONG before_time=0;
   static int cnt=0;
   if(cnt!=0){//最初はbefore_timeに値が入っていないので飛ばす
	   //まずSleep関数の誤差を踏まえて16666-1000の15666μ秒待機したい。
	   //待機すべき時間=15666-(かかった時間)により
	   //waitに待機したい時間を格納する。
	   wait=15666-(GetNowHiPerformanceCount()-before_time);
      if(wait>0){//待機すべき時間があれば
         //15666マイクロ秒の、まず15ミリ秒になるまでSleepする。
         miri_wait=wait/1000;
         Sleep(miri_wait);
	  }
      //次の正確な待機はwhileで行う。
	  //待機すべき時間=15666-(かかった時間)により
      //wait2にマイクロ秒待機したい時間を格納する。
      wait2=16600-(GetNowHiPerformanceCount()-before_time);
	  if(wait2>0){
		 micro_wait=wait2;
		 start=GetNowHiPerformanceCount();
		 while(GetNowHiPerformanceCount()-start<micro_wait);
      }
      //結局1周にかかった時間をfps_waitに格納する。16666が入る事を期待する。
      //fps_waitはunsigned intのグローバス変数
      fps_wait=(unsigned int)(GetNowHiPerformanceCount()-before_time);
   }
   before_time=GetNowHiPerformanceCount();
   cnt++;
   return;
}

void fps(){//fpsを計算する関数
        int i;
		double fps;
		static int cnt=0;
		static int f[60]={};
		static double ave=0;
		int fn=func_state;
		if(!(fn==1||fn==11||fn==100||fn==1000||fn==0)){
			cnt=0;
			return;
		}
        f[cnt]=fps_wait;
        if(cnt==39){
                ave=0;
				double sum=0;
				for(i=10;i<40;i++){
					sum+=f[i];
				}
				sum/=30;
				for(i=10;i<40;i++){
					if(f[i]>sum*1.5)
						f[i]=sum;
					ave+=f[i];
				}
                ave/=30.0;
				ave/=1000.0;
        }
		if(ave>10){
			int col; char st[64];
			fps=1000/ave;
			if(fps>65){col=GetColor(255,0,0);strcpy(st,"FPSが高すぎる");}
			if(56<=fps && fps<=65){col=GetColor(255,255,255);strcpy(st,"");}
			if(50<=fps && fps<56){col=GetColor(255,255,0);strcpy(st,"スペック不足");}
			if(fps<50){col=GetColor(255,0,0);strcpy(st,"非常にスペック不足");}
			if(count>120 && fps>5){
				if(func_state==1)
					DrawFormatStringToHandle(0,0,col,font[5],"[%.1f] %s",fps,st);
				else
					DrawFormatStringToHandle(0,462,col,font[5],"[%.1f] %s",fps,st);
			}
		}
		cnt++;
		if(cnt>=40)
			cnt=0;
        return;
}