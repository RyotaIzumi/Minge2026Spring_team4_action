#pragma once
#include <Siv3D.hpp>
#include "Global.h"

class MainGameSerializer {
private:
	int32 moraleValue1;
	int32 moraleValue2;
	int32 moraleValue3;
	int32 moraleValue4;
public:
    MainGameSerializer();
    ~MainGameSerializer();

	void LoadCharactersMoraleValue();
	void LoadGameSettings();
	void LoadEndingValue();
	void defineGlobalStatuses();
	void SaveCharactersMoraleValue();
	void SaveGameSettings();
    void SaveEndingValue();
};
