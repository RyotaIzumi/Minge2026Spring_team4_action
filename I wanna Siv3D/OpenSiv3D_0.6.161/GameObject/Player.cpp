#include "Player.h"
#include "../Audio/AudioAsset.h"
#include "../GameObject/Trigger.h"
#include "../GameObject/Block.h"
#include "../GameObject/Warp.h"
#include "../GameObject/SavePoint.h"
#include "../GameObject/Sign.h"
#include "../GameObject/Item.h"

namespace Iwanna {
	Player::Player() {
		jump = 8.5; //１段目ジャンプ力
		jump2 = 7; //２段目ジャンプ力
		djump = true; //２段ジャンプできるかどうか
		maxSpeed = 3; //横方向速度の最大値
		gravity = 0.4; //重力の値
		maxVspeed = 9; //縦方向速度(主に落下速度)の最大値
		image_speed = 0.2; //アニメーション再生速度
		isMuteki = false; //無敵状態かどうか
		roomOutTrue = false;//kid君をroom外にいけるようにする
		isDead = false;//死亡状態かどうか
		isGenerateBullet = false;//弾生成フラグ
		isOutOfScreen = false;//画面外判定用フラグ

		//GameObject.hの値初期化
		pos = Vec2(-100, -100);
		hitBox = std::make_shared<RectHitBox>(pos, hitBoxSize);
		type = ObjectType::Player;
		canPlayerKill = false;

		//アニメーションデータの登録
		//(アクション名,フレーム数,各フレーム再生時間,ループするかどうか(省略可), 左右差分があるか(省略可))
		spriteSystem = SpriteSystem(32, 32);
		spriteSystem.addSprite(SpriteAction::PLAYER_WAIT, SpriteData(U"sprIdle_normal", 4, 0.15,true,false));
		spriteSystem.addSprite(SpriteAction::PLAYER_RUN, SpriteData(U"sprRunning_normal", 6, 0.1,true,false));
		spriteSystem.addSprite(SpriteAction::PLAYER_JUMP, SpriteData(U"sprJump_normal", 1, 0.1,true,false));
		spriteSystem.addSprite(SpriteAction::PLAYER_FALL, SpriteData(U"sprFall_normal", 1, 0.1,true,false));
		
		//アニメーションデータの登録
		//(アクション名,フレーム数,各フレーム再生時間,ループするかどうか(省略可), 左右差分があるか(省略可))
		/*
		spriteSystem = SpriteSystem(32, 32);
		spriteSystem.addSprite(SpriteAction::PLAYER_WAIT, SpriteData(U"sprPlayerIdle", 4, 0.15, true, false));
		spriteSystem.addSprite(SpriteAction::PLAYER_RUN, SpriteData(U"sprPlayerRunning", 4, 0.1, true, false));
		spriteSystem.addSprite(SpriteAction::PLAYER_JUMP, SpriteData(U"sprPlayerJump", 2, 0.1, true, false));
		spriteSystem.addSprite(SpriteAction::PLAYER_FALL, SpriteData(U"sprPlayerFall", 2, 0.1, true, false));
		*/

		//hp関連
		maxHp = 2;
		hp = maxHp;

		//初期の向き
		direction = Global::Direction::RIGHT;

		hspeed = 0.0;
		vspeed = 0.0;

		depth = 30;
	}

	void Player::update() {

		hspeed = 0.0;
		isChanedActionWait = false;

		//hp関連の処理
		if (mutekiInterval.reachedZero()) {
			isMuteki = false;
		}

		if (isDead) return;

		if (!Global::isPlayerFrozen) {
			if (Global::inputLeft.pressed()) playerMoveLeft();
			if (Global::inputRight.pressed()) playerMoveRight();
			if (Global::inputShoot.down()) playerShoot();
			if (Global::inputJump.down()) playerJump();
			if (Global::inputJump.up()) playerVJump();
		}

		// 重力反映
		gravity = 0.4;
		vspeed += gravity;
		if (Abs(vspeed) > maxVspeed) {
			vspeed = (vspeed > 0 ? 1 : -1) * maxVspeed;
		}

		// ジャンプ時アニメーション反映
		if (!isOnGround) {
			if (vspeed < -0.05) {
				spriteSystem.setSprite(SpriteAction::PLAYER_JUMP);
				isChanedActionWait = true;
			}
			if (vspeed > 0.05) {
				spriteSystem.setSprite(SpriteAction::PLAYER_FALL);
				isChanedActionWait = true;
			}
		}

		if(!isChanedActionWait)
			spriteSystem.setSprite(SpriteAction::PLAYER_WAIT);

		isOnGround = false;
	}

