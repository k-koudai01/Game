#pragma once
#include "DxLib.h"
#include "Camera.h"

class Camera;
class Bullet
{
public:
	// 基本関数
	Bullet();
	virtual ~Bullet();

	// デフォルト初期化（リセット / 非アクティブ）
	virtual bool Initialize();

	// 明示的に発射する初期化
	virtual bool Initialize(const VECTOR& start, const VECTOR& dir, float speed = 200.0f, float life = 3.0f, float length = 50.0f, int color = -1);    // 初期化
	virtual bool Terminate();     // 終了
	virtual bool Process();       // 更新処理
	virtual bool Render(const VECTOR& cameraPos);        // 描画処理

	// ヘルパー
	void Activate(const VECTOR& start, const VECTOR& dir, float speed = 200.0f, float life = 3.0f, float length = 50.0f, int color = -1) { Initialize(start, dir, speed, life, length, color); }
	void Deactive() { _active = false; }
	bool IsActive() const { return _active; }

	//3D太線分描画関数
	void DrawThickLine3D(const VECTOR& start, const VECTOR& end, int color, float thickness, const VECTOR& cameraPos);

	//変数
	float _speed    = 100.0f;
	VECTOR _vPos    = VGet(0, 0, 0);
	VECTOR _dir     = VGet(0, 0, 1);
	float _life     = 3.0f;
	float timeAlive = 0.0f;
	float _length   = 50.0f;
	int   _color	= GetColor(255, 255, 0);
	bool  _active   = false;
	float _thickness = 2.0f; //弾の太さ
	unsigned long _lastTimeMs = 0;

	
};

