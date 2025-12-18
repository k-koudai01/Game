#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "Player.h" 
#include "Camera.h"
#include "Gun.h"
#include "mymath.h"

bool ModeGame::Initialize()
{
	if(!base::Initialize()) { return false; }
	// プレイヤーキャラを生成
	_player = new Player();
	_player->Initialize();

	_gun = new Gun();
	_gun->Initialize();
	_gun->SetScale(5.5f);

	// 銃をプレイヤーに装備（初期化時に一度だけ）
	_gun->AttachToPlayer(_player);
	

	// マップデータの読み込み
	_handleMap = MV1LoadModel("res/Dungeon/Dungeon.mv1");
	_handleSky = MV1LoadModel("res/SkySphere/skysphere.mv1");

	//コリジョン情報の生成
	_frameMapCollision = MV1SearchFrame(_handleMap, "dungeon_collision");
	MV1SetupCollInfo(_handleMap, _frameMapCollision, 16, 16, 16);
	//コリジョンのフレームを描画しない
	MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);

	_cameraTP = new Camera();
	_cameraTP->Initialize();
	//// カメラを生成
	//_camera = new Camera();
	//_camera->Initialize();
	
	// FPS カメラを生成（実体）
	_fCamera = new fpsCamera();
	_fCamera->Initialize();

	//初期アクティブカメラをTPSカメラに設定	
	_camera = _cameraTP;	

	// ここで両方向をつなぐ
	_camera->_player = _player;
	_cameraTP->_player = _player;
	_fCamera->_player = _player;
	_player->SetCamera(_camera);

	// その他初期化
	_bViewCollision = TRUE;

	//敵一体生成
	_enemy = new Enemy();
	_enemy->Initialize();
	_enemy->SetTargetPlayer(_player);
	
	return true;
}

bool ModeGame::Terminate()
{
	base::Terminate();
	_player->Terminate();
	if (_gun){ _gun->Terminate(); delete _gun; _gun = nullptr; }

	// カメラ解放（実体をTerminateして delete）
	if(_cameraTP) { _cameraTP->Terminate(); delete _cameraTP; _cameraTP = nullptr; }
	if(_fCamera) { _fCamera->Terminate();  delete _fCamera;  _fCamera = nullptr; }

	_camera = nullptr;
	if(_enemy) { _enemy->Terminate(); delete _enemy; _enemy = nullptr; }
	return true;
}

bool ModeGame::Process()
{
	base::Process();
	int key = ApplicationMain::GetInstance()->GetKey();
	int trg = ApplicationMain::GetInstance()->GetTrg();
	_player->SetCamera(_camera);
	
	_player->Process(key, trg);
	if (_gun) { _gun->Process(key, trg);}
	PlayerCollision();

	if(_enemy) { _enemy->Process(); }

	// デバッグ機能
	if(trg & PAD_INPUT_6)
	{
		_bViewCollision = !_bViewCollision;
	}
	if(_bViewCollision)
	{
		MV1SetFrameVisible(_handleMap, _frameMapCollision, TRUE);
	}
	else
	{
		MV1SetFrameVisible(_handleMap, _frameMapCollision, FALSE);
	}

	// カメラ切り替え
	if(key & PAD_INPUT_5)
	{
		_fpsHoldTimerMs += GetStepTm();
		if(_fpsHoldTimerMs >= _fpsHoldThresholdMs)
		{
			// ボタンを押し続けている間はFPSカメラにする
			if(_camera != _fCamera)
			{
				_camera = _fCamera;

				// 関連付け更新（状態変化時のみ）
				if(_camera) { _camera->_player = _player; }
				if(_cameraTP) { _player->SetCamera(_cameraTP); } // TPS参照を保持しておく
				if(_fCamera) { _fCamera->_player = _player; }
				_player->SetCamera(_camera);

				// プレイヤーに切替通知（必要に応じてアニメ等を切替）
				_player->OnSwitchToFPS();
			}
		}
		
	}
	else
	{
		_fpsHoldTimerMs = 0;
		// ボタンを離している間はTPSに戻す
		if(_camera != _cameraTP)
		{
			_camera = _cameraTP;

			// 関連付け更新（状態変化時のみ）
			if(_camera) { _camera->_player = _player; }
			if(_cameraTP) { _player->SetCamera(_cameraTP); }
			if(_fCamera) { _fCamera->_player = _player; }
			_player->SetCamera(_camera);

			// プレイヤーに切替通知
			_player->OnSwitchToTPS();
		}
	}
	// 両方のカメラを毎フレーム更新する（TPSは常にプレイヤーを追う必要があるため）
	if(_cameraTP)
	{
		if (_camera == _cameraTP) _cameraTP->Process(key, trg);
		else                      _cameraTP->FollowUpdate(); // 非アクティブ時は入力を無視して追従のみ

	}
	if(_fCamera)
	{
		if (_camera == _fCamera)  _fCamera->Process(key, trg);
		else                      _fCamera->FollowUpdate();
	}
	return true;
}

