#pragma once
#include <directxmath.h>
#include <d3d11_1.h>
#include "Collision.h"
#include <vector>
#include <math.h>
#include "Model.h"

using namespace DirectX;
using namespace std;

enum PhysicsMode
{
	PHYSICS_NONE,
	PHYSICS_VELOCITY,
	PHYSICS_ACCELERATION,
	PHYSICS_MULTIFORCE,
	PHYSICS_SLIDING
};

class ParticleModel
{
public:
	ParticleModel(SGnode* transform);
	~ParticleModel();

	void Update(float t);
	void move(float AmountX, float AmountY, float AmountZ);

	XMFLOAT3 GetVelocity() { return mVelocity; }
	void SetVelocity(XMFLOAT3 velocity) { mVelocity = velocity; }
	void SetVelocity(float x, float y, float z) { mVelocity.x = x; mVelocity.y = y; mVelocity.z = z; }

	XMFLOAT3 GetAcceleration() { return mAcceleration; }
	void SetAcceleration(XMFLOAT3 Acceleration) { mAcceleration = Acceleration; }
	void SetAcceleration(float x, float y, float z) { mAcceleration.x = x; mAcceleration.y = y; mAcceleration.z = z; }

	XMFLOAT3 GetEngineForce() { return mEngineForce; }
	void SetEngineForce(XMFLOAT3 engineforce) { mEngineForce = engineforce; }

	XMFLOAT3 GetBrakeForce() { return mBrakeForce; }
	void SetBrakeForce(XMFLOAT3 brakeforce) { mBrakeForce = brakeforce; }

	XMFLOAT3 GetFrictionForce() { return mFrictionForce; }
	void SetFrictionForce(XMFLOAT3 frictionforce) { mFrictionForce = frictionforce; }

	XMFLOAT3 GetTransform() { return _Model->_model->_translateVector; }

	void PhysicsMove(float t);

	float getMass(){ return _mass; }
	void setMass(float mass) { _mass = mass; }

	void SetSlideOrientation(XMFLOAT3 slideangle) { mSlideOrientation = slideangle; }
	void SetSlideOrientation(float x, float y, float z) { mSlideOrientation.x = x, mSlideOrientation.y = y, mSlideOrientation.z = z; }
	XMFLOAT3 GetSlideOrientation() { return mSlideOrientation; }

	void SetPhysicsMode(PhysicsMode mode){ mPhysicsMode = mode; }
	PhysicsMode GetPhysicsMode(){ return mPhysicsMode; }

	void SlidingForce(float theta, float frCoef);
	void SlidingMotion(float t);

	void MotionFluid(float t);
	void DragForce(XMFLOAT3 velocity, float dragfactor);
	void DragLamFlow(XMFLOAT3 velocity, float dragfactor);
	void DragTurbFlow(XMFLOAT3 velocity, float dragfactor);
		
	void UpdateNetForce();
	void UpdateAcceleration();
	void UpdateState(float t);

	BSphere* GetBoundingSphere() { return mBoundingSphere; }

	void ReverseVelocity();

	//Methods for Vectors
	float GetMagnitude(XMFLOAT3 vector);
	XMFLOAT3 NormalizeVector(XMFLOAT3 vector);

	SGnode* GetModel() { return _Model; }

protected:
	SGnode * _Model;
	BSphere* mBoundingSphere;
	bool mCollsionOn;

	PhysicsMode mPhysicsMode;

	XMFLOAT3 mSlideOrientation;

	//Forces
	XMFLOAT3 mVelocity;
	XMFLOAT3 mAcceleration;
	XMFLOAT3 mFrictionForce;
	XMFLOAT3 mBrakeForce;
	XMFLOAT3 mEngineForce;
	XMFLOAT3 mNetForce;
	XMFLOAT3 mDragforce; // add to net force
	XMFLOAT3 mGravity;

	float mTheTa;
	float mFrCoef;
	float mForceMag;
	XMFLOAT3 mSlideForce;

	vector<XMFLOAT3*> mForce;
	XMFLOAT3 mForces[3];
	bool mLaminar;
	float _mass;

	void moveConstVelocity(float t);
	void moveConstAcceleration(float t);

	

};

