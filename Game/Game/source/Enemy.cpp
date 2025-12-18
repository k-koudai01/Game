#include "Enemy.h"
#include "mymath.h"
#include "Player.h"
#include "DxLib.h"

bool Enemy::Initialize()
{
	// 基底クラスの初期化
	_handle = MV1LoadModel("res/SDChar/SDChar.mv1");
	_attach_index = -1;
	_status = STATUS::NONE;
	_total_time = 0.f;
	_play_time = 0.f;
	_colSubY = 40.0f;
	speed = 1.5f;
	// 位置,向きの初期化
	_vPos = VGet(200, 0, 0);
	_vDir = VGet(0, 0, -1);
	//コリジョン初期化
	_collision_weight = 10.f;
	_collision_r = 30.f;
	return true;
}

bool Enemy::Terminate()
{
	//動作的に確保した弾を開放
	_bullets.clear();
	if(_attach_index != -1)
	{
		MV1DetachAnim(_handle, _attach_index);
		_attach_index = -1;
	}
	if(_handle != -1)
	{
		MV1DeleteModel(_handle);
		_handle = -1;
	}
	return true;
}

bool Enemy::Process()
{
	unsigned long now = GetNowCount();
	float         dt  = (now - _lastTimeMs) / 1000.0f;
	_lastTimeMs		  =  now;
	_shootTimer		 += dt;

	// 基底クラスの更新処理
	STATUS oldStatus = _status;
	if(_target)
	{
		VECTOR toTarget = VSub(_target->GetPos(), _vPos);
		float dist = VSize(toTarget);

		const float chaseDistance = 3000.0f; // 追跡距離
		const float  stopDistance = 50.0f;   // 停止距離
		if(dist < chaseDistance && dist > stopDistance)
		{
			VECTOR dir = VNorm(toTarget);
			_v		   = VScale(dir, speed);
			_vPos	   = VAdd(_vPos, _v);
			_vDir	   = dir;
			_status	   = STATUS::WALK;
		}
		else
		{
			_v = VGet(0, 0, 0);
			_status = STATUS::WAIT;
		}
		for(int i = (int)_bullets.size() - 1; i >= 0; --i)
		{
			Bullet* b = _bullets[i];
			if(b)
			{
				b->Process();
				if(!b->IsActive())
				{
					b->Terminate();
					delete b;
					_bullets.erase(_bullets.begin() + i);
				}
			}
		}
		
	}
	else
	{
		_status = STATUS::WAIT;
	}

	if(oldStatus == _status)
	{
		_play_time += 0.5f;
	}
	else
	{
		if(_attach_index != -1)
		{
			MV1DetachAnim(_handle, _attach_index);
			_attach_index = -1;
		}
		switch(_status)
		{
			case STATUS::WAIT:
				_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "idle"), -1, FALSE);
				break;
			case STATUS::WALK:
				_attach_index = MV1AttachAnim(_handle, MV1GetAnimIndex(_handle, "run"), -1, FALSE);
				break;
			default:
				break;
		}
		_total_time = MV1GetAttachAnimTotalTime(_handle, _attach_index);
		_play_time = 0.0f;
	}
	if(_play_time >= _total_time)
	{
		_play_time = 0.0f;
	}
	return true;
}

bool Enemy::Render()
{
	if(_handle == -1) return true;
	MV1SetAttachAnimTime(_handle, _attach_index, _play_time);

	MV1SetPosition(_handle, _vPos);
	VECTOR vRot = { 0, 0, 0 };
	if(VSize(_vDir) != 0.0f)
	{
		vRot.y = atan2(_vDir.x * -1, _vDir.z * -1);
	}
	MV1SetRotationXYZ(_handle, vRot);
	MV1DrawModel(_handle);

	//弾の描画
	VECTOR camPos = VGet(0, 0, 0);
	if(_target && _target->_cam)
	{
		camPos = _target->_cam->GetPos();
	}
	return true;
}
