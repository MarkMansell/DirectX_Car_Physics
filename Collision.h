#pragma once
#include <directxmath.h>
#include <d3d11_1.h>

using namespace DirectX;

class BSphere
{
public:
	BSphere(XMFLOAT3* centre, float radius) { mCentre = centre; mRadius = radius; mCollided = false; }
	~BSphere();	
	void Update();
	
	float GetBoundingRadius() { return mRadius; }
	bool GetCollision(){ return mCollided; }
	XMFLOAT3* GetCentre() { return mCentre; }
	
private:
	XMFLOAT3* mCentre;
	float mRadius;
	bool mCollided;
};

class Collision
{
public:
	static bool SphereToSphereCollision(BSphere *s1, BSphere *s2)
	{
			XMFLOAT3 S1Centre = *s1->GetCentre();
			XMFLOAT3 S2Centre = *s2->GetCentre();
			float dx = S2Centre.x - S1Centre.x;
			float dy = S2Centre.y - S1Centre.y;
			float dz = S2Centre.z - S1Centre.z;

			float dSquared = dx*dx + dy*dy + dz *dz;

			float sumOfBoundingRadius = s1->GetBoundingRadius() + s2->GetBoundingRadius();

			if (dSquared < (sumOfBoundingRadius * sumOfBoundingRadius))
			{
				return true;
			}

		return false;
	}


};





