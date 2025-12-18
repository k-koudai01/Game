#pragma once
#include "CharaBase.h"
#include "Bullet.h"
#include <vector>

class Player;
class Enemy :public CharaBase
{
public:
	virtual bool Initialize() override;
	virtual bool Terminate() override;
	virtual bool Process() override;
	virtual bool Render() override;
	//セッター
	void SetTargetPlayer(Player* p) { _target = p; }
	void SetSpeed(float s) { speed = s; }

protected:
	int _handle = -1;
	int _attach_index = -1;
	float _total_time = 0.f;
	float _play_time = 0.f;
	//ステータス
	enum class STATUS
	{
		NONE,
		WAIT,
		WALK,
		_EOT_
	};
	STATUS _status = STATUS::NONE;

	float _colSubY = 40.0f;
	VECTOR _v = VGet(0, 0, 0);
	float speed = 1.5f;
	Player* _target = nullptr;
	
	// 弾関連
	std::vector<Bullet*> _bullets;
	float   _shootCooldown = 1.5f; // 発射間隔
	float   _shootTimer = 0.0f;    // 発射タイマー
	unsigned long _lastTimeMs = 0;
};

