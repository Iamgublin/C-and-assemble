extern void boss_shot_bulletH000();
extern void boss_shot_bulletH001();
extern void boss_shot_bulletH002();
extern void boss_shot_bulletH003();
extern void boss_shot_bulletH004();
extern void boss_shot_bulletH005();
extern void boss_shot_bulletH006();
extern void boss_shot_bulletH007();
extern void boss_shot_bulletH008();
extern void boss_shot_bulletH009();
extern void boss_shot_bulletH010();
extern void boss_shot_bulletH011();
extern void boss_shot_bulletH012();//(53)
extern void boss_shot_bulletH013();//(53)
extern void boss_shot_bulletH014();//(56)

void (*boss_shot_bullet[DANMAKU_MAX])() =
{
        boss_shot_bulletH014,//漢字弾幕(56)

		boss_shot_bulletH012,//漢字弾幕(53)
        boss_shot_bulletH013,//漢字弾幕(53)

//中ボス
        boss_shot_bulletH000,//ノーマル
        boss_shot_bulletH002,//スペル～パーフェクトフリーズ～
//ラスボス
        boss_shot_bulletH001,//ノーマル～サイレントセレナ～
        boss_shot_bulletH004,//スペル～ケロちゃん～
        boss_shot_bulletH009,//スペル～反魂蝶～
//終り

        boss_shot_bulletH000,//ぬるぽ回避用(この弾幕は出てこない)
};
