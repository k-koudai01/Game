#include "AppFrame.h"
#include "ApplicationMain.h"
#include "ModeGame.h"
#include "CharaBase.h"


bool CharaBase::Initialize()
{
	return true;
}

bool CharaBase::Terminate()
{
	return true;
}	

bool CharaBase::Process()
{
	return true;
}

bool CharaBase::Render()
{
	return true;
}

bool CharaBase::IsHitCircle(CharaBase* c1, CharaBase* c2)
{
	float w, h, length;
	w = c1->_vPos.x - c2->_vPos.x;
	h = c1->_vPos.z - c2->_vPos.z;
	length = (int)sqrt((float)(w * w + h * h));

	if(length < c1->_collision_r + c2->_collision_r)
	{
		return true;
	}
	return false;
}

