#pragma once
#include "Camera.h"

class fpsCamera :public Camera
{
	typedef Camera Cbase;
public:
	// 基本関数
	virtual bool Initialize() override;
	virtual bool Terminate()  override;
	virtual bool Process(int key, int trg)override;
	virtual bool Render()override;

	void MoveBy(const VECTOR& vMove); // カメラをvMove分移動させる
	void RightStyckControl();		  // 右スティックでカメラ操作

	//Getter / Setter :Pos
	VECTOR SetPos(VECTOR Pos) { return _vPos = Pos; }
	VECTOR SetTarget(VECTOR Target) { return _vTarget = Target; }

	//  Y軸回転（左手系）
	VECTOR RotateYLocal(const VECTOR& v, float yaw);
	//　カメラ切り替え時の更新
	void AlignTargetToPlayerDirection();
	float headHeight = 95.0f;

	//一人称視点の右スティックにかかわる変数
	float yaw = 0.0f;
	float pitch = 0.0f;
	float yawSpeed = 0.045f;
	float pitchSpeed = 0.035f;
	float minPitch = -1.2f;
	float maxPitch = 1.2f;
	float smooth = 0.14f;
	float backOffset = 8.0f;
	float lookDist = 1000.0f; // 注視点までの距離
};