	void Player::updateLate() {
		if (isDead) {
			hitBox->setPos(pos);
			return;
		}

		// 移動
		pos.x += hspeed;
		pos.y += vspeed;

		if (!Global::isLoopStage) {
			//非ループモード時は画面外で死ぬ
			checkOutOfScreen();
			if (isOutOfScreen) {
				playerDead();
			}
		}
		else {
			//ループモード時は画面端で反対側にワープ
			loopStage();
		}

		hitBox->setPos(pos);
	}

	void Player::draw() const {
		const ScopedRenderStates2D rs{ SamplerState::ClampNearest };
		TextureRegion texture = spriteSystem.getTextureRegion(direction);
		if(!isDead)texture.scaled(1.0).drawAt(pos.x,pos.y - 3, ColorF(1.0, isMuteki ? 0.5 : 1.0));
		else texture.scaled(1.0).drawAt(pos.x, pos.y - 3, ColorF(0.8,0,0,0.8));
		//hitBox->draw(ColorF(Palette::Red,0.6));
	}

	void Player::playerMoveLeft() {
		hspeed = -maxSpeed;
		direction = Global::Direction::LEFT;
		spriteSystem.setSprite(SpriteAction::PLAYER_RUN);
		isChanedActionWait = true;
	}

	void Player::playerMoveRight() {
		hspeed = maxSpeed;
		direction = Global::Direction::RIGHT;
		spriteSystem.setSprite(SpriteAction::PLAYER_RUN);
		isChanedActionWait = true;
	}

	void Player::playerJump() {
		if (isOnGround) {
			vspeed = -jump;
			djump = true;
			AudioAsset(Sound::DJUMP).playOneShot();
			isOnGround = false;
		}
		else if (djump) {
			vspeed = -jump2;
			djump = false;
			AudioAsset(Sound::JUMP).playOneShot();
		}
	}

	void Player::playerVJump() {
		if (vspeed < 0) {
			vspeed *= 0.45;
		}
	}

	void Player::playerShoot() {
		isGenerateBullet = true;
	}

	//ダメージを受けた際の処理
	void Player::playerHited() {
		if (hp > 0) {
			AudioAsset(Sound::DEATH).playOneShot();
			hp--;
		}

		if (hp <= 0) {
			AudioAsset(Sound::DEATH).playOneShot();
			playerDead();
		}

		isMuteki = true;
		mutekiInterval.restart();
	}

	void Player::playerDead() {
		if (isDead) {
			return;
		}

		isDead = true;
		++Global::deathCount;
		if (Global::isLow1RestartDeathCheckActive
			&& Global::low1RestartDeathCheckElapsed <= Global::low1RestartDeathCheckDuration) {
			Global::endingValue = 5;
			Global::isLow1RestartDeathCheckActive = false;
		}
		hspeed = 0;
		vspeed = 0;
		spriteSystem.stopOrPlayAnimation(false);
		AudioAsset(Sound::DEATH).playOneShot();
	}

	Vec2 Player::snappedPos(Vec2 p)
	{
		return {
			Math::Floor(p.x),
			Math::Floor(p.y)
		};
	}

