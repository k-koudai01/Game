#include "fpsCamera.h"
#include "mymath.h"	
bool fpsCamera::Initialize()
{
	// 視点はプレイヤーの目の高さに合わせ、近クリップを小さくして銃が切り取られないようにする
	_vPos = VGet(0.0f, 60.0f, 0.0f);
	_vTarget = VGet(0.0f, 60.0f, 1.0f);
	_clipNear = 0.1f;   // FPSでは小さめにすると手元の銃が見える
	_clipFar = 10000.0f;
	return true;
}

bool fpsCamera::Terminate()
{
	return true;
}	

bool fpsCamera::Process(int key, int trg)
{
	if(_player == nullptr) return true;

	// プレイヤー位置を基準にカメラを即時配置（FPSでは遅延を少なくする）
	VECTOR playerPos = _player->GetPos();
	VECTOR desiredPos = VAdd(playerPos, VGet(0.0f, headHeight, 0.0f));

	// 直接反映（遅延を無くすため smooth=1.0 相当）
	_vPos = desiredPos;

	// 前方ベクトルはプレイヤーの向き（水平成分のみを用いる）
	VECTOR forward = _player->_vDir;
	forward.y = 0.0f;
	float len = sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
	if(len != 0.0f)
	{
		forward.x /= len; forward.y /= len; forward.z /= len;
	}
	else {
		forward = VGet(0.0f, 0.0f, 1.0f);
	}

	// 注視点はカメラ位置 + 水平方向の forward * 距離
	const float lookDist = 100.0f;
	VECTOR desiredTarget = VGet(
		_vPos.x + forward.x * lookDist,
		_vPos.y + forward.y * lookDist,
		_vPos.z + forward.z * lookDist
	);

	// ターゲットも即時反映（遅延を無くす）
	_vTarget = desiredTarget;

	RightStyckControl();
	return true;
}

bool fpsCamera::Render()
{
	// カメラ情報表示
	{
		int x = 0, y = 500, size = 16;
		SetFontSize(size);
		DrawFormatString(x, y, GetColor(0, 255, 0), "Camera:"); y += size;
		DrawFormatString(x, y, GetColor(0, 255, 0), "  target = (%5.2f, %5.2f, %5.2f)", _vTarget.x, _vTarget.y, _vTarget.z); y += size;
		DrawFormatString(x, y, GetColor(0, 255, 0), "  pos    = (%5.2f, %5.2f, %5.2f)", _vPos.x, _vPos.y, _vPos.z); y += size;
		float sx = _vPos.x - _vTarget.x;
		float sz = _vPos.z - _vTarget.z;
		float length = sqrt(sz * sz + sx * sx);
		float rad = atan2(sz, sx);
		float deg = RAD2DEG(rad);
		DrawFormatString(x, y, GetColor(255, 0, 0), "  len = %5.2f, rad = %5.2f, deg = %5.2f", length, rad, deg); y += size;
	}

	//カメラを適用
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);
	SetCameraNearFar(_clipNear, _clipFar);
	return true;
}

void fpsCamera::MoveBy(const VECTOR& vMove)
{
	// 基底実装を利用
	Cbase::MoveBy(vMove);
}

// カメラ切り替え時の更新
void fpsCamera::AlignTargetToPlayerDirection()
{
	if(!_player) return;

	// プレイヤーの向きから前方ベクトルを算出
	VECTOR playerDir = _player->_vDir;
	float len = sqrt(playerDir.x * playerDir.x + playerDir.y * playerDir.y + playerDir.z * playerDir.z);

	// 正規化
	if(len < 0.0001f)
	{
		playerDir = VGet(0.0f, 0.0f, -1.0f);
		
	}
	else 
	{
		playerDir = VNorm(playerDir);
	}
	//水平成分からyawを算出
	VECTOR horiz = playerDir; horiz.y = 0.0f;
	float hlen = sqrt(horiz.x * horiz.x + horiz.y * horiz.y + horiz.z * horiz.z);
	if(hlen < 0.0001f)
	{
		yaw = 0.0f;
	}
	else
	{
		// RightStyckControl の forward.x = sin(yaw), forward.z = -cos(yaw) に合うように計算
		yaw = atan2(horiz.x, -horiz.z);
	}

	//垂直成分からpitchを算出
	pitch = asinf(playerDir.y);
	if(pitch > maxPitch) pitch = maxPitch;
	if(pitch < minPitch) pitch = minPitch;

	//カメラ位置とターゲットをプレイヤーの向きに合わせる（更新）
	_vPos = VAdd(_player->GetPos(), VGet(0.0f, headHeight, 0.0f));

	VECTOR forward;
	forward.x = sinf(yaw) * cosf(pitch);
	forward.y = sinf(pitch);
	forward.z = -cosf(yaw) * cosf(pitch);
	_vTarget = VGet(
		_vPos.x + forward.x * lookDist,
		_vPos.y + forward.y * lookDist,
		_vPos.z + forward.z * lookDist
	);
}
// 右スティックでカメラ操作
void fpsCamera::RightStyckControl()
{
	if (!_player) return;

	//　頭の位置にカメラを固定
	_vPos = VAdd(_player->GetPos(), VGet(0.0f, headHeight, 0.0f));

	// スティック入力取得
	float rx = _player->rx;
	float ry = _player->ry;
	// デットゾーン
	float deadZone = _player->analogMin;

	//何も入力されていないとき
	const float returnRate = 0.12f; 

	// 入力があるときは通常更新、ないときは既定方向へ戻す
	if(fabs(rx) > deadZone || fabs(ry) > deadZone)
	{
		//YaW更新
		if (fabs(rx) > deadZone)
		{
			yaw += -rx * yawSpeed;
		}

		//Pitch更新
		if (fabs(ry) > deadZone)
		{
			pitch += -ry * pitchSpeed;
		}
	}
	//Pitch制限
	if(pitch > maxPitch) pitch = maxPitch; // 上限
	if(pitch < minPitch) pitch = minPitch; // 下限

	//yaw, pitchから前方ベクトルを算出
	VECTOR forward;
	forward.x = sinf(yaw) * cosf(pitch);
	forward.y = sin(pitch);
	forward.z = -cos(yaw) * cosf(pitch);

	// 注視点更新
	_vTarget = VGet(
		_vPos.x + forward.x * lookDist,
		_vPos.y + forward.y * lookDist,
		_vPos.z + forward.z * lookDist
	);
}

// Y軸回転（左手系）
VECTOR fpsCamera::RotateYLocal(const VECTOR& v, float yaw)
{
	float c = cosf(yaw);
	float s = sinf(yaw);
	VECTOR r;
	r.x = v.x * c - v.z * s;
	r.y = v.y;
	r.z = v.x * s + v.z * c;
	return r;
}