#pragma once

class Moles
{
public:
	Moles();
	~Moles(void);
	bool Init(
			XMFLOAT3 startPosition,
			XMFLOAT3 endPosition,
			XMFLOAT3 speedAscend,
			XMFLOAT3 speedDescend,
			XMFLOAT3 speedHit,
			float restAtTopTime,
			float maxMolesMoving,
			float timeBetweenUpdates,
			float timeBetweenMolePop,
			XMFLOAT3 startMolePosition,
			XMFLOAT3 moleSpacing,
			UINT moleColumns,
			UINT moleRows,
			float moleRadius);
	void UpdateMoles(float dt);
	int MolePicked(FXMVECTOR Origin, FXMVECTOR Direction);
	void HitMole(UINT moleIndex);
	XMFLOAT3 GetMolePosition(UINT moleIndex);
	UINT GetMoleCount();
	Mole GetMoleAtPosition(UINT moleIndex);

private:
	float mTimeLapsedUpdate; //the time lapsed since the beginning of initilization
	float mTimeLapsedPop; //the time lapsed since the beginning of initilization
	XMFLOAT3 mStartPosition;
	XMFLOAT3 mEndPosition;
	XMFLOAT3 mSpeedAscend;
	XMFLOAT3 mSpeedDescend;
	XMFLOAT3 mSpeedHit;
	float mRestAtTopTime;
	float mMaxMolesMoving;
	float mTimeBetweenUpdates;
	float mTimeBetweenMolePop;
	float currMolePopTime;
	UINT mMoleColumns;
	UINT mMoleRows;
	UINT mMoleCount;
	std::vector<Mole> mMoles;

	int MolesInMotionCount();

};
