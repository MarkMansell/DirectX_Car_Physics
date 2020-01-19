#pragma once
#pragma once

#include <vector>
#include "Model.h"

using namespace std;
using namespace DirectX;

class CarAIController
{
public:
	CarAIController();
	~CarAIController();

	void Update(float delta);

	void SetAICar(SGnode* aicar){ mAICar = aicar; }
	void SetPLayerCarPos(XMFLOAT3* pos){ mPlayerCar = pos; }
	void MoveTowards(XMFLOAT3* pos, float delta);
	void AddWaypoint(XMFLOAT3 pos);
	void Avoid(XMFLOAT3 pos, float delta);

private:
	vector<XMFLOAT3> mTrackPoints;
	XMFLOAT3* mPlayerCar;
	SGnode* mAICar;
	float mSpeed;
	XMFLOAT3* mCurrentWaypoint;
	int mWayPointNum;
	float mAvoidSpeed;
};