	void Player::onCollision(GameObject& other) {
		// ブロック衝突
		if (other.type == ObjectType::Block) {

			// ブロックが他タイプだった場合
			auto* block = dynamic_cast<Block*>(&other);
			if (this->intersects(other)) {
				if (block->blockType == BlockType::Hide) {
					auto* hideBlock = dynamic_cast<HideBlock*>(&other);
					if (hideBlock->getIsHidden()) {
						hideBlock->setIsHidden(false);
						AudioAsset(Sound::BLOCKCHANGE).playOneShot();
					}
				}
				else if (block->blockType == BlockType::ConditionalHide) {
					auto* chBlock = dynamic_cast<ConditionalHideBlock*>(&other);
					if (chBlock->getIsHidden() && chBlock->getHasCollide()) {
						chBlock->setIsHidden(false);
						AudioAsset(Sound::BLOCKCHANGE).playOneShot();
					}
				}
				else if (block->blockType == BlockType::Fake) {
					auto* fakeBlock = dynamic_cast<FakeBlock*>(&other);
					if (!fakeBlock->getIsHidden()) {
						fakeBlock->setIsHidden(true);
						AudioAsset(Sound::BLOCKCHANGE).play();
					}
				}
				else if (block->blockType == BlockType::Water) {//水ブロック
					auto* waterBlock = dynamic_cast<WaterBlock*>(&other);
					djump = true;//水中で二段ジャンプ復活
					if (vspeed > 2)vspeed = 2;//水中での落下速度制限
				}
			}

			if (!block->getHasCollide())return;

			// --- 以下通常のブロックとの衝突判定 ---

			Vec2 modifiedPos = snappedPos(pos);

			// --- 横方向 予測衝突 ---
			if (hspeed != 0) {
				RectF nextHitBox = RectF(Arg::center(modifiedPos.x + hspeed, modifiedPos.y), hitBoxSize.x - 2, 1);

				if (nextHitBox.intersects(*other.hitBox->getRect()))
				{
					if (hspeed > 0) {
						pos.x = other.hitBox->left().x - 5;
					}
					else {
						pos.x = other.hitBox->right().x + 5;
					}

					hspeed = 0;
				}
			}

			// --- 縦方向 予測衝突 ---
			if (vspeed != 0) {
				//方向によって当たり判定の位置、大きさを変える
				RectF nextHitBox;
				if(vspeed > 0) nextHitBox = RectF(Arg::center(hitBox->bottom().x, hitBox->bottom().y + vspeed), hitBoxSize.x - 6, 1);
				if(vspeed < 0) nextHitBox = RectF(Arg::center(hitBox->top().x, hitBox->top().y - vspeed - 3), hitBoxSize.x - 6, 1);

				if (nextHitBox.intersects(*other.hitBox->getRect()))
				{
					if (vspeed > 0) {
						pos.y = other.hitBox->top().y - 10;
						djump = true;
						isOnGround = true;
					}
					else {
						pos.y = other.hitBox->bottom().y + 9;
					}
					gravity = 0;
					vspeed = 0;
				}
			}

			RectF nextHitBox = RectF(Arg::center(modifiedPos.x + hspeed, modifiedPos.y + vspeed + 2), hitBoxSize.x - 2, hitBoxSize.y - 9);
			if (nextHitBox.intersects(*other.hitBox->getRect())) {
				hspeed = 0;
			}
		}

		// PlayerKill属性を持つオブジェクトとの衝突
		if (other.canPlayerKill) {
			if (this->intersects(other) && !isDead && !isMuteki) {
				if (Global::getItem1 && (Global::nowRoomName == U"boss" || Global::nowRoomName == U"bossLow" || Global::nowRoomName == U"ExBoss" || Global::nowRoomName == U"trapBoss")) playerHited();
				else playerDead();
			}
		}

		// トリガーとの衝突
		if (other.type == ObjectType::Trigger) {
			if (this->intersects(other)) {
				auto* trigger = dynamic_cast<Trigger*>(&other);
				if (!trigger->getCheckPrevID()) {
					trigger->triggerActivate();

					//trap2 map専用
					if (trigger->getTrapID() == 30) {
						Global::trapActivatedId30InTrap2Map = true;
					}
				}
				else {
					if(trigger->getTrapID() - 1 == nowTrapID)trigger->triggerActivate();
				}
				if(trigger->getTriggerType() == TriggerType::Secret) {
					auto* secretTrigger = dynamic_cast<SecretTrigger*>(trigger);
					secretTrigger->triggerActivate();
				}
			}
		}

		// ワープとの衝突
		if (other.type == ObjectType::Warp) {
			if (this->intersects(other)) {
				auto* warp = dynamic_cast<Warp*>(&other);
				if (warp->getCanWarp()) {
					Global::prevRoomName = Global::nowRoomName;
					Global::nowRoomName = (Global::nowRoomName == U"tutorialLow" && Global::moraleValue1 >= 90)
						? Global::chooseGenerateStage()
						: Global::isGenerateStage(Global::nowRoomName)
						? Global::chooseGenerateStage()
						: warp->getNextRoomName();
					Global::isChangeRoom = true;
				}
			}
		}

		// セーブとの衝突
		if (other.type == ObjectType::SavePoint) {
			auto* save = dynamic_cast<SavePoint*>(&other);
			if (save->getSaveType() == SaveType::Secret) {
				auto* secretSave = dynamic_cast<SecretSavePoint*>(save);
				secretSave->isPlayerTouching = this->intersects(*secretSave);
				// セーブポイントに触れている状態で、特定のキーを押すと脱出
				if (!Global::prepareGetItem1 && secretSave->isPlayerTouching && Global::inputEscape.down()) {
					Global::prevRoomName = Global::nowRoomName;
					Global::nowRoomName = secretSave->getEscapeRoomName();
					Global::isChangeRoom = true;
				}
			}
		}

		// 看板との衝突
		if (other.type == ObjectType::Sign) {
			 auto* sign = dynamic_cast<Sign*>(&other);
			 sign->isPlayerTouching = this->intersects(*sign);
		}

		// アイテムとの衝突
		if (other.type == ObjectType::Item) {
			auto* item = dynamic_cast<Item*>(&other);
			if (!item->isPlayerTouching && this->intersects(other)) {
				item->isPlayerTouching = true;
			}
		}
	}

