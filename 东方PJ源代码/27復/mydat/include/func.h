extern void boss_shot_bulletH000();
extern void boss_shot_bulletH001();

void (*boss_shot_bullet[DANMAKU_MAX])() =
{
	boss_shot_bulletH001,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
	boss_shot_bulletH000,
};