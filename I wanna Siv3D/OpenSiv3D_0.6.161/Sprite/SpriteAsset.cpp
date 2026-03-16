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

		//さくらんぼテクスチャ登録
		static const FilePath CherryPath = U"Texture/Cherry/";
		registerTexture(U"sprCherry", CherryPath + U"sprCherry.png");
		registerTexture(U"sprCherryAllWhite", CherryPath + U"sprCherryAllWhite.png");
		registerTexture(U"sprCherryFrameWhite", CherryPath + U"sprCherryFrameWhite.png");
		registerTexture(U"sprCherryWhite", CherryPath + U"sprCherryWhite.png");
		registerTexture(U"sprCherryLow", CherryPath + U"sprCherryLow.png");

		//ブロックテクスチャ登録
		static const FilePath BlockPath = U"Texture/Block/";
		registerTexture(U"sprBlock_low1", BlockPath + U"sprBlock_low1.png");
		registerTexture(U"sprBlock_low2", BlockPath + U"sprBlock_low2.png");
		registerTexture(U"sprBlock_low3", BlockPath + U"sprBlock_low3.png");
		registerTexture(U"sprBlockShootTrough", BlockPath + U"sprBlockShootTrough.png");

		//mikuテクスチャ登録
		static const FilePath MikuPath = U"Texture/Miku/";
		registerTexture(U"sprMiku", MikuPath + U"sprMiku.png");

		//針テクスチャ登録
		static const FilePath SpikePath = U"Texture/Spike/";
		registerTexture(U"sprSpikeUp_low", SpikePath + U"sprSpikeUp_low.png");
		registerTexture(U"sprSpikeLeft_low", SpikePath + U"sprSpikeLeft_low.png");
		registerTexture(U"sprSpikeDown_low", SpikePath + U"sprSpikeDown_low.png");
		registerTexture(U"sprSpikeRight_low", SpikePath + U"sprSpikeRight_low.png");

		//その他テクスチャ登録
		static const FilePath OtherPath = U"Texture/Other/";
		registerTexture(U"sprSave", OtherPath + U"sprSave.png");
		registerTexture(U"warningTrap", OtherPath + U"warningTrap.png");
		registerTexture(U"steamTrap", OtherPath + U"steamTrap.png");
		registerTexture(U"adTrap", OtherPath + U"adTrap.png");
		registerTexture(U"treeTrap", OtherPath + U"tree.png");
		registerTexture(U"transParentTreeTrap", OtherPath + U"transParentTree.png");
		registerTexture(U"sprGAMEOVER", OtherPath + U"sprGAMEOVER.png");
		registerTexture(U"hideLine", OtherPath + U"hideLine.png");
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
