#include "Gun.h"
#include "DxLib.h"
#include "mymath.h"	

bool Gun::Initialize()
{
	_gunHandle = MV1LoadModel("res/Gun/GunM29.mv1");
	// モデルスケール（1.0 が原寸）
	if(_gunHandle != -1)
	{
		MV1SetScale(_gunHandle, VGet(_scale, _scale, _scale));
	}
	/*_gunPos = VGet(0.5f, -0.5f, 1.0f); */// 銃の位置調整用	
	return true;
}

bool Gun::Terminate()
{
	// モデル解放
	if(_gunHandle != -1) 
	{
		MV1DeleteModel(_gunHandle);
		_gunHandle = -1;
	}

	return true;
}

bool Gun::Process(int key, int trg)
{
	if(_player)
	{
		// プレイヤーの位置を取得
		VECTOR playerPos = _player->GetPos();

		// プレイヤー向きから角度を算出
		float yaw = atan2(_player->_vDir.x, _player->_vDir.z);

		// mymathのマクロを使って90度をラジアンで取得
		const float ANGLE_OFFSET = DEG2RAD(90.0f); // = PI/2

		// 銃のローカルオフセットをプレイヤーの回転に合わせて回す
		VECTOR rotatedOffset = RotateY(_gunPos, yaw + ANGLE_OFFSET);

		// FPS モードかつカメラがある場合はカメラ基準で「目の前」に置く
		if(_player->isFps && _player->_cam != nullptr)
		{
			// カメラ位置とターゲットから前方ベクトルを取得
			VECTOR camPos = _player->_cam->_vPos;
			VECTOR camTarget = _player->_cam->_vTarget;
			
			// 前方ベクトル（正規化）
			VECTOR forward = VecNormalize(VSub(camTarget, camPos));
			// 上ベクトル
			VECTOR up    = VGet(0.0f, 1.0f, 0.0f);
			// 右ベクトル
			VECTOR right = VecNormalize(VCross(up, forward));

			// 調整パラメータ
			const float zOffset = 80.7f;  // zへのオフセット距離
			const float xOffset = 0.5f;  // xへのオフセット距離
			const float yOffset = -8.f; // yへのオフセット距離

			// 銃の位置をカメラ位置からのオフセットで計算
			_vPos = VAdd(VAdd(VAdd(camPos, VScale(forward, zOffset)), VScale(right, xOffset)), VGet(0.0f, yOffset, 0.0f));

			// 銃の向きはカメラの向きと合わせる
			_vDir = forward;
		}
		else
		{
			// TPS: プレイヤーの位置に銃のオフセットを加算して銃のワールド位置を計算
			_vPos = VAdd(playerPos, rotatedOffset);

			// プレイヤーの向きを銃にも適用
			_vDir = _player->_vDir;
		}
	}
	
	return true;
}


bool Gun::Render()
{
	if(_gunHandle == -1) return true;
	if(!_player)         return true;

	// 位置・回転・スケールをセットして描画
	MV1SetPosition(_gunHandle, _vPos);
	// スケールを適用（軸別に変えたい場合は VGet(x,y,z) を調整）
	MV1SetScale(_gunHandle, VGet(_scale, _scale, _scale));

	//FPSのときはカメラの前方ベクトルから角度を復元して回す
	if(_player->isFps == true && _player->_cam != nullptr)
	{
		//カメラの前方・上ベクトルを取得
		VECTOR camForward = VNorm(VSub(_player->_cam->_vTarget, _player->_cam->_vPos));
		VECTOR camUp	  = VGet(0.0f, 1.0f, 0.0f);

		//pitch（X軸回転）を算出
		float horizontalLen = sqrtf(camForward.x * camForward.x + camForward.z * camForward.z);
		float pitch		    = atan2f(camForward.y, horizontalLen); // -PI/2 ～ +PI/2
		//モデルの原点向きに依存するオフセット
		const float ANGLE_OFFSET = DEG2RAD(90.0f); // = PI/2

		//yaw（Y軸回転）を算出
		float yaw = atan2(camForward.x, camForward.z);
		// 回転ベクトルを作成
		VECTOR vRot = { -pitch , yaw + ANGLE_OFFSET, 0.0f };
		// 回転をセットして描画
		MV1SetRotationXYZ(_gunHandle, vRot);
		MV1DrawModel(_gunHandle);

		//　デバッグ用
		{
			int x = 0, y = 600, size = 16;
			SetFontSize(size);
			DrawFormatString(x, y, GetColor(255, 0, 0), "Gun FPS Mode:"); y += size;
			DrawFormatString(x, y, GetColor(255, 0, 0), "pos = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z); y += size;
			DrawFormatString(x, y, GetColor(255, 0, 0), "dir = (%5.2f, %5.2f, %5.2f)", _vDir.x, _vDir.y, _vDir.z); y += size;
			DrawFormatString(x, y, GetColor(255, 0, 0), "Rot = (%5.2f, %5.2f, %5.2f)",  vRot.x,  vRot.y,  vRot.z); y += size;
		}
	}
	return true;
}

// playerに装備させる(プレイヤーと同じ操作をする)の設定
void Gun::AttachToPlayer(Player* player)
{
	_player = player;
	if(_player)
	{
		float yaw = atan2(_player->_vDir.x, _player->_vDir.z);      // プレイヤー向きから角度を算出
		const float ANGLE_OFFSET = DEG2RAD(90.0f); // = PI/2		// mymathのマクロを使って90度をラジアンで取得
		VECTOR rotatedOffset = RotateY(_gunPos, yaw + ANGLE_OFFSET);// 銃のローカルオフセットをプレイヤーの回転に合わせて回す
		_vPos = VAdd(_player->GetPos(), rotatedOffset);				// プレイヤーの位置に銃のオフセットを加算して銃のワールド位置を計算
	}
}

// プレイヤーから外す
void Gun::Detach()
{
	_player = nullptr;
}

// Y軸回りにベクトルを回転させる関数
VECTOR Gun::RotateY(const VECTOR& v, float yaw)
{
	// Y軸回りに回転（右手系、yawはラジアン）
	float c = cosf(yaw);
	float s = sinf(yaw);
	VECTOR r;
	r.x = v.x * c + v.z * s;
	r.y = v.y;
	r.z = -v.x * s + v.z * c;

	return r;
}

