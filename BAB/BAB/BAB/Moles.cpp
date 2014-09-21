#include "stdafx.h"

XMFLOAT3 ZER0_VELOCITY(0.0f,0.0f,0.0f);
Moles::Moles(void)
{
}

Moles::~Moles(void)
{
	mMoles.clear();
}

bool Moles::Init(
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
		float moleRadius)
{
	mTimeLapsedUpdate =0; //the time lapsed since the beginning of initilization
	mTimeLapsedPop = 0;
	mStartPosition = startPosition;
	mEndPosition = endPosition;
	mSpeedAscend = speedAscend;
	mSpeedDescend = speedDescend;
	mSpeedHit = speedHit;
	mRestAtTopTime = restAtTopTime;
	mMaxMolesMoving = maxMolesMoving;
	mTimeBetweenUpdates = timeBetweenUpdates;
	mTimeBetweenMolePop =timeBetweenMolePop;
	currMolePopTime =mTimeBetweenMolePop;

	mMoleColumns = moleColumns;
	mMoleRows = moleRows;
	mMoleCount = moleColumns*moleRows;
	std::vector<Mole> newMoles(mMoleCount);
	mMoles = newMoles;
	for(UINT i=0;i<mMoleColumns;i++)
	{
		for(UINT j= 0;j<mMoleRows;j++)
		{
			mMoles[i*mMoleColumns+j].mPosition= XMFLOAT3((j+1)*  +moleSpacing.x + startMolePosition.x,
														startMolePosition.y,
														(i+1)*  +moleSpacing.z +startMolePosition.z);
			mMoles[i*mMoleColumns+j].mVelocity=ZER0_VELOCITY;
			mMoles[i*mMoleColumns+j].mInPauseAtTop = false;
			mMoles[i*mMoleColumns+j].mRestAtTopTime = 0;
			mMoles[i*mMoleColumns+j].mRadius  = moleRadius;
		}
	}
	return true;
}

void Moles::UpdateMoles(float dt)
{
	mTimeLapsedPop +=dt;
	mTimeLapsedUpdate +=dt;

	if(mTimeLapsedUpdate > mTimeBetweenUpdates)
	{	
		for(UINT i=0;i<mMoleCount;i++)
		{
			if(mMoles[i].mPosition.y >=mEndPosition.y)
			{	//at the top in the pause
				mMoles[i].mInPauseAtTop=true;
				mMoles[i].mRestAtTopTime += mTimeLapsedUpdate;
				mMoles[i].mVelocity=ZER0_VELOCITY;
			}
			if(mMoles[i].mRestAtTopTime>=mRestAtTopTime)
			{ //at the top after pause ...going down
				mMoles[i].mInPauseAtTop=false;
				mMoles[i].mVelocity = mSpeedDescend;
				mMoles[i].mRestAtTopTime=0;
			}
			if(mMoles[i].mPosition.y < mStartPosition.y)
			{	//at the bottom
				mMoles[i].mVelocity=ZER0_VELOCITY;
				mMoles[i].mPosition = mStartPosition;
			}
			mMoles[i].mPosition.x = mMoles[i].mPosition.x + mMoles[i].mVelocity.x;
			mMoles[i].mPosition.y = mMoles[i].mPosition.y + mMoles[i].mVelocity.y;
			mMoles[i].mPosition.z = mMoles[i].mPosition.z + mMoles[i].mVelocity.z;
		}
		mTimeLapsedUpdate=0;
	}
	

	if(mTimeLapsedPop > currMolePopTime)
	{	
		mTimeLapsedPop=0;
		bool acceptablePick=false;
		int indexPick=0;
		int guessPick=0;
		while(!acceptablePick && MolesInMotionCount()< mMaxMolesMoving)
		{
			indexPick = (int)MathHelper::RandF(0,(float)mMoleCount-0.5f);
			if(	mMoles[indexPick].mVelocity.x ==ZER0_VELOCITY.x &&
				mMoles[indexPick].mVelocity.y ==ZER0_VELOCITY.y &&
				mMoles[indexPick].mVelocity.z ==ZER0_VELOCITY.z )
			{
				acceptablePick=true;
				mMoles[indexPick].mVelocity = mSpeedAscend;
			}
		}
		currMolePopTime=MathHelper::RandF(mTimeBetweenMolePop/2,mTimeBetweenMolePop*2);
	}
}

int Moles::MolesInMotionCount()
{
	int molesInActionCount=0;
	for(UINT i=0;i<mMoleCount;i++)
		if(	mMoles[i].mVelocity.x != ZER0_VELOCITY.x  ||
			mMoles[i].mVelocity.y != ZER0_VELOCITY.y  ||
			mMoles[i].mVelocity.z != ZER0_VELOCITY.z  )
			molesInActionCount++;
	return molesInActionCount;
}


int Moles::MolePicked(FXMVECTOR Origin, FXMVECTOR Direction)
{
	for(UINT i=0;i<mMoleCount;i++)
	{
		XNA::Sphere tmpSphere;
		tmpSphere.Radius = mMoles[i].mRadius;
		Mole tmpMole = GetMoleAtPosition(i);
		tmpSphere.Center = XMFLOAT3(tmpMole.mPosition.x,tmpMole.mPosition.y,tmpMole.mPosition.z);
		float tmpDist;
		if(XNA::IntersectRaySphere( Origin, Direction, &tmpSphere, &tmpDist ))
		{
			return i;
		}
	}
	return -1;
}


void Moles::HitMole(UINT moleIndex)
{
	if(moleIndex>=mMoles.size() || moleIndex<0)
		return;
	mMoles[moleIndex].mInPauseAtTop=false;
	mMoles[moleIndex].mVelocity= mSpeedHit;
	mMoles[moleIndex].mRestAtTopTime=0;
	mMoles[moleIndex].mPosition.x = mMoles[moleIndex].mPosition.x + mMoles[moleIndex].mVelocity.x;
	mMoles[moleIndex].mPosition.y = mMoles[moleIndex].mPosition.y + mMoles[moleIndex].mVelocity.y;
	mMoles[moleIndex].mPosition.z = mMoles[moleIndex].mPosition.z + mMoles[moleIndex].mVelocity.z;
}

XMFLOAT3 Moles::GetMolePosition(UINT moleIndex)
{
	if(moleIndex>=mMoles.size() || moleIndex<0)
		return XMFLOAT3(0,0,0);
	return mMoles[moleIndex].mPosition;
}

UINT Moles::GetMoleCount()
{
	return mMoleCount;
}

Mole Moles::GetMoleAtPosition(UINT moleIndex)
{
if(moleIndex>=mMoles.size() || moleIndex<0)
		return Mole();
	return mMoles[moleIndex];
}
