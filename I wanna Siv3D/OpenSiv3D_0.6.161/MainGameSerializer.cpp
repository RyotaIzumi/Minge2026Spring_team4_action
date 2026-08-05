#include "MainGameSerializer.h"

MainGameSerializer::MainGameSerializer() {
}

MainGameSerializer::~MainGameSerializer() {
}

void MainGameSerializer::LoadCharactersMoraleValue() {
	const JSON json = JSON::Load(U"CharactersMoraleValue.json");

	if (!json) {
		throw Error{ U"キャラクター士気度JSONファイルをロードできませんでした" };
	}

	moraleValue1 = json[U"MoraleValue1"].get<int32>();
	moraleValue2 = json[U"MoraleValue2"].get<int32>();
	moraleValue3 = json[U"MoraleValue3"].get<int32>();
	moraleValue4 = json[U"MoraleValue4"].get<int32>();
	Global::moraleValue1 = moraleValue1;
	Global::moraleValue2 = moraleValue2;
	Global::moraleValue3 = moraleValue3;
	Global::moraleValue4 = moraleValue4;
	Global::getItem1 = json.contains(U"GetItem1")
		? json[U"GetItem1"].get<bool>()
		: false;
	Global::getItem2 = json.contains(U"GetItem2")
		? json[U"GetItem2"].get<bool>()
		: false;
}

void MainGameSerializer::LoadGameSettings() {
	const JSON json = JSON::Load(U"GameSettings.json");

	if (!json) {
		Global::soundVolume = 1.0;
		return;
	}

	Global::soundVolume = json.contains(U"SoundVolume")
		? Clamp(json[U"SoundVolume"].get<double>(), 0.0, 1.0)
		: 1.0;
}

void MainGameSerializer::LoadEndingValue() {
	const JSON json = JSON::Load(U"EndingValue.json");

	if (!json) {
		Global::endingValue = 0;
		return;
	}

	Global::endingValue = json.contains(U"EndingValue")
		? Clamp(json[U"EndingValue"].get<int32>(), 0, 9)
		: 0;
}

void MainGameSerializer::defineGlobalStatuses() {
	Global::endingValue = 4;

	// 開始room
	if (moraleValue1 >= 101 && moraleValue2 >= 101 && moraleValue3 >= 101 && moraleValue4 >= 101) {
		Global::startRoomName = U"ExMiluArea";
	}
	else if (moraleValue2 >= 90 && moraleValue3 >= 90 && moraleValue4 >= 90) {
		Global::startRoomName = U"tutorialTrap";
	}
	else if (moraleValue1 < 30 && moraleValue2 < 30) {
		Global::startRoomName = U"tutorialLow";
	}
	else if (moraleValue1 >= 90 && moraleValue1 <= 100) {
		Global::remainingGenerateStageNames.clear();
		Global::startRoomName = U"tutorialLow";
		Global::endingValue = 3;
	}
	else if (moraleValue2 > 90 && moraleValue2 <= 100)Global::startRoomName = U"trapBoss";
	else if(moraleValue1 <= 30) Global::startRoomName = U"normal1";
	else Global::startRoomName = U"normal2";

	// ゲームタイトル
	if (moraleValue2 > 90 && moraleValue2 <= 100) Window::SetTitle(U"TestPlayGame (Debug Build)");
	else Window::SetTitle(U"I gonna be the treasure hunter");

	//Texture
	if (moraleValue1 >= 101 && moraleValue2 >= 101 && moraleValue3 >= 101 && moraleValue4 >= 101) Global::mainTextureNumber = 2;
	else if (moraleValue3 < 30) Global::mainTextureNumber = 0;
	else Global::mainTextureNumber = 1;

	//BGM
	if (moraleValue4 < 30) Global::mainBgmNumber = 0;
	else if (moraleValue4 < 70) Global::mainBgmNumber = 1;
	else if (moraleValue4 <= 100) Global::mainBgmNumber = 2;
}

void MainGameSerializer::SaveCharactersMoraleValue() {
	JSON json;

	json[U"MoraleValue1"] = Global::moraleValue1;
	json[U"MoraleValue2"] = Global::moraleValue2;
	json[U"MoraleValue3"] = Global::moraleValue3;
	json[U"MoraleValue4"] = Global::moraleValue4;
	json[U"GetItem1"] = Global::getItem1;
	json[U"GetItem2"] = Global::getItem2;

	json.save(U"CharactersMoraleValue.json");
}

void MainGameSerializer::SaveGameSettings() {
	JSON json;

	json[U"SoundVolume"] = Clamp(Global::soundVolume, 0.0, 1.0);

	json.save(U"GameSettings.json");
}

// エンディング種類値を保存する処理をここに実装
void MainGameSerializer::SaveEndingValue() {
	JSON json;

	// 値をセット
	json[U"EndingValue"] = Global::endingValue;

	// ファイル保存
	json.save(U"EndingValue.json");
}
