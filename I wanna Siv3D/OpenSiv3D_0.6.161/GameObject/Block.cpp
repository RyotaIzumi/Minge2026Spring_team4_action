#include "Block.h"
#include "../Audio/AudioAsset.h"

namespace Iwanna {
	Block::Block(String name, Vec2 startPos) {
		textureName = name;
		//GameObject.hの値初期化
		pos = {startPos.x * side, startPos.y * side};
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Block;
		blockType = BlockType::Normal;

		canPlayerKill = false;
		hasCollide = true;
	}
	void Block::update() {
	}
	void Block::trapUpdate(int32 id) {
	}
	void Block::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(textureName).draw(pos);
	}
	void Block::onCollision(GameObject& other) {
	}

	//当たり判定の有無を設定
	void Block::setHasCollide(bool b) {
		hasCollide = b;
	}
	//当たり判定の有無を返す
	bool Block::getHasCollide() const {
		return hasCollide;
	}

	// ----- 隠しブロック ----- //
	HideBlock::HideBlock(String name, Vec2 startPos) : Block(name, startPos) {
		textureName = name;
		//GameObject.hの値初期化
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Block;
		blockType = BlockType::Hide;

		canPlayerKill = false;
		hasCollide = true;
	}

	void HideBlock::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(textureName).draw(pos,ColorF(1.0,isHidden ? 0.0 : 1.0));
	}

	void HideBlock::onCollision(GameObject& other) {
	}

	bool HideBlock::getIsHidden() const {
		return isHidden;
	}

	void HideBlock::setIsHidden(bool hidden) {
		isHidden = hidden;
	}

	// ----- 罠IDにより起動する隠しブロック -----
	ConditionalHideBlock::ConditionalHideBlock(String name, Vec2 startPos, int32 id) : HideBlock(name, startPos) {
		textureName = name;
		triggerID = id;
		//GameObject.hの値初期化
		pos = startPos;
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });

		type = ObjectType::Block;
		blockType = BlockType::ConditionalHide;

		isTriggerTrap = true;
		setIsHidden(true);
		canPlayerKill = false;
		hasCollide = false;
	}

	void ConditionalHideBlock::trapUpdate(int32 id) {
		if (triggerID == id) {
			hasCollide = true;
		}
	}

	// ----- 偽ブロック ----- //
	FakeBlock::FakeBlock(String name, Vec2 startPos) : Block(name, startPos) {
		textureName = name;
		//GameObject.hの値初期化
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Block;
		blockType = BlockType::Fake;

		isHidden = false;
		canPlayerKill = false;
		hasCollide = false;
	}

	void FakeBlock::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(textureName).draw(pos, ColorF(1.0, isHidden ? 0.0 : 1.0));
	}

	void FakeBlock::onCollision(GameObject& other) {
	}

	bool FakeBlock::getIsHidden() const {
		return isHidden;
	}

	void FakeBlock::setIsHidden(bool hidden) {
		isHidden = hidden;
	}

	// ----- 弾のみ通過ブロック ----- //
	ShootTroughBlock::ShootTroughBlock(String name, Vec2 startPos) : Block(name, startPos) {
		textureName = name;
		//GameObject.hの値初期化
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Block;
		blockType = BlockType::ShootThrough;

		canPlayerKill = false;
		hasCollide = true;
	}

	void ShootTroughBlock::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(textureName).draw(pos);
	}

	void ShootTroughBlock::onCollision(GameObject& other) {
	}

	// ----- ギミックなどにより壊れるブロック ----- //
	BreakBlock::BreakBlock(String name, Vec2 startPos, int32 id) : Block(name, startPos) {
		textureName = name;
		triggerID = id;
		//GameObject.hの値初期化
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Block;
		blockType = BlockType::Break;

		isBreak = false;
		isTriggerTrap = true;
		canPlayerKill = false;
		hasCollide = true;

		//壊れる際の挙動の乱数決定
		direction = Random(360);
		speed = 1 + Random(5);
		calculateSpeed();
	}

	void BreakBlock::update() {
	}

	void BreakBlock::trapUpdate(int32 id) {
		if (triggerID == id && !isBreak) {
			AudioAsset(Sound::BLOCKBREAK).play();
			isBreak = true;
		}

		if (isBreak) {
			hasCollide = false;

			vspeed += blockGravity;
			pos.x += hspeed;
			pos.y += vspeed;

			blockAlpha -= 0.1;
			if (blockAlpha < 0) {
				isDelete = true;
			}
		}
	}

	void BreakBlock::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(textureName).draw(pos, ColorF(1.0, blockAlpha));
	}

	void BreakBlock::onCollision(GameObject& other) {
	}

	bool BreakBlock::getIsBreak() const {
		return isBreak;
	}

	void BreakBlock::setIsBreak(bool hidden) {
		isBreak = hidden;
	}

	int32 BreakBlock::getID() {
		return triggerID;
	}

	// ----- 水ブロック ----- //
	WaterBlock::WaterBlock(String name, Vec2 startPos) : Block(name, startPos) {
		blockType = BlockType::Water;

		canPlayerKill = false;
		hasCollide = false;
	}
}
