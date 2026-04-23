# pragma once
# include <Siv3d.hpp>

namespace Iwanna {
	namespace Sound {

		const String MAIN_LOW{ U"main_low" };
		const FilePath Path_MAIN_LOW{ U"Sound/Bgm/main_low.mp3" };
		const String MAIN_NORMAL{ U"main_normal" };
		const FilePath Path_MAIN_NORMAL{ U"Sound/Bgm/main_normal.mp3" };
		const String BOSS_NORMAL{ U"boss_normal" };
		const FilePath Path_BOSS_NORMAL{ U"Sound/Bgm/boss_normal.mp3" };
		const String GAMEOVER_NORMAL{ U"gameover_normal" };
		const FilePath Path_GAMEOVER_NORMAL{ U"Sound/Bgm/gameover_normal.mp3" };
		const String EX_BOSS{ U"ex_boss" };
		const FilePath Path_EX_BOSS{ U"Sound/Bgm/ex_boss.mp3" };

		//SEの名前とパスの定数
		static const String SE_PATH = U"Sound/Se/";

		static const String DEATH = U"sndDeath";
		static const String DEATH_PATH = SE_PATH + DEATH + U".mp3";
		static const String JUMP = U"sndJump";
		static const String JUMP_PATH = SE_PATH + JUMP + U".mp3";
		static const String DJUMP = U"sndDJump";
		static const String DJUMP_PATH = SE_PATH + DJUMP + U".mp3";
		static const String SHOOT = U"sndShoot";
		static const String SHOOT_PATH = SE_PATH + SHOOT + U".mp3";
		static const String CHERRYFALL = U"sndCherry";
		static const String CHERRYFALL_PATH = SE_PATH + CHERRYFALL + U".mp3";
		static const String BLOCKCHANGE = U"sndBlockChange";
		static const String BLOCKCHANGE_PATH = SE_PATH + BLOCKCHANGE + U".mp3";
		static const String BLOCKBREAK = U"sndBreak";
		static const String BLOCKBREAK_PATH = SE_PATH + BLOCKBREAK + U".mp3";
		static const String SPIKETRAP = U"sndSpikeTrap";
		static const String SPIKETRAP_PATH = SE_PATH + SPIKETRAP + U".wav";
		static const String BOSSHIT = U"sndBossHit";
		static const String BOSSHIT_PATH = SE_PATH + BOSSHIT + U".wav";
		static const String ERROR = U"sndWindowsError";
		static const String ERROR_PATH = SE_PATH + ERROR + U".wav";
		static const String DISCORD = U"sndDiscord";
		static const String DISCORD_PATH = SE_PATH + DISCORD + U".mp3";
		static const String TITLECARD = U"sndTitleCard";
		static const String TITLECARD_PATH = SE_PATH + TITLECARD + U".wav";

		//EXボスSE
		static const String EXPRO= U"sndExpro";
		static const String EXPRO_PATH = SE_PATH + EXPRO + U".mp3";
		static const String SPARK = U"sndSpark";
		static const String SPARK_PATH = SE_PATH + SPARK + U".mp3";
		static const String SORD_WEAK = U"sndSordWeak";
		static const String SORD_WEAK_PATH = SE_PATH + SORD_WEAK + U".mp3";
		static const String SORD_STRONG = U"sndSordStrong";
		static const String SORD_STRONG_PATH = SE_PATH + SORD_STRONG + U".mp3";
		
		void registerBGMs();
		void registerSEs();
		void registerAudio(const String& name, const String& path, const Loop loop = Loop::No);
		void registerAudiosSync();
		void loadAudiosSync();
	}
}
