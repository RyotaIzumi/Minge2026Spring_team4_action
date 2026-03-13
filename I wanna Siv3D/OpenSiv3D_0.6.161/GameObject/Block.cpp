#include "Block.h"

namespace Iwanna {
	Block::Block(String name, Vec2 startPos) {
		textureName = name;
		//GameObject.hの値初期化
		pos = {startPos.x * side, startPos.y * side};
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Block;
		blockType = BlockType::Normal;
		canPlayerKill = false;
	}
	void Block::update() {
	}
	void Block::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(textureName).draw(pos);
	}
	void Block::onCollision(GameObject& other) {
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

	// ----- 弾のみ通過ブロック ----- //
	ShootTroughBlock::ShootTroughBlock(String name, Vec2 startPos) : Block(name, startPos) {
		textureName = name;
		//GameObject.hの値初期化
		pos = { startPos.x * side, startPos.y * side };
		hitBox = std::make_shared<RectHitBox>(pos, SizeF{ side,side });
		type = ObjectType::Block;
		blockType = BlockType::ShootThrough;
		canPlayerKill = false;
	}

	void ShootTroughBlock::draw() const {
		//hitBox->draw(Palette::Gray);
		TextureAsset(textureName).draw(pos);
	}

	void ShootTroughBlock::onCollision(GameObject& other) {
	}
}
