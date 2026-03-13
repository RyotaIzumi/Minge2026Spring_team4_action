#pragma once
#include <Siv3D.hpp>
#include "HitBox.h"
#include "../Sprite/SpriteSystem.h"
#include "../Global.h"
#include "GameObject.h"

namespace Iwanna {
	enum class BlockType {
		Normal,
		Hide,
		ShootThrough
	};

	class Block : public GameObject {
	protected:
		int32 side = 32;
		String textureName = U"sprBlock";
	public:
		Block(String name, Vec2 startPos);
		BlockType blockType = BlockType::Normal;

		void update() override;
		void draw() const override;
		void onCollision(GameObject& other) override;
	};

	class HideBlock : public Block {
	private:
		bool isHidden = true;
	public:
		HideBlock(String name, Vec2 startPos);
		void draw() const override;
		void onCollision(GameObject& other) override;
		bool getIsHidden() const;
		void setIsHidden(bool hidden);
	};

	class ShootTroughBlock : public Block {
	private:
	public:
		ShootTroughBlock(String name, Vec2 startPos);
		void draw() const override;
		void onCollision(GameObject& other) override;
	};
}
