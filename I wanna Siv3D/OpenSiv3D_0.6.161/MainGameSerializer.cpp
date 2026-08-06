#include "MainGameSerializer.h"

namespace {
	FilePath GetGameSaveFilePath() {
		wchar_t* userProfilePath = nullptr;
		size_t userProfilePathLength = 0;
		FilePath basePath;

		if (_wdupenv_s(&userProfilePath, &userProfilePathLength, L"USERPROFILE") == 0
			&& userProfilePath != nullptr) {
			basePath = Unicode::FromWstring(userProfilePath);
			free(userProfilePath);
		}
		else {
			const FilePath localAppData = FileSystem::GetFolderPath(SpecialFolder::LocalAppData);
			basePath = FileSystem::ParentPath(FileSystem::ParentPath(localAppData));
		}

		return basePath
			+ U"/AppData/LocalLow/PandD_org/【PandD Game Dev Club】Our Chaotic Developers Refuse to Stick to the Plan/GameSave.json";
	}

	String GetCurrentEndingId() {
		const int32 endingIndex = Clamp(Global::endingValue, 0, 10);
		return U"Ending_" + String{ static_cast<char32>(U'A' + endingIndex) };
	}

	Array<String> ReadStringArray(const JSON& json, const String& key) {
		Array<String> values;

		if (!json.contains(key)) {
			return values;
		}

		for (const auto& value : json[key].arrayView()) {
			values << value.getString();
		}

		return values;
	}
}

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
		Global::isFullscreen = false;
		return;
	}

	Global::soundVolume = json.contains(U"SoundVolume")
		? Clamp(json[U"SoundVolume"].get<double>(), 0.0, 1.0)
		: 1.0;
	Global::isFullscreen = json.contains(U"Fullscreen")
		? json[U"Fullscreen"].get<bool>()
		: false;
}

void MainGameSerializer::LoadEndingValue() {
	const JSON json = JSON::Load(U"EndingValue.json");

	if (!json) {
		Global::endingValue = 0;
		return;
	}

	Global::endingValue = json.contains(U"EndingValue")
		? Clamp(json[U"EndingValue"].get<int32>(), 0, 10)
		: 0;
}

void MainGameSerializer::defineGlobalStatuses() {
	Global::endingValue = Global::getInitialEndingValue();

	// 開始room
	if (Global::isNoMoraleEndingRoute()) {
		Global::startRoomName = U"tutorialLow";
	}
	else if (Global::isEndingKRoute()) {
		Global::startRoomName = U"ExMiluArea";
	}
	else if (Global::isEndingGRoute()) {
		Global::startRoomName = U"tutorialTrap";
	}
	else if (Global::isEndingBRoute()) {
		Global::startRoomName = U"tutorialLow";
	}
	else if (Global::isEndingDRoute()) {
		Global::remainingGenerateStageNames.clear();
		Global::startRoomName = U"tutorialLow";
	}
	else if(moraleValue1 <= 30) Global::startRoomName = U"normal1";
	else Global::startRoomName = U"tutorial";

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
	json[U"Fullscreen"] = Global::isFullscreen;

	json.save(U"GameSettings.json");
}

void MainGameSerializer::SaveEndingClearRecord() {
	const FilePath savePath = GetGameSaveFilePath();
	FileSystem::CreateDirectories(FileSystem::ParentPath(savePath));

	JSON json = JSON::Load(savePath);
	if (!json) {
		json = JSON{};
	}

	const String endingId = GetCurrentEndingId();
	Array<String> reachedEndingIds = ReadStringArray(json, U"reachedEndingIds");
	if (not reachedEndingIds.includes(endingId)) {
		reachedEndingIds << endingId;
	}

	json[U"chapterId"] = U"Chapter";
	json[U"currentBlockId"] = U"";
	json[U"reachedBlockIds"] = ReadStringArray(json, U"reachedBlockIds");
	json[U"reachedEndingIds"] = reachedEndingIds;

	const int32 clearTime = Max(0, static_cast<int32>(Floor(Global::elapsedPlayTime)));
	const int32 deathCount = Max(0, Global::deathCount);
	bool shouldUpdateRecord = true;

	if (json.contains(U"eachEndingClearTime")
		&& json[U"eachEndingClearTime"].contains(endingId)) {
		const int32 savedClearTime = json[U"eachEndingClearTime"][endingId].get<int32>();
		shouldUpdateRecord = (savedClearTime <= 0 || clearTime < savedClearTime);
	}

	if (shouldUpdateRecord) {
		json[U"eachEndingClearTime"][endingId] = clearTime;
		json[U"eachEndingDeathCount"][endingId] = deathCount;
	}

	json.save(savePath);
}

// エンディング種類値を保存する処理をここに実装
void MainGameSerializer::SaveEndingValue() {
	JSON json;

	// 値をセット
	json[U"EndingValue"] = Global::endingValue;

	// ファイル保存
	json.save(U"EndingValue.json");
}
