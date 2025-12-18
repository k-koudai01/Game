#pragma once
#include "appframe.h"

#include <string>

class CharaBase
{
public:

	virtual bool Initialize();
	virtual bool Terminate();
	virtual bool Process();
	virtual bool Render();

	virtual VECTOR GetPos() const { return _vPos; }
	virtual VECTOR SetPos(const VECTOR& vPos) { return _vPos = vPos; }
	virtual VECTOR GetDir() const { return _vDir; }
	virtual VECTOR SetDir(const VECTOR& vDir) { return _vDir = vDir; }
	// ベクトル関係
	VECTOR _vPos;
	VECTOR _vDir;
	//コリジョン判定
	float _collision_r;
	float _collision_weight;
	bool IsHitCircle(CharaBase* c1, CharaBase* c2);
	bool IsHitCircle(CharaBase* c1, VECTOR point);
};

