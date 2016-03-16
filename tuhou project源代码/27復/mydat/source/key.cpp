#include "../include/GV.h"

unsigned int stateKey[256];

int GetHitKeyStateAll_2(){
    char GetHitKeyStateAll_Key[256];
    GetHitKeyStateAll( GetHitKeyStateAll_Key );
    for(int i=0;i<256;i++){
        if(GetHitKeyStateAll_Key[i]==1) stateKey[i]++;
        else                            stateKey[i]=0;
    }
    return 0;
}

int CheckStateKey(unsigned char Handle){
	return stateKey[Handle];
}

//パッドの入力状態を格納する変数
pad_t pad;

//引数１と引数２のうち大きい方を引数１に代入する
void input_pad_or_key(int *p, int k){
	*p = *p>k ? *p : k;
}

//パッドとキーボードの両方の入力をチェックする関数
void GetHitPadStateAll(){
	int i,PadInput,mul=1;
	PadInput = GetJoypadInputState( DX_INPUT_PAD1 );//パッドの入力状態を取得
	for(i=0;i<16;i++){
		if(PadInput & mul)	pad.key[i]++;
		else				pad.key[i]=0;
		mul*=2;
	}
	input_pad_or_key(&pad.key[configpad.left]	,CheckStateKey(KEY_INPUT_LEFT	 ));
	input_pad_or_key(&pad.key[configpad.up]	    ,CheckStateKey(KEY_INPUT_UP	     ));
	input_pad_or_key(&pad.key[configpad.right]  ,CheckStateKey(KEY_INPUT_RIGHT	 ));
	input_pad_or_key(&pad.key[configpad.down]	,CheckStateKey(KEY_INPUT_DOWN	 ));
	input_pad_or_key(&pad.key[configpad.shot]	,CheckStateKey(KEY_INPUT_Z	     ));
	input_pad_or_key(&pad.key[configpad.bom]	,CheckStateKey(KEY_INPUT_X		 ));
	input_pad_or_key(&pad.key[configpad.slow]	,CheckStateKey(KEY_INPUT_LSHIFT	 ));
	input_pad_or_key(&pad.key[configpad.start]  ,CheckStateKey(KEY_INPUT_ESCAPE	 ));
	input_pad_or_key(&pad.key[configpad.change] ,CheckStateKey(KEY_INPUT_LCONTROL));
}

//渡されたパッドキー番号の入力状態を返す。返り値が-1なら不正
int CheckStatePad(unsigned int Handle){
	if(0<=Handle && Handle<PAD_MAX){
		return pad.key[Handle];
	}
	else{
		printfDx("CheckStatePadに渡した値が不正です\n");
		return -1;
	}
}