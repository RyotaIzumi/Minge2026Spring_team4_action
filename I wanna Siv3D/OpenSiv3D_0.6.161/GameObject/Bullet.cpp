#include "Bullet.h"
#include "../Audio/AudioAsset.h"
#include "../GameObject/SavePoint.h"
#include "../GameObject/Block.h"
#include "../GameObject/Cherry.h"
#include "../GameObject/Spike.h"
#include "../GameObject/Boss/ExBossCherry.h"

namespace Iwanna {
	Bullet::Bullet(Vec2& genePos, double hs, Player* owner) : ownerPlayer(owner) {

		//GameObject.hの値初期化
		pos = genePos;
		prevPos = pos;
		hitBox = std::make_shared<CircleHitBox>(pos, hitBoxSize);
		type = ObjectType::Bullet;
		canPlayerKill = false;
		isDelete = false;
		isOutOfScreen = false;

		hspeed = hs;
		vspeed = 0;

		depth = 42;
	}

	void Bullet::update() {
		checkOutOfScreen();
		// 位置更新
		prevPos = pos;
		pos.x += hspeed;
		pos.y += vspeed;
		
		// 当たり判定位置更新
		hitBox->setPos(pos);
	}

	void Bullet::draw() const {
		TextureAsset(Global::canUseItem1Effect() ? U"sprBullet2" : U"sprBullet").drawAt(pos.x, pos.y);
		//hitBox->draw(Palette::Blue);//判定の可視化
	}

	void Bullet::onCollision(GameObject& other) {
		// ブロック、ミク衝突
		if (this->intersects(other) && other.type == ObjectType::Block) {
			auto* block = dynamic_cast<Block*>(&other);
			if (block->blockType != BlockType::ShootThrough && block->getHasCollide()) {
				isDelete = true;
			}
		}

		// ミク衝突
		if (this->intersects(other) && other.type == ObjectType::Miku) {
			isDelete = true;
		}

		// item1取得後は針を破壊できる
		if (!isDelete && Global::canUseItem1Effect() && other.type == ObjectType::Spike) {
			auto* spike = dynamic_cast<Spike*>(&other);
			if (!spike->getIsDebris() && hitsSpike(*spike)) {
				spike->breakAsDebris();
				isDelete = true;
			}
		}

		// hpをもつりんご衝突
		if (this->intersects(other) && other.type == ObjectType::Cherry) {
			auto* cherry = dynamic_cast<Cherry*>(&other);
			if (cherry->getHasHp()) {
				const int32 damage = dynamic_cast<ExBossCherry*>(cherry) ? 1 : (Global::canUseItem1Effect() ? 2 : 1);
				if(!cherry->getIsMuteki()) cherry->hited(10);
				isDelete = true;
			}
		}

		// セーブポイント衝突
		if (this->intersects(other) && other.type == ObjectType::SavePoint) {
			if (ownerPlayer && ownerPlayer->getIsDead()) {
				return;
			}

			auto* savePoint = dynamic_cast<SavePoint*>(&other);
			switch (savePoint->saveType) {
			case SaveType::Normal:
			{
				savePoint->saved();
				break;
			}
			case SaveType::MoveTrap:
			{
				auto* smt = dynamic_cast<SaveMoveTrap*>(&other);
				if (!smt->getIsStartTrap()) smt->saved();
				break;
			}
			case SaveType::FakeTrap:
			{
				auto* sft = dynamic_cast<SaveFakeTrap*>(&other);
				if (!sft->getIsStartTrap()) sft->setIsStartTrap(true);
				break;
			}
			case SaveType::Boss:
			{
				auto* bsp = dynamic_cast<BossSavePoint*>(&other);
				bsp->saved();
				break;
			}
			case SaveType::Secret:
			{
				auto* ssp = dynamic_cast<SecretSavePoint*>(&other);
				if(!Global::isSecretTriggerActivated)ssp->saved();
				break;
			}
			}
		}
	}

	//画面外判定
	void Bullet::checkOutOfScreen() {
		const int32 excess = hitBoxSize * 2;//画面端からの余白
		if (pos.x < -1 * excess || pos.x > Global::stageWidth + excess ||
			pos.y < -1 * excess || pos.y > Global::stageHeight + excess) {
			isOutOfScreen = true;
		}
		else {
			isOutOfScreen = false;
		}
	}

	bool Bullet::hitsSpike(const Spike& spike) const {
		if (this->intersects(spike)) {
			return true;
		}

		const RectF sweptRect{
			Min(prevPos.x, pos.x) - hitBoxSize,
			Min(prevPos.y, pos.y) - hitBoxSize,
			Abs(pos.x - prevPos.x) + hitBoxSize * 2.0,
			Abs(pos.y - prevPos.y) + hitBoxSize * 2.0
		};

		return sweptRect.intersects(spike.getBroadRect());
	}
}
