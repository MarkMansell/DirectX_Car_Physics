#pragma once
#include "ParticleModel.h"

class CarParticleModel : public ParticleModel
{
public:
	CarParticleModel(SGnode* model);
	~CarParticleModel();

	//void SetEngineSpeed(XMFLOAT3 speed);
	float CalcWheelSpeed(float delta);

	void Update(float t);

	void SetEngineSpeed(float speed){ mEngineSpeed = speed; }
	void MoveFowards(float delta, float accelfactor);
	void Turn(float angle, float accelfactor);

	void CollisionResponse(XMFLOAT3 pos, float delta);
	XMFLOAT3 GetCarVelocity(){ return mCarVelocity; }
	XMFLOAT3 mCarAcceleration;
	XMFLOAT3 mCarMaxSpeed;
private:
	float mWheelRadius;
	float mEngineSpeed;
	float mCarSpeed;
	float mMaxSpeed;
	XMFLOAT3 mCarVelocity;

};

