#include "SpriteAsset.h"

namespace Iwanna {
	Array<std::pair<String, String>> registry;

	void registerTexture(const String& name, const String& path) {
		registry << std::pair<String, String>(name, Resource(path));
	}

	void registerTextures() {

		//kid君のテクスチャ登録
		static const FilePath PlayerPath = U"Texture/Player/";
		registerTexture(U"sprPlayerIdle", PlayerPath + U"sprPlayerIdle.png");
		registerTexture(U"sprPlayerFall", PlayerPath + U"sprPlayerFall.png");
		registerTexture(U"sprPlayerJump", PlayerPath + U"sprPlayerJump.png");
		registerTexture(U"sprPlayerRunning", PlayerPath + U"sprPlayerRunning.png");
		registerTexture(U"sprPlayer_low", PlayerPath + U"sprPlayer_low.png");
		registerTexture(U"sprBullet", PlayerPath + U"bullet.png");
		registerTexture(U"sprBullet2", PlayerPath + U"bullet2.png");

		registerTexture(U"sprIdle_normal", PlayerPath + U"sprIdle_normal.png");
		registerTexture(U"sprRunning_normal", PlayerPath + U"sprRunning_normal.png");
		registerTexture(U"sprFall_normal", PlayerPath + U"sprFall_normal.png");
		registerTexture(U"sprJump_normal", PlayerPath + U"sprJump_normal.png");

		//さくらんぼテクスチャ登録
		static const FilePath CherryPath = U"Texture/Cherry/";
		registerTexture(U"sprCherry", CherryPath + U"sprCherry.png");
		registerTexture(U"sprCherryAllWhite", CherryPath + U"sprCherryAllWhite.png");
		registerTexture(U"sprCherryFrameWhite", CherryPath + U"sprCherryFrameWhite.png");
		registerTexture(U"sprCherryWhite", CherryPath + U"sprCherryWhite.png");
		registerTexture(U"sprCherryLow", CherryPath + U"sprCherryLow.png");
		registerTexture(U"sprCherryLowWhite", CherryPath + U"sprCherryLowWhite.png");
		registerTexture(U"sprCherryLowWhiteLine", CherryPath + U"sprCherryLowWhiteLine.png");
		registerTexture(U"sprCherryLowBarrageWhite", CherryPath + U"sprCherryLowBarrageWhite.png");
		registerTexture(U"sprCherryLowAllWhite", CherryPath + U"sprCherryLowAllWhite.png");
		registerTexture(U"sprCherryLowBoss", CherryPath + U"sprCherryLowBoss.png");
		registerTexture(U"sprCherryTrap", CherryPath + U"sprCherryTrap.png");
		registerTexture(U"sprCherryTrapBoss", CherryPath + U"sprCherryTrapBoss.png");
		registerTexture(U"sprSparkCherry", CherryPath + U"sparkCherry.png");

		//ブロックテクスチャ登録
		static const FilePath BlockPath = U"Texture/Block/";
		registerTexture(U"sprBlock_low1", BlockPath + U"sprBlock_low1.png");
		registerTexture(U"sprBlock_low2", BlockPath + U"sprBlock_low2.png");
		registerTexture(U"sprBlock_low3", BlockPath + U"sprBlock_low3.png");

		registerTexture(U"sprBlock_normal1", BlockPath + U"sprBlock_normal1.png");
		registerTexture(U"sprBlock_normal2", BlockPath + U"sprBlock_normal2.png");
		registerTexture(U"sprBlock_normal3", BlockPath + U"sprBlock_normal3.png");


		registerTexture(U"sprWater", BlockPath + U"sprWater.png");
		registerTexture(U"sprBlockShootTrough", BlockPath + U"sprBlockShootTrough.png");

		registerTexture(U"sprBlock_secret1", BlockPath + U"sprBlock_secret1.png");
		registerTexture(U"sprBlock_secret2", BlockPath + U"sprBlock_secret2.png");
		registerTexture(U"sprBlock_secret3", BlockPath + U"sprBlock_secret3.png");
		registerTexture(U"sprBlock_extra1", BlockPath + U"sprBlock_extra1.png");
		registerTexture(U"sprBlock_extra2", BlockPath + U"sprBlock_extra2.png");
		registerTexture(U"sprBlock_extra3", BlockPath + U"sprBlock_extra3.png");
		registerTexture(U"sprBlock_extra4", BlockPath + U"sprBlock_extra4.png");
		registerTexture(U"sprBlock_haibokusya", BlockPath + U"sprBlock_haibokusya.jpg");

		//mikuテクスチャ登録
		static const FilePath MikuPath = U"Texture/Miku/";
		registerTexture(U"sprMiku", MikuPath + U"sprMiku.png");

		//針テクスチャ登録
		static const FilePath SpikePath = U"Texture/Spike/";
		registerTexture(U"sprSpikeUp_low", SpikePath + U"sprSpikeUp_low.png");
		registerTexture(U"sprSpikeLeft_low", SpikePath + U"sprSpikeLeft_low.png");
		registerTexture(U"sprSpikeDown_low", SpikePath + U"sprSpikeDown_low.png");
		registerTexture(U"sprSpikeRight_low", SpikePath + U"sprSpikeRight_low.png");

		registerTexture(U"sprSpikeUp_normal", SpikePath + U"sprSpikeUp_normal.png");
		registerTexture(U"sprSpikeLeft_normal", SpikePath + U"sprSpikeLeft_normal.png");
		registerTexture(U"sprSpikeDown_normal", SpikePath + U"sprSpikeDown_normal.png");
		registerTexture(U"sprSpikeRight_normal", SpikePath + U"sprSpikeRight_normal.png");

		registerTexture(U"sprSpikeUp_secret", SpikePath + U"sprSpikeUp_secret.png");
		registerTexture(U"sprSpikeLeft_secret", SpikePath + U"sprSpikeLeft_secret.png");
		registerTexture(U"sprSpikeDown_secret", SpikePath + U"sprSpikeDown_secret.png");
		registerTexture(U"sprSpikeRight_secret", SpikePath + U"sprSpikeRight_secret.png");

		registerTexture(U"sprSpikeUp_trap", SpikePath + U"sprSpikeUp_trap.png");
		registerTexture(U"sprSpikeLeft_trap", SpikePath + U"sprSpikeLeft_trap.png");
		registerTexture(U"sprSpikeDown_trap", SpikePath + U"sprSpikeDown_trap.png");
		registerTexture(U"sprSpikeRight_trap", SpikePath + U"sprSpikeRight_trap.png");
		registerTexture(U"sprSpikeExtra", SpikePath + U"sprSpikeExtra.png");

		//背景テクスチャ登録
		static const FilePath BackgroundPath = U"Texture/Background/";
		registerTexture(U"background_sample", BackgroundPath + U"sample.png");
		registerTexture(U"background_black", BackgroundPath + U"black.png");
		registerTexture(U"background_cave", BackgroundPath + U"cave.png");
		registerTexture(U"background_caveLong", BackgroundPath + U"caveLong.png");
		registerTexture(U"background_trapBossCave1", BackgroundPath + U"trapBossCave1.png");
		registerTexture(U"background_trapBossCave2", BackgroundPath + U"trapBossCave2.png");

		//その他テクスチャ登録
		static const FilePath OtherPath = U"Texture/Other/";
		registerTexture(U"sprSave", OtherPath + U"sprSave.png");
		registerTexture(U"sprSave_low", OtherPath + U"sprSave_low.png");
		registerTexture(U"sprSave_normal", OtherPath + U"sprSave_normal.png");
		registerTexture(U"sprWarp", OtherPath + U"sprWarp.png");
		registerTexture(U"sprWarp_low", OtherPath + U"sprWarp_low.png");
		registerTexture(U"sprWarp_normal", OtherPath + U"sprWarp_normal.png");
		registerTexture(U"sprSecretWarp", OtherPath + U"sprSecretWarp.png");
		registerTexture(U"sprFakeSave", OtherPath + U"sprFakeSave.png");
		registerTexture(U"warningTrap", OtherPath + U"warningTrap.png");
		registerTexture(U"steamTrap", OtherPath + U"steamTrap.png");
		registerTexture(U"discordTrap", OtherPath + U"discordTrap.png");
		registerTexture(U"adTrap", OtherPath + U"adTrap.png");
		registerTexture(U"panddChanTrap", OtherPath + U"panddChan.png");
		registerTexture(U"treeTrap", OtherPath + U"tree.png");
		registerTexture(U"transParentTreeTrap", OtherPath + U"transParentTree.png");
		registerTexture(U"sprGAMEOVER", OtherPath + U"sprGAMEOVER.png");
		registerTexture(U"sprGAMEOVER_low", OtherPath + U"sprGAMEOVER_low.png");
		registerTexture(U"sprGAMEOVER_normal", OtherPath + U"sprGAMEOVER_normal.png");
		registerTexture(U"hideLine", OtherPath + U"hideLine.png");
		registerTexture(U"sprSign", OtherPath + U"sprSign.png");
		registerTexture(U"sprSign_low", OtherPath + U"sprSign_low.png");
		registerTexture(U"sprSign_normal", OtherPath + U"sprSign_normal.png");
		registerTexture(U"heart", OtherPath + U"heart.png");
		registerTexture(U"item1", OtherPath + U"item1.png");
		registerTexture(U"item2", OtherPath + U"item2.png");
		registerTexture(U"batu", OtherPath + U"batu.png");
		registerTexture(U"achieveFrame", OtherPath + U"achieveFrame.png");
		registerTexture(U"tayama", OtherPath + U"tayama.png");
		registerTexture(U"sprExtraLogo", OtherPath + U"sprExtraLogo.png");
		registerTexture(U"Ryuta_hide", OtherPath + U"ryuta_hide.png");
		registerTexture(U"sprNameBar_low", OtherPath + U"sprNameBar_low.png");
		registerTexture(U"sprNameBar_normal", OtherPath + U"sprNameBar_normal.png");
		registerTexture(U"sprNameBar_trap", OtherPath + U"sprNameBar_trap.png");

		registerTexture(U"sprTitleCard_boss", OtherPath + U"sprTitleCard_boss.png");
		registerTexture(U"sprTitleCard_exboss", OtherPath + U"sprTitleCard_exboss.png");
		registerTexture(U"sprTitleCard_secret", OtherPath + U"sprTitleCard_secret.png");
		registerTexture(U"sprTitleCard_milu", OtherPath + U"sprTitleCard_milu.png");
		registerTexture(U"sprTitleCard_mochi", OtherPath + U"sprTitleCard_mochi.png");
		registerTexture(U"sprTitleCard_got", OtherPath + U"sprTitleCard_got.png");
		registerTexture(U"sprTitleCard_ryuta", OtherPath + U"sprTitleCard_ryuta.png");
		registerTexture(U"sprTitleCard_reno", OtherPath + U"sprTitleCard_reno.png");
	}

	void registerTexturesSync() {
		for (auto& p : registry) {
			TextureAsset::Register(p.first, p.second);
		}
	}

	void loadTexturesSync() {
		for (auto& p : registry) {
			TextureAsset::Load(p.first);
		}
	}
}