	// 地面に接地しているかどうかを取得
	bool Player::getOnGround() const {
		return isOnGround;
	}

	// 死亡状態かどうかを取得
	bool Player::getIsDead() const {
		return isDead;
	}

	// 弾生成フラグを設定
	void Player::setIsGenerateBullet(bool value) {
		isGenerateBullet = value;
	}

	// 弾生成フラグを取得
	bool Player::getIsGenerateBullet() const {
		return isGenerateBullet;
	}

	// 向きを取得
	Global::Direction Player::getDirection() const {
		return direction;
	}

	// 無敵状態を設定
	void Player::setIsMuteki(bool value) {
		isMuteki = value;
	}

	// 無敵状態かどうかを取得
	bool Player::getIsMuteki() const {
		return isMuteki;
	}

	//hpを取得
	int32 Player::getHp() const {
		return hp;
	}

	// アニメーションの再生と停止を切り替える
	// true の場合は再生、false の場合は停止
	void Player::setStopOrPlayAnimation(bool isPlay) {
		spriteSystem.stopOrPlayAnimation(isPlay);
	}

	//ステージ上での現在の罠IDを取得する
	void Player::setNowTrapID(int32 id) {
		nowTrapID = id;
	}

	//画面外判定
	void Player::checkOutOfScreen() {
		const int32 excessX = 0;//画面端からの余白
		const int32 excessY = 0;//画面端からの余白
		if ((pos.x < -1 * excessX || pos.x > Global::stageWidth + excessX ||
			pos.y < -1 * excessY || pos.y > Global::stageHeight + excessY)) {
			isOutOfScreen = true;
		}
		else {
			isOutOfScreen = false;
		}
	}

	//ループモード時の画面外判定
	void Player::loopStage() {
		const int32 excessX = hitBoxSize.x / 2 - 4;//画面端からの余白
		const int32 excessY = hitBoxSize.y / 2 - 4;//画面端からの余白
		if (pos.x < -1 * excessX) {
			pos.x = Global::stageWidth + excessX;
		}
		else if (pos.x > Global::stageWidth + excessX) {
			pos.x = -1 * excessX;
		}
		else if (pos.y < -1 * excessY) {
			pos.y = Global::stageHeight + excessY;
		}
		else if (pos.y > Global::stageHeight + excessY) {
			pos.y = -1 * excessY;
		}
	}
}
