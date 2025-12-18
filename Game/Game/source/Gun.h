#pragma once
#include "Player.h"
class Gun : public Player
{
	public:
	typedef Player Cbase;

	// 基本関数
	virtual bool Initialize()override;     // 初期化
	virtual bool Terminate() override;     // 終了
	virtual bool Process(int key, int trg);// 更新処理
	virtual bool Render()    override;     // 描画処理
	
	//playerに装備させる(プレイヤーと同じ操作をする)の設定
	void AttachToPlayer(Player* _player);
	void Detach();                // プレイヤーから外す

	//スケール操作
	float GetScale() const     { return _scale ; }
	void SetScale(float scale) { _scale = scale; }

	//　回転取得
	VECTOR GetRotation() const;
	void   SetRotation(const VECTOR& rot);
	//  Y軸回りにベクトルを回転させる関数
	static VECTOR RotateY(const VECTOR& v, float yaw);
	VECTOR _gunPos = VGet(0.5f, 100.f, -40.0f); // 銃の位置調整用
protected:

	Player* _player = nullptr;                // 装備しているプレイヤーへのポインタ
	

	// 3Dモデル描画用
	int _gunHandle;
	int _gunAttach_index;
	float _gunTotal_time;
	float _Gun_time;

	// モデルスケール（1.0 が原寸）
	float _scale = 1.0f;

	int _rotation;

	MATRIX _baseMatrix;
};

