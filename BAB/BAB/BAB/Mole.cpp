#include "stdafx.h"

Mole::Mole(void):GameObject()
{
	mInPauseAtTop=false;
}

Mole::Mole(XMFLOAT3 newPos, XMFLOAT3 newVelocity)
:GameObject(newPos,newVelocity)
{
	mInPauseAtTop=false;
}

Mole::~Mole(void)
{
}
