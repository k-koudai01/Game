#pragma once

#include "appframe.h"
#include "Player.h"	
#include <string>

class Player;

// 1/1000の定数
constexpr float INV1000 = 0.001f;

class Camera
{
public:
	
	// 基本関数
	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process(int key, int trg);
	virtual bool Render();

	void MoveBy(const VECTOR& vMove); // カメラをvMove分移動させる
	virtual void RightStyckControl();		  // 右スティックでカメラ操作
	
	bool UseStick(); // スティック入力を使用する
	// カメラ追従更新
	virtual void FollowUpdate();

	//Getter / Setter :Pos
	VECTOR GetPos()        const { return _vPos; }
	VECTOR SetPos(VECTOR Pos)       { return _vPos = Pos; }
	VECTOR SetTarget(VECTOR Target) { return _vTarget = Target; }

	Player* _player = nullptr; // カメラが追従するプレイヤー


	VECTOR _vPos;				 // 位置
	VECTOR _vTarget;			 // 距離
	float  _clipNear, _clipFar;	 // クリップ

	float _minAboveTarget = 10.0f;   // 下限（ターゲットより何以上上にいるか）
	float _maxAboveTarget = 300.0f;  // 上限（ターゲットより何以下にいるか）

};

