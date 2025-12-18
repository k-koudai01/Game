#include "Player.h"

#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "mymath.h"


bool Player::Initialize() 
{
	if(!Cbase::Initialize()) { return false; }
	// モデルデータのロード（テクスチャも読み込まれる）
	_handle = MV1LoadModel("res/SDChar/SDChar.mv1");
	// 3Dモデルの1番目のアニメーションをアタッチする
	_attach_index = -1;
	// ステータスを「なし」に設定
	_status = STATUS::NONE;
	// アタッチしたアニメーションの総再生時間を取得する
	_total_time = 0.f;
	// 再生時間の初期化
	_play_time = 0.0f;
	// 位置,向きの初期化
	_vPos = VGet(0, 0, 0);
	_vDir = VGet(0, 0, -1); //キャラクターはデフォルト-Z方向を向いている
	// 移動ベクトルの初期化
	_v = VGet(0, 0, 0);      
	//腰の位置の設定
	_colSubY = 40.0f;
	//アナログスティックの設定
	analogMin = 0.3f;
	//弾の生成
	if(_bullet == nullptr) 
	{
		_bullet = new Bullet();
	}
	//弾の初期化
	_bullet->Initialize();
	//コリジョン初期化
	_collision_weight = 10.f;
	_collision_r = 30.f;
	return true;
}

bool Player::Terminate()
{
	Cbase::Terminate();
	
	if(_bullet)
	{
		_bullet->Terminate();
		delete _bullet;
		_bullet = nullptr;
	}
	return true;
}

bool Player::Process(int key, int trg) 
{
	Cbase::Process();
	// ジョイパッドの状態を取得
	{
		DINPUT_JOYSTATE di;
		GetJoypadDirectInputState(DX_INPUT_PAD1, &di);
		if (GetJoypadDirectInputState(DX_INPUT_PAD1, &di) == 0)
		{
			lx = (float)di.X / 1000.f;
			ly = (float)di.Y / 1000.f;
			rx = (float)di.Z / 1000.f;
			ry = (float)di.Rz / 1000.f;
		}
	}

	// FPS/TPS切り替えキー
	// 
	static bool prevSwitchKey = false;
	bool switchKey = (CheckHitKey(KEY_INPUT_TAB) != 0);

	if(switchKey && !prevSwitchKey)
	{
		// 押下の立ち上がりで切替を一度だけ実行
		if(_cam != nullptr && dynamic_cast<fpsCamera*>(_cam) != nullptr)
		{
			// FPS -> TPS
			Camera* newCam = new Camera();
			newCam->_player = this;
			newCam->Initialize();
			if(_cam) { _cam->Terminate(); delete _cam; }
			_cam = newCam;
			OnSwitchToTPS();
		}
		else
		{
			// TPS -> FPS
			fpsCamera* newCam = new fpsCamera();
			newCam->_player = this;
			newCam->Initialize();
			if(_cam) { _cam->Terminate(); delete _cam; }
			_cam = newCam;
			OnSwitchToFPS();
		}
	}

	prevSwitchKey = switchKey;
	
	//　処理前のステータスを保存しておく
	STATUS oldStatus = _status;

	isFps = false;
	if(_cam != nullptr)
	{
		isFps = (dynamic_cast<fpsCamera*>(_cam) != nullptr);
	}

	// カメラの種類によって向きの計算方法を変える
	float camrad = 0.f; // カメラの向いている角度
	if(_cam != nullptr)
	{
		float sx = _cam->_vPos.x - _cam->_vTarget.x;
		float sz = _cam->_vPos.z - _cam->_vTarget.z;
		camrad = atan2f(sz, sx);
	}

	// キーボード入力
	int input = key;

	//左スティック値
	float lstickX = lx;
	float lstickY = ly;

	// FPSモードでなければ移動処理
	if(isFps != TRUE)
	{
		// 操作
		if(CheckHitKey(KEY_INPUT_W) || CheckHitKey(KEY_INPUT_UP))
		{
			input |= PAD_INPUT_UP;
			lstickY = -1.0f;
		}
		if(CheckHitKey(KEY_INPUT_S) || CheckHitKey(KEY_INPUT_DOWN))
		{
			input |= PAD_INPUT_DOWN;
			lstickY = 1.0f;
		}
		if(CheckHitKey(KEY_INPUT_A) || CheckHitKey(KEY_INPUT_LEFT))
		{
			input |= PAD_INPUT_LEFT;
			lstickX = -1.0f;
		}
		if(CheckHitKey(KEY_INPUT_D) || CheckHitKey(KEY_INPUT_RIGHT))
		{
			input |= PAD_INPUT_RIGHT;
			lstickX = 1.0f;
		}

		// 移動方向を決める
		_v = { 0,0,0 };
		float mvSpeed = 5.f;
		float length = sqrt(lstickX * lstickX + lstickY * lstickY);
		float rad = atan2(lstickX, lstickY);

		if(input & PAD_INPUT_DOWN) { _v.x = 1; }
		if(input & PAD_INPUT_UP) { _v.x = -1; }
		if(input & PAD_INPUT_LEFT) { _v.z = -1; }
		if(input & PAD_INPUT_RIGHT) { _v.z = 1; }


		// アナログ左スティック用
		if(length < analogMin)
		{
			// 入力が小さかったら動かなかったことにする
			length = 0.f;
		}
		else
		{
			length = mvSpeed;
		}

		// vをrad分回転させる
		if(VSize(_v) > 0.f) { length = mvSpeed; }
		_v.x = cos(rad + camrad) * length;
		_v.z = sin(rad + camrad) * length;

		// 移動前の位置を保存
		oldPos = _vPos;

		// vの分移動
		_vPos = VAdd(_vPos, _v);

		//移動量をそのままキャラの方向にする
		if(VSize(_v) > 0.f)
		{
			_vDir = _v;
			_status = STATUS::WALK; // 移動中
		}
		else
		{
			_status = STATUS::WAIT;
		}
	}

	// ステータスが変わっていないか？
	if(oldStatus == _status)
	{
		// 再生時間を進める
		_play_time += 0.5f;
	}
	else
	{
		// ステータスが変わっていたらアニメーションを変更する
		if (_attach_index != -1)
		{
			MV1DetachAnim(_handle, _attach_index);
			_attach_index = -1;
		}
		// ステータスに応じたアニメーションをアタッチする
		switch(_status)
		{
			case STATUS::WAIT:
				_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "idle"), -1, FALSE);
				break;
			case STATUS::WALK:
				_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "run"), -1, FALSE);
				break;

		}
		// アタッチしたアニメーションの総再生時間を取得する
		_total_time = MV1GetAttachAnimTotalTime(_handle, _attach_index);
		// 再生時間を初期化
		_play_time = 0.0f;
	}

	// 再生時間がアニメーションの総再生時間に達したら再生時間を０に戻す
	if(_play_time >= _total_time)
	{
		_play_time = 0.0f;
	}

	if(isFps == TRUE)
	{
		if(trg & PAD_INPUT_1)
		{
			// 弾の発射位置
			VECTOR bulletStartP = VAdd(_vPos, VGet(0.f, _colSubY + 50, 0.f));
			//弾の発射方向
			VECTOR bulletDir;
			if(_cam != nullptr && isFps)
			{
				bulletDir = VecNormalize(VSub(_cam->_vTarget, _cam->_vPos)); // カメラの向き
			}
			else
			{
				// _vDir がゼロの可能性があるのでフォールバック
				if(VSize(_vDir) == 0.0f) bulletDir = VGet(0.0f, 0.0f, -1.0f);
				else                     bulletDir = VecNormalize(_vDir); // プレイヤーの向き
			}

			//確認用に一発出す
			_bullet->Activate(bulletStartP, bulletDir, _bullet->_speed, _bullet->_life, _bullet->_length, _bullet->_color);
		}

		
	}
	// 弾の毎フレーム処理（互換性のため引数なし Process() を使用）
	_bullet->Process();

	return true;
}

