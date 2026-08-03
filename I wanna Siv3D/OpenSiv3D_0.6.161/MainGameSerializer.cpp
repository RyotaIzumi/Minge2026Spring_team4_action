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

	// 開始room
	if (moraleValue2 > 90)Global::startRoomName = U"tutorialTrap";
	else if(moraleValue1 <= 30) Global::startRoomName = U"normal1";
	else Global::startRoomName = U"tutorial";

	// ゲームタイトル
	if (moraleValue2 > 90) Window::SetTitle(U"TestPlayGame (Debug Build)");
	else Window::SetTitle(U"TestPlayGame");

	//Texture
	if (moraleValue3 < 40) Global::mainTextureNumber = 0;
	else if (moraleValue3 <= 100) Global::mainTextureNumber = 1;

	//BGM
	if (moraleValue4 < 30) Global::mainBgmNumber = 0;
	else if (moraleValue4 < 70) Global::mainBgmNumber = 1;
	else if (moraleValue4 <= 100) Global::mainBgmNumber = 2;
}

// エンディング種類値を保存する処理をここに実装
void MainGameSerializer::SaveEndingValue() {
	JSON json;

	// 値をセット
	json[U"EndingValue"] = 0;

	// ファイル保存
	json.save(U"EndingValue.json");
}
