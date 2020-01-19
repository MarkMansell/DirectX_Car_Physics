#include "CarAIController.h"


CarAIController::CarAIController() 
{
	mSpeed = 300.0f;
	mAvoidSpeed = 300.0f;
	mWayPointNum = 0;
}


CarAIController::~CarAIController()
{
}

void CarAIController::MoveTowards(XMFLOAT3* pos, float delta) 
{
	float epsilon = 10.0f;
	XMFLOAT3 dir;
	dir.x = pos->x - mAICar->_model->_translateVector.x;
	dir.y = pos->y - mAICar->_model->_translateVector.y;
	dir.z = pos->z - mAICar->_model->_translateVector.z;

	float len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
	dir.x /= len;
	dir.y /= len;
	dir.z /= len;


	mAICar->_model->_translateVector.x += dir.x * mSpeed * delta;
	mAICar->_model->_translateVector.z += dir.z * mSpeed * delta;

	float angle = atan2(mAICar->_model->_translateVector.x - pos->x, mAICar->_model->_translateVector.z - pos->z);
	mAICar->_model->_rotateAngles.y = angle;	

	if (abs(pos->x - mAICar->_model->_translateVector.x) <= epsilon && abs(pos->y - mAICar->_model->_translateVector.y) <= epsilon)
	{
		if (mWayPointNum >= mTrackPoints.size() - 1)
		{
			mWayPointNum = 0;
			return;
		}
		else
		{
			mWayPointNum++;
			return;
		}
	}
}

void CarAIController::Update(float delta)
{
	MoveTowards(&mTrackPoints[mWayPointNum], delta);
}

void CarAIController::AddWaypoint(XMFLOAT3 pos)
{
	mTrackPoints.push_back(pos);
}


void CarAIController::Avoid(XMFLOAT3 pos, float delta)
{
	XMFLOAT3 dir;
	dir.x = pos.x - mAICar->_model->_translateVector.x;
	dir.y = pos.y - mAICar->_model->_translateVector.y;
	dir.z = pos.z - mAICar->_model->_translateVector.z;

	float len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
	dir.x /= len;
	dir.y /= len;
	dir.z /= len;


	mAICar->_model->_translateVector.x -= dir.x * mAvoidSpeed * delta;
	mAICar->_model->_translateVector.z -= dir.z * mAvoidSpeed * delta;
}

