#include "SpriteAsset.h"

namespace Iwanna {
	Array<std::pair<String, String>> registry;

	void registerTexture(const String& name, const String& path) {
		registry << std::pair<String, String>(name, path);
	}

	void registerTextures() {

		//kid君のテクスチャ登録
		static const FilePath PlayerPath = U"Texture/Player/";
		registerTexture(U"sprPlayerIdle", PlayerPath + U"sprPlayerIdle.png");
		registerTexture(U"sprPlayerFall", PlayerPath + U"sprPlayerFall.png");
		registerTexture(U"sprPlayerJump", PlayerPath + U"sprPlayerJump.png");
		registerTexture(U"sprPlayerRunning", PlayerPath + U"sprPlayerRunning.png");
		registerTexture(U"sprBullet", PlayerPath + U"bullet.png");

		registerTexture(U"sprIdle_normal", PlayerPath + U"sprIdle_normal.png");
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
		registerTexture(U"sprSparkCherry", CherryPath + U"sparkCherry.png");

		//ブロックテクスチャ登録
		static const FilePath BlockPath = U"Texture/Block/";
		registerTexture(U"sprBlock_low1", BlockPath + U"sprBlock_low1.png");
		registerTexture(U"sprBlock_low2", BlockPath + U"sprBlock_low2.png");
		registerTexture(U"sprBlock_low3", BlockPath + U"sprBlock_low3.png");
		registerTexture(U"sprWater", BlockPath + U"sprWater.png");
		registerTexture(U"sprBlockShootTrough", BlockPath + U"sprBlockShootTrough.png");

		registerTexture(U"sprBlock_secret1", BlockPath + U"sprBlock_secret1.png");
		registerTexture(U"sprBlock_secret2", BlockPath + U"sprBlock_secret2.png");
		registerTexture(U"sprBlock_secret3", BlockPath + U"sprBlock_secret3.png");

		//mikuテクスチャ登録
		static const FilePath MikuPath = U"Texture/Miku/";
		registerTexture(U"sprMiku", MikuPath + U"sprMiku.png");

		//針テクスチャ登録
		static const FilePath SpikePath = U"Texture/Spike/";
		registerTexture(U"sprSpikeUp_low", SpikePath + U"sprSpikeUp_low.png");
		registerTexture(U"sprSpikeLeft_low", SpikePath + U"sprSpikeLeft_low.png");
		registerTexture(U"sprSpikeDown_low", SpikePath + U"sprSpikeDown_low.png");
		registerTexture(U"sprSpikeRight_low", SpikePath + U"sprSpikeRight_low.png");

		registerTexture(U"sprSpikeUp_secret", SpikePath + U"sprSpikeUp_secret.png");
		registerTexture(U"sprSpikeLeft_secret", SpikePath + U"sprSpikeLeft_secret.png");
		registerTexture(U"sprSpikeDown_secret", SpikePath + U"sprSpikeDown_secret.png");
		registerTexture(U"sprSpikeRight_secret", SpikePath + U"sprSpikeRight_secret.png");

		//背景テクスチャ登録
		static const FilePath BackgroundPath = U"Texture/Background/";
		registerTexture(U"background_sample", BackgroundPath + U"sample.png");
		registerTexture(U"background_black", BackgroundPath + U"black.png");

		//その他テクスチャ登録
		static const FilePath OtherPath = U"Texture/Other/";
		registerTexture(U"sprSave", OtherPath + U"sprSave.png");
		registerTexture(U"sprWarp", OtherPath + U"sprWarp.png");
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
		registerTexture(U"hideLine", OtherPath + U"hideLine.png");
		registerTexture(U"sprSign", OtherPath + U"sprSign.png");

		registerTexture(U"sprTitleCard_boss", OtherPath + U"sprTitleCard_boss.png");
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