bool ModeGame::Render()
{
	base::Render();
	// 3D基本設定
	SetUseZBuffer3D(TRUE);
	SetWriteZBuffer3D(TRUE);
	SetUseBackCulling(TRUE);

	//ライト設定
	SetUseLighting(TRUE);


	//プレイヤーの描画
	_player->Render();
	
	// 敵の描画
	if(_enemy) { _enemy->Render(); }

	if(_bViewCollision)
	{
		DrawLine3D(VAdd(_player->_vPos, VGet(0, _player->GetColSubY(), 0)), VAdd(_player->_vPos, VGet(0, -99999.f, 0)),  GetColor(255, 0, 0));
		DrawSphere3D(_player->_vPos, _player->_collision_r, 10, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
		DrawSphere3D(_enemy->_vPos, _enemy->_collision_r, 10, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
	}
	
	//マップの描画
	{
		MV1DrawModel(_handleMap);
		MV1DrawModel(_handleSky);
	}

	// 0,0,0を中心に線を引く
	{
		float linelength = 1000.f;
		VECTOR v = { 0, 0, 0 };
		DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
		DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
		DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
	}

	if(_gun) { _gun->Render(); }
	// カメラの設定（ビュー行列をセット -> これを描画の前に呼ぶ）
	if(_camera) { _camera->Render(); }


	return true;
}

// プレイヤーのコリジョン処理
void ModeGame::PlayerCollision()
{
	// プレイヤーの位置を取得
	VECTOR playerPos    = _player->GetPos();
	VECTOR v            = _player->GetMoveV();
	float playercolsubY = _player->GetColSubY();
	VECTOR oldvPos		= playerPos;

	// 移動した先でコリジョン判定
	MV1_COLL_RESULT_POLY hitPoly;

	// 主人公の腰位置から下方向への直線
	hitPoly = MV1CollCheck_Line(_handleMap, _frameMapCollision,
		VAdd(_player->_vPos, VGet(0, _player->_colSubY, 0)), VAdd(playerPos, VGet(0, -99999.f, 0)));

	if(hitPoly.HitFlag)
	{
		// 当たった
		// 当たったY位置をキャラ座標にする
		_player->_vPos.y = hitPoly.HitPosition.y;

		if(_camera)
		{
			// カメラも同じ分移動させる
			if(_cameraTP) { _cameraTP->MoveBy(v); }
			
			// FPSカメラも同じ分移動させる
			if(_fCamera && _fCamera != _cameraTP) { _fCamera->MoveBy(v); }
		}
	}
	else
	{
		// 当たらなかった。元の座標に戻す
		_player->_vPos = _player->oldPos;
	}

}
// プレイヤーと敵の当たり判定
void ModeGame::PlayerEnemyCollision(Player* pl, Enemy* en)
{
	if(!pl || !en) return;
	// 円同士の当たり判定
	if (pl->IsHitCircle(pl, en))
	{
		// 当たったときの処理
	}
}

// ステルス関係
bool ModeGame::IsPlayerInEnemyView(const Enemy* enemy, Player* player, float viewDistance, float viewAngleDeg)
{

}
