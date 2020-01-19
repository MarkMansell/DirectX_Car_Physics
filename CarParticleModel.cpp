#include "CarParticleModel.h"
#include <math.h>

CarParticleModel::CarParticleModel(SGnode* model) : ParticleModel(model)
{
	//mVelocity = XMFLOAT3(0.5f, 0.5f, 0.5f);
	mMaxSpeed = 8.0f;
	mCarMaxSpeed = XMFLOAT3(0.015f, 0.0f, 8.0f); // Steering, Up, Fowards
	mCarSpeed = 0.0f;
	mCarVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mCarAcceleration = XMFLOAT3(4.0f, 1.0f, 1.0f); // Steering, Up, Fowards
	
}


CarParticleModel::~CarParticleModel()
{
}

float CarParticleModel::CalcWheelSpeed(float delta)
{
	float radius = 0.05f;
	float wheelAngle = 0.0f;
	wheelAngle += XM_PI / radius / 360 * delta;
	wheelAngle *= mCarVelocity.z;
	return wheelAngle * 10;
}



void CarParticleModel::Update(float t)
{
	ParticleModel::UpdateNetForce();
	// Update all the forces on the car.
	XMFLOAT3 temp = _Model->_model->GetTranslateVector();
	temp.x += mGravity.x * _mass * t;
	temp.y += mGravity.y * _mass * t;
	temp.z += mGravity.z * _mass * t;

	if (temp.y < 3.0f)
	{
		temp.y = 3.0f;
	}

	_Model->_model->SetTranslateVector(temp);
}



void CarParticleModel::MoveFowards(float delta, float accelfactor)
{
	mCarVelocity.z += ((accelfactor * mCarMaxSpeed.z) - mCarVelocity.z) * mCarAcceleration.z * delta;
	XMFLOAT3 dir = _Model->_model->GetTranslateVector();

	dir.x += mCarVelocity.z * sin(_Model->_model->_rotateAngles.y);
	dir.z += mCarVelocity.z * cos(_Model->_model->_rotateAngles.y);
	
	_Model->_model->SetTranslateVector(dir);
}

void CarParticleModel::Turn(float delta, float accelfactor)
{
	mCarVelocity.x += ((accelfactor * mCarMaxSpeed.x) - mCarVelocity.x) * mCarAcceleration.z * delta;
	if (mCarVelocity.z > 0.1)
	{
		_Model->_model->_rotateAngles.y += mCarVelocity.x;
	}
	else if (mCarVelocity.z < -0.1)
	{
		_Model->_model->_rotateAngles.y -= mCarVelocity.x;
	}
}

void CarParticleModel::CollisionResponse( XMFLOAT3 pos, float delta)
{
	XMFLOAT3 carpos = _Model->_model->GetTranslateVector();
	XMFLOAT3 dir;
	dir.x = pos.x - carpos.x;
	dir.y = pos.y - carpos.y;
	dir.z = pos.z - carpos.z;

	float len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
	dir.x /= len;
	dir.y /= len;
	dir.z /= len;

	XMFLOAT3 newpos = _Model->_model->OldVector;

	newpos.x -= dir.x * 200 * _mass *delta; 
	newpos.z -= dir.z * 200 * _mass *delta;
	
	_Model->_model->SetTranslateVector(newpos);
}