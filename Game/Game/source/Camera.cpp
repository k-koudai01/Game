#include "Camera.h"
#include "mymath.h"
#include "ApplicationMain.h"

bool Camera::Initialize()
{
	_vPos = VGet(0, 90.f, -300.f);
	_vTarget = VGet(0, 60, 0);
	_clipNear = 2.0f;
	_clipFar = 10000.0f;
	return true;
}

bool Camera::Terminate()
{

	return true;
}

bool Camera::Process(int key, int trg)
{
	// プレイヤー追従更新
	if(!_player) return true;
	//アナログスティック対応
	UseStick();

	// デッドゾーン
	const float dead = _player->analogMin;

	//右スティックでカメラ操作
	RightStyckControl();

	// カメラがターゲットを貫通しないよう最低高さを保証する
	if(_vPos.y < _vTarget.y + _minAboveTarget)
	{
		_vPos.y = _vTarget.y + _minAboveTarget;
	}
	if(_vPos.y > _vTarget.y + _maxAboveTarget)
	{
		_vPos.y = _vTarget.y + _maxAboveTarget;
	}
	return true;
}

bool Camera::Render()
{
	// カメラ設定更新
	SetCameraPositionAndTarget_UpVecY(_vPos, _vTarget);
	SetCameraNearFar(_clipNear, _clipFar);


	// カメラターゲットを中心に短い線を引く
	{
		float linelength = 10.f;
		VECTOR v = _vTarget;
		DrawLine3D(VAdd(v, VGet(-linelength, 0, 0)), VAdd(v, VGet(linelength, 0, 0)), GetColor(255, 0, 0));
		DrawLine3D(VAdd(v, VGet(0, -linelength, 0)), VAdd(v, VGet(0, linelength, 0)), GetColor(0, 255, 0));
		DrawLine3D(VAdd(v, VGet(0, 0, -linelength)), VAdd(v, VGet(0, 0, linelength)), GetColor(0, 0, 255));
	}


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

	return true;
}	

void Camera::MoveBy(const VECTOR& vMove)
{
	_vPos = VAdd(_vPos, vMove);
	_vTarget = VAdd(_vTarget, vMove);
}

void Camera::RightStyckControl()
{
	// Y軸回転
	float sx = _vPos.x - _vTarget.x;
	float sz = _vPos.z - _vTarget.z;
	float rad = atan2(sz, sx);
	float length = sqrt(sz * sz + sx * sx);
	if (_player->rx > _player->analogMin) { rad -= 0.05f; }
	if (_player->rx < -_player->analogMin) { rad += 0.05f; }
	_vPos.x = _vTarget.x + cos(rad) * length;
	_vPos.z = _vTarget.z + sin(rad) * length;

	// Y位置
	if (_player->ry > _player->analogMin) { _vPos.y -= 5.f; }
	if (_player->ry < -_player->analogMin) { _vPos.y += 5.f; }
}

// カメラ追従更新
void Camera::FollowUpdate()
{
	if(!_player) return;

	//現在のカメラの角度と距離を保持して、ターゲットの見プレイヤーへの合わせる
	// 値の取得
	float sx = _vPos.x - _vTarget.x;
	float sz = _vPos.z - _vTarget.z;
	float rad = atan2(sz, sx);
	float length = sqrt(sz * sz + sx * sx);

	//プレイヤーの基準ターゲット位置
	VECTOR newTarget = VAdd(_player->GetPos(), VGet(0.f, _player->GetColSubY(), 0.f));
	_vTarget = newTarget;

	//同じ角度・距離を保って一を更新
	_vPos.x = _vTarget.x + cos(rad) * length;
	_vPos.z = _vTarget.z + sin(rad) * length;

	// Y位置はプレイヤーの上に一定量確保
	const float minAboveTarget = 10.f;     // ターゲットより何以上上にいるか（下限）
	if(_vPos.y < _vTarget.y + minAboveTarget)
	{
		_vPos.y = _vTarget.y + minAboveTarget;
	}
}

bool Camera::UseStick()
{
	if(!_player) return true;
	// アナログスティック対応
	DINPUT_JOYSTATE di;
	// 正常に取得できた時だけ値を反映する（GetJoypadDirectInputState は 0 を返すと成功）
	if(GetJoypadDirectInputState(DX_INPUT_PAD1, &di) == 0)
	{
		// 左スティック
		_player->lx = (float)di.X / 1000.0f;
		_player->ly = (float)di.Y / 1000.0f;

		// 右スティック（環境によっては Z/Rz を使うことが多いのでそれを採用）
		_player->rx = (float)di.Z / 1000.0f;
		_player->ry = (float)di.Rz / 1000.0f;

		_player->di = di;
	}
	return true;
}