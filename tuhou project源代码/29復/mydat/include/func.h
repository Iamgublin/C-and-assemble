extern void boss_shot_bulletH000();
extern void boss_shot_bulletH001();
extern void boss_shot_bulletH002();
extern void boss_shot_bulletH003();
extern void boss_shot_bulletH004();
extern void boss_shot_bulletH005();

void (*boss_shot_bullet[DANMAKU_MAX])() =
{
	boss_shot_bulletH003,
	boss_shot_bulletH002,
	boss_shot_bulletH001,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
};