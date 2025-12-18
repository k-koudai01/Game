#include "Bullet.h"
#include "mymath.h"	

Bullet::Bullet()
{
	Initialize();
}	

Bullet::~Bullet()
{
	Terminate();
}

bool Bullet::Initialize()
{
	_active = false;               // 非アクティブに設定
	_speed = 1000.0f;		       // 速度初期化
	_vPos = VGet(0, 0, 0);		   // 位置初期化
	_dir = VGet(0, 0, 1);		   // 方向初期化
	_life = 3.0f;				   // 寿命初期化
	timeAlive = 0.0f;			   // 生存時間初期化
	_length = 50.0f;			   // 長さ初期化
	_color = GetColor(255, 255, 0);// 色初期化
	_lastTimeMs = GetNowCount();   // 現在時間を取得
	return true;
}

bool Bullet::Initialize(const VECTOR& start, const VECTOR& dir, float speed, float life, float length, int color)
{
	_vPos = start;
	_dir = VecNormalize(dir); // 正規化
	_speed = speed;
	_life = life;
	_length = length;
	_color = (color == -1) ? GetColor(255, 255, 0) : color;
	timeAlive = 0.0f;
	_active = true;
	_lastTimeMs = GetNowCount();

	return true;
}

bool Bullet::Terminate()
{
	_active = false;
	return true;
}

bool Bullet::Process()
{
	if(!_active) return true;

	unsigned long now = GetNowCount();
	float dt = (now - _lastTimeMs) / 1000.0f; // 秒単位の経過時間
	_lastTimeMs = now;

	// 位置更新
	_vPos = VAdd(_vPos, VecScale(_dir, _speed * dt));

	//寿命計測
	timeAlive += dt;
	if(timeAlive >= _life)
	{
		_active = false; // 非アクティブ化
	}
	return true;
}

bool Bullet::Render(const VECTOR& cameraPos)
{
	if(!_active) return true;

	VECTOR end = VecAdd(_vPos, VecScale(_dir, _length));
	DrawThickLine3D(_vPos, end, _color, _thickness, cameraPos);
	return true;
}

void Bullet::DrawThickLine3D(const VECTOR& start, const VECTOR& end, int color, float thickness, const VECTOR& cameraPos)
{
	//線方向（ベクトルの正規化）
	VECTOR dir = VNorm(VGet(end.x - start.x, end.y - start.y, end.z - start.z));

	//中点と視線
	VECTOR mid = VScale(VAdd(start, end), 0.5f);
	VECTOR view = VNorm(VGet(cameraPos.x - mid.x, cameraPos.y - mid.y, cameraPos.z - mid.z));

	// 幅ベクトル = dir × view
	VECTOR width;
	width.x = dir.y * view.z - dir.z * view.y;
	width.y = dir.z * view.x - dir.x * view.z;
	width.z = dir.x * view.y - dir.y * view.x;

	// 各成分を二乗して長さを計算
	float len = sqrt(width.x * width.x + width.y * width.y + width.z * width.z);
	if(len < 1e-6f)
	{
		width = VGet(1.0f, 0.0f, 0.0f); // 適当なベクトルを設定
		len = 1.0f;
	}

	// 幅ベクトルを正規化して半分の太さを掛ける
	float half = thickness * 0.5f;

	VECTOR p1 = VAdd(start, width);
	VECTOR p2 = VAdd(end  , width);
	VECTOR p3 = VSub(end  , width);
	VECTOR p4 = VSub(start, width);

	// 線の描画
	DrawTriangle3D(p1, p2, p3, color, TRUE);
	DrawTriangle3D(p1, p3, p4, color, TRUE);

}