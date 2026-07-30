#include "MainGameSerializer.h"

MainGameSerializer::MainGameSerializer() {
}

MainGameSerializer::~MainGameSerializer() {
}

void MainGameSerializer::LoadCharactersMoraleValue() {
	const JSON json = JSON::Load(U"CharactersMoraleValue.json");

	if (!json) {
		std::cout << "Failed to load characters morale value" << std::endl;
		std::cout << "Now using default values" << std::endl;

		moraleValue1 = 10;
		moraleValue2 = 10;
		moraleValue3 = 10;
		moraleValue4 = 10;
	} else {
		moraleValue1 = json[U"MoraleValue1"].get<int32>();
		moraleValue2 = json[U"MoraleValue2"].get<int32>();
		moraleValue3 = json[U"MoraleValue3"].get<int32>();
		moraleValue4 = json[U"MoraleValue4"].get<int32>();
	}
}

void MainGameSerializer::defineGlobalStatuses() {

	// 開始room
	if (moraleValue2 > 90)Global::startRoomName = U"trap1";
	else if(moraleValue1 <= 30) Global::startRoomName = U"normal1";
	else Global::startRoomName = U"tutorial";

	// ゲームタイトル
	if (moraleValue2 > 90) Window::SetTitle(U"I wanna Siv3D (Debug Build)");
	else Window::SetTitle(U"I wanna continue Siv3D");

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
