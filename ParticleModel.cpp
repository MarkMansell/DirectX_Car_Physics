#include "ParticleModel.h"


ParticleModel::ParticleModel(SGnode* model) : _Model(model)
{
	mVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mFrictionForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mEngineForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mBrakeForce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mDragforce = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mGravity = XMFLOAT3(0.0f, -9.81f, 0.0f);

	mForces[0] = mFrictionForce;
	mForces[1] = mEngineForce;
	mForces[2] = mBrakeForce;

	mPhysicsMode = PHYSICS_NONE;

	mNetForce = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mSlideForce = XMFLOAT3(0.0f, 0.0f, 0.0f);

	mForce.push_back(&mFrictionForce);
	mForce.push_back(&mEngineForce);
	mForce.push_back(&mBrakeForce);
	mForce.push_back(&mSlideForce);
	mForce.push_back(&mDragforce);
	mCollsionOn = true;
	_mass = 1.0f;
}


ParticleModel::~ParticleModel()
{

}





void ParticleModel::Update(float t)
{
	switch (mPhysicsMode)
	{
	case PHYSICS_NONE:
		break;
	case PHYSICS_VELOCITY:
		moveConstVelocity(t);
		break;
	case PHYSICS_ACCELERATION: 
		moveConstAcceleration(t);
		break;
	case PHYSICS_MULTIFORCE:
		UpdateState(t);
		break;
	case PHYSICS_SLIDING:
		SlidingMotion(t); // Fix in tutorial
		break;
	default:
		break;
	}
}

void ParticleModel::UpdateState(float t)
{
	UpdateNetForce();
	UpdateAcceleration();
	PhysicsMove(t);
}

void ParticleModel::UpdateNetForce()
{
	mNetForce = XMFLOAT3(0.0f, 0.0f, 0.0f);

	for (vector<XMFLOAT3*>::iterator i = mForce.begin(); i != mForce.end(); i++)
	{
		mNetForce.x += (*i)->x;
		mNetForce.y += (*i)->y;
		mNetForce.z += (*i)->z;
	}
}

void ParticleModel::UpdateAcceleration()
{
	mAcceleration.x = mNetForce.x / _mass;
	mAcceleration.y = mNetForce.y / _mass;
	mAcceleration.z = mNetForce.z / _mass;
}

void ParticleModel::SlidingMotion(float t)
{
	//Calculate the sliding force
	SlidingForce(XMConvertToRadians(70.0f), 0.8f);
	UpdateState(t);
}

void  ParticleModel::SlidingForce(float theta, float frCoef)
{
	float Gravity = -9.81f;

	//Calculate magnitude of force
	mForceMag = _mass * Gravity * (sin(theta) - frCoef * cos(theta));

	mSlideForce.x = mForceMag * cos(theta);
	mSlideForce.y = mForceMag * sin(theta);
	mSlideForce.z = 0.0f;

	if (mSlideForce.y > 0.0f)
	{
		mSlideForce.x = 0.0f;
		mSlideForce.y = 0.0f;
	}
}
// ----------------------------------------------------------------------Constant Velocity Function--------------------------------------------------------------------
void ParticleModel::moveConstVelocity(float t)
{
	float timeStop = t;

	XMFLOAT3 position = _Model->_model->_translateVector;

	position.x = position.x + mVelocity.x * timeStop;
	position.y = position.y + mVelocity.y * timeStop;
	position.z = position.z + mVelocity.z * timeStop;

	_Model->_model->_translateVector = position;
}

void ParticleModel::moveConstAcceleration(float t)
{
	float timeStep = t;
	XMFLOAT3 position = _Model->_model->_translateVector;

	position.x = position.x + mVelocity.x * timeStep + 0.5f * mAcceleration.x * timeStep * timeStep;
	position.y = position.y + mVelocity.y * timeStep + 0.5f * mAcceleration.y * timeStep * timeStep;
	position.z = position.z + mVelocity.z * timeStep + 0.5f * mAcceleration.z * timeStep * timeStep;	

	mVelocity.x = mVelocity.x + mAcceleration.x * timeStep;
	mVelocity.y = mVelocity.y + mAcceleration.y * timeStep;
	mVelocity.z = mVelocity.z + mAcceleration.z * timeStep;
	
	//_Model->_model->_translateVector = position;
}


void ParticleModel::MotionFluid(float t)
{
	DragForce(mVelocity, 10.0f);
	UpdateState(t);
	PhysicsMove(t);
}

void ParticleModel::DragForce(XMFLOAT3 velocity, float dragfactor)
{
	if (mLaminar)
	{
		DragLamFlow(velocity, dragfactor);
	}
	else
	{
		DragTurbFlow(velocity, dragfactor);
	}
}

void ParticleModel::DragLamFlow(XMFLOAT3 velocity, float dragfactor)
{
	//Calculate Drag force
	mDragforce.x = dragfactor * velocity.x;
	mDragforce.y = dragfactor * velocity.y;
	mDragforce.z = dragfactor * velocity.z;
}

void ParticleModel::DragTurbFlow(XMFLOAT3 velocity, float dragfactor)
{
	float velMag;
	float DragMag;
	XMFLOAT3 UnitVel;
	velMag = GetMagnitude(velocity);
	UnitVel = NormalizeVector(velocity);
	
	DragMag = dragfactor * velMag * velMag;

	mDragforce.x = -DragMag * UnitVel.x;
	mDragforce.y = -DragMag * UnitVel.y;
	mDragforce.z = -DragMag * UnitVel.z;
}

void ParticleModel::ReverseVelocity()
{
	XMFLOAT3 tmp = mVelocity;
	tmp.x = tmp.x * -tmp.x;
	tmp.y = tmp.y * -tmp.y;
	tmp.z = tmp.z * -tmp.z;

	mVelocity = tmp;
}

void ParticleModel::PhysicsMove(float t)
{
	float timeStep = t;
	XMFLOAT3 position = _Model->_model->_translateVector;

	position.x = position.x + mVelocity.x * timeStep + 0.5f * mAcceleration.x * timeStep * timeStep;
	position.y = position.y + mVelocity.y * timeStep + 0.5f * mAcceleration.y * timeStep * timeStep;
	position.z = position.z + mVelocity.z * timeStep + 0.5f * mAcceleration.z * timeStep * timeStep;

	mVelocity.x = mVelocity.x + mAcceleration.x * timeStep;
	mVelocity.y = mVelocity.y + mAcceleration.y * timeStep;
	mVelocity.z = mVelocity.z + mAcceleration.z * timeStep;

	_Model->_model->_translateVector = position;
}


float ParticleModel::GetMagnitude(XMFLOAT3 vector)
{
	return sqrtf(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

XMFLOAT3 ParticleModel::NormalizeVector(XMFLOAT3 vector)
{
	XMFLOAT3 temp = vector;
	float Magnitude = GetMagnitude(vector);
	temp.x /= Magnitude;
	temp.y /= Magnitude;
	temp.z /= Magnitude;
	return temp;
}

