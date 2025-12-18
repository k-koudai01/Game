#pragma once
#include "appframe.h"
#include "ApplicationMain.h"
#include "Player.h" 
#include "Camera.h"
#include "fpsCamera.h"
#include "Enemy.h"
#include <string>

class Gun;
// モード
class ModeGame : public ModeBase
{
	typedef ModeBase base;
public:
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();
	// プレイヤーとマップの当たり判定
	void PlayerCollision();
	// プレイヤーと敵の当たり判定
	void PlayerEnemyCollision(Player* _pl, Enemy* _en);
	//ステルス関係
	bool IsPlayerInEnemyView(Player* pl, const Enemy* en, float viewDistance, float viewAngleDeg);
	void DrawEnemyWithCamera(const Enemy* en, const Camera* cam);

	Player* _player;
	Enemy * _enemy;
	Camera* _camera;
	Camera* _cameraTP;
	fpsCamera* _fCamera;
	Gun* _gun = nullptr;

protected:
	//マップ用
	int _handleMap;
	int _handleSky;
	int _frameMapCollision;
	// デバッグ用
	bool	_bViewCollision;

private:
	//　長押しの際にディレイを入れる
	// unsigned long型はms単位の時間計測に使う(負の値を取らない)
	unsigned long _fpsHoldTimerMs = 0;      // 経過時間計測用タイマー
	unsigned long _fpsHoldThresholdMs = 150;// 押し続けてから切り替わるまでの時間(ms)
};
