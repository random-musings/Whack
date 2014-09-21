#include "stdafx.h"

GameObject::GameObject(void)
	:mPosition(0,0,0),
	mVelocity(0,0,0)
{
}

GameObject::GameObject(XMFLOAT3 newPos,XMFLOAT3 newVelocity)
	:mPosition(newPos),
	mVelocity(newVelocity)
{
}

GameObject::~GameObject(void)
{
}
