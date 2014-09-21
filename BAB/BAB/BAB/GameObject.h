#pragma once

class GameObject
{
public:
	GameObject(void);
	GameObject(XMFLOAT3 newPos,XMFLOAT3 newVelocity);
	~GameObject(void);
	XMFLOAT3 mPosition;
	XMFLOAT3 mVelocity;
};