bool Player::Render()
{
	Cbase::Render();
	// 再生時間をセットする
	MV1SetAttachAnimTime(_handle, _attach_index, _play_time);
	if (isFps != TRUE)
	{
		// モデルを描画する
		{
			MV1SetPosition(_handle, _vPos);
			//　向きからY軸回転を算出
			VECTOR vRot = { 0, 0, 0 };
			vRot.y = atan2(_vDir.x * -1, _vDir.z * -1); // モデルが標準でどちらを向いているかで式が変わる(これは-zを向いている場合)
			MV1SetRotationXYZ(_handle, vRot);
			// 描画
			MV1DrawModel(_handle);
		}

		//弾の呼び出し
		if(_bullet && _cam)
		{
			_bullet->Render(_cam->GetPos());
		}
	}
	//弾の呼び出し
	if(_bullet && _cam)
	{
		_bullet->Render(_cam->GetPos());
	}
	

	return true;
}

// FPSモードに切り替えたときの処理
void Player::OnSwitchToFPS()
{
	if(!_cam) return;
	// カメラをプレイヤーの頭上に移動させる
	_cam->SetTarget(VAdd(_vPos, VGet(0.f, 120.f, 0.f)));
	// カメラ位置を頭位置にセット（FPS）
	_cam->SetPos(VAdd(_vPos, VGet(0.f, 120.f, 0.f)));

	// プレイヤーの向きをカメラの向きに合わせる
	if(auto fcam = dynamic_cast<fpsCamera*> (_cam))
	{
		fcam->AlignTargetToPlayerDirection();
	}
}

// TPSモードに切り替えたときの処理
void Player::OnSwitchToTPS()
{
	if (!_cam) return;
	// TPS 用の背後オフセットを設定（例: 上 + 後ろ）
	VECTOR target = VAdd(_vPos, VGet(0.0f, _colSubY, 0.0f));
	_cam->SetTarget(target);
	//プレイヤーの向きから後方方向を計算
	VECTOR forward = VGet(0.0, 0.0, -1.0f);	
	if(VSize(_vDir) != 0.0f)
	{
		forward = VecNormalize(_vDir); // プレイヤーの後ろの向き
	}

	//TPSのオフセット
	const float upOffset = 70.0f;
	const float backDistance = 300.0f;

	//カメラの位置 = ターゲット + 上方向 - forward * オフセット距離
	VECTOR posAbove = VAdd(target, VGet(0.0f, upOffset, 0.0f)); // ターゲットの上方向オフセット
	VECTOR posBack = VGet(forward.x * backDistance, forward.y * backDistance, forward.z * backDistance); // プレイヤーの後方方向オフセット
	VECTOR camPos = VSub(posAbove, posBack);
	
	_cam->SetPos(camPos);
}

