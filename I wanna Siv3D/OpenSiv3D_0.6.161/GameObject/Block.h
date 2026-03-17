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
		ConditionalHide,
		Fake,
		ShootThrough,
		Break
	};

	class Block : public GameObject {
	protected:
		int32 side = 32;
		String textureName = U"sprBlock";
		bool hasCollide = true;
	public:
		Block(String name, Vec2 startPos);
		BlockType blockType = BlockType::Normal;

		bool isTriggerTrap = false;//トリガーによる罠用かどうか

		void update() override;
		virtual void trapUpdate(int32 id);
		void draw() const override;
		void onCollision(GameObject& other) override;

		void setHasCollide(bool b);
		bool getHasCollide() const;
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

	class ConditionalHideBlock : public HideBlock {
	private:
		int32 triggerID = 0;
	public:
		ConditionalHideBlock(String name, Vec2 startPos, int32 id);
		void trapUpdate(int32 id) override;
	};

	class FakeBlock : public Block {
	private:
		bool isHidden = true;
	public:
		FakeBlock(String name, Vec2 startPos);
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

	class BreakBlock : public Block {
	private:
		bool isBreak;
		int32 triggerID = 0;

		double blockAlpha = 1.0;
		double blockDir = 0;
		double blockGravity = 0.9;
	public:
		BreakBlock(String name, Vec2 startPos, int32 id);
		void update() override;
		void trapUpdate(int32 id) override;
		void draw() const override;
		void onCollision(GameObject& other) override;
		bool getIsBreak() const;
		void setIsBreak(bool hidden);
		int32 getID();
	};
}
