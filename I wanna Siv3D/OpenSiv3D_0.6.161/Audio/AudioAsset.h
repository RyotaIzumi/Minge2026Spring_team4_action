# pragma once
# include <Siv3d.hpp>

namespace Iwanna {
	namespace Sound {

		const String MAIN_LOW{ U"main_low" };
		const FilePath Path_MAIN_LOW{ U"Sound/Bgm/main_low.mp3" };
		const String MAIN_NORMAL{ U"main_normal" };
		const FilePath Path_MAIN_NORMAL{ U"Sound/Bgm/main_normal.mp3" };

		//SEの名前とパスの定数
		static const String SE_PATH = U"Sound/Se/";

		static const String DEATH = U"sndDeath";
		static const String DEATH_PATH = SE_PATH + DEATH + U".wav";
		static const String JUMP = U"sndJump";
		static const String JUMP_PATH = SE_PATH + JUMP + U".wav";
		static const String DJUMP = U"sndDJump";
		static const String DJUMP_PATH = SE_PATH + DJUMP + U".wav";
		static const String SHOOT = U"sndShoot";
		static const String SHOOT_PATH = SE_PATH + SHOOT + U".wav";
		static const String CHERRYFALL = U"sndCherry";
		static const String CHERRYFALL_PATH = SE_PATH + CHERRYFALL + U".wav";
		static const String BLOCKCHANGE = U"sndBlockChange";
		static const String BLOCKCHANGE_PATH = SE_PATH + BLOCKCHANGE + U".wav";
		static const String BLOCKBREAK = U"sndBreak";
		static const String BLOCKBREAK_PATH = SE_PATH + BLOCKBREAK + U".wav";
		static const String SPIKETRAP = U"sndSpikeTrap";
		static const String SPIKETRAP_PATH = SE_PATH + SPIKETRAP + U".wav";
		static const String ERROR = U"sndWindowsError";
		static const String ERROR_PATH = SE_PATH + ERROR + U".wav";
		
		void registerBGMs();
		void registerSEs();
		void registerAudio(const String& name, const String& path, const Loop loop = Loop::No);
		void registerAudiosSync();
		void loadAudiosSync();
	}
}
