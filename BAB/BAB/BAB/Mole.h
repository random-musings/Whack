#pragma once


class Mole: public GameObject
{
public:
	Mole(void);
	Mole(XMFLOAT3 newPos, XMFLOAT3 newVelocity);
	~Mole(void);

	bool mInPauseAtTop; 
	float mRestAtTopTime;
	float mRadius;
};
