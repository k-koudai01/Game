#pragma once
#include "CharaBase.h"
#include "Camera.h"
#include "Bullet.h"
class Camera;
class Bullet;
class Gun;

class Player: public CharaBase
{
	typedef CharaBase Cbase;
public:
	virtual bool Initialize()override;
	virtual bool Terminate() override;
	virtual bool Process(int key, int trg);
	virtual bool Render()    override;

	float GetColSubY() const { return _colSubY; }
	VECTOR GetOldPos() const { return oldPos; }
	VECTOR GetMoveV()  const { return _v; }
	VECTOR SetMoveV(const VECTOR& v) { return _v = v; }

	void SetCamera(Camera* cam) { _cam = cam; }
	
	// fpsとtpsの切り替え
	void OnSwitchToFPS();
	void OnSwitchToTPS();

	float _colSubY;  // コリジョン用Yオフセット
	VECTOR oldPos;
	VECTOR _v;
	float playerSpeed;

	//アナログスティック関係
	float analogMin; // アナログスティックの無効範囲
	float lx, ly, rx, ry;
	DINPUT_JOYSTATE di;

	// FPSモードかどうか
	bool isFps = false;

	//カメラ
	Camera* _cam;

	//銃（弾）
	Bullet* _bullet;

protected:
	
	// 3Dモデル描画用
	int _handle;
	int _attach_index;
	float _total_time;
	float _play_time;

	
	
	enum class STATUS
	{
		NONE,
		WAIT,
		WALK,
		_EOT_
	};
	STATUS _status;
};

