# include "AudioAsset.h"
# include <cassert>

namespace Iwanna {
	namespace Sound {

		struct SoundRegistry {
			String name;
			String path;
			Loop loop = Loop::No;
		};

		Array<SoundRegistry> registry;

		//BGMを登録する
		void registerBGMs() {
			registerAudio(MAIN_LOW, Path_MAIN_LOW, Loop::Yes);
			registerAudio(MAIN_NORMAL, Path_MAIN_NORMAL, Loop::Yes);
			registerAudio(MAIN_HIGH, Path_MAIN_HIGH, Loop::Yes);
			registerAudio(SECRET, Path_SECRET, Loop::Yes);
			registerAudio(BOSS_NORMAL, Path_BOSS_NORMAL, Loop::Yes);
			registerAudio(GAMEOVER_NORMAL, Path_GAMEOVER_NORMAL, Loop::No);
			registerAudio(EX_BOSS, Path_EX_BOSS, Loop::Yes);
		}
		
		//SEを登録する
		void registerSEs() {
			registerAudio(DEATH, DEATH_PATH);
			registerAudio(JUMP, JUMP_PATH);
			registerAudio(DJUMP, DJUMP_PATH);
			registerAudio(SHOOT, SHOOT_PATH);
			registerAudio(CHERRYFALL, CHERRYFALL_PATH);
			registerAudio(BLOCKCHANGE, BLOCKCHANGE_PATH);
			registerAudio(BLOCKBREAK, BLOCKBREAK_PATH);
			registerAudio(SPIKETRAP, SPIKETRAP_PATH);
			registerAudio(BOSSHIT, BOSSHIT_PATH);
			registerAudio(ERROR,ERROR_PATH);
			registerAudio(DISCORD,DISCORD_PATH);
			registerAudio(TITLECARD, TITLECARD_PATH);
			registerAudio(ITEM_GET, ITEM_GET_PATH);
			registerAudio(ACHIEVE, ACHIEVE_PATH);

			//EXボスSE
			registerAudio(EXPRO, EXPRO_PATH);
			registerAudio(SPARK, SPARK_PATH);
			registerAudio(SORD_WEAK, SORD_WEAK_PATH);
			registerAudio(SORD_STRONG, SORD_STRONG_PATH);
			registerAudio(BOSS_WARP, BOSS_WARP_PATH);

			//TayamaBoss VC
			registerAudio(VC_BAAN, VC_BAAN_PATH);
			registerAudio(VC_BAKANA, VC_BAKANA_PATH);
			registerAudio(VC_BIKKURI, VC_BIKKURI_PATH);
			registerAudio(VC_BUBUU, VC_BUBUU_PATH);
			registerAudio(VC_HAKKYOU, VC_HAKKYOU_PATH);
			registerAudio(VC_HOSO1, VC_HOSO1_PATH);
			registerAudio(VC_HOSO2, VC_HOSO2_PATH);
			registerAudio(VC_HOSO3, VC_HOSO3_PATH);
			registerAudio(VC_PON, VC_PON_PATH);
		}

		/**
		 * @brief オーディオを登録する
		 * @param name オーディオ名
		 * @param path オーディオファイルのパス
		 * @param loop ループ設定
		 */
		void registerAudio(const String& name, const String& path, const Loop loop) {
			registry << SoundRegistry{ name, path, loop };
		}

		
		 // オーディオを同期的に登録する
		void registerAudiosSync() {
			for (auto& r : registry) {
				AudioAsset::Register(r.name, r.path, r.loop);
			}
		}

		
		// オーディオを同期的にロードする
		void loadAudiosSync() {
			for (auto& r : registry) {
				AudioAsset::Load(r.name);
			}
		}
	}
}
