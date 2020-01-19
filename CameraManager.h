#pragma once
#include "Camera.h"
#include <map>
#include <string>
using namespace DirectX;

struct CameraData{
	XMFLOAT4 _eye;
	XMFLOAT3 _angles;
	FLOAT _width;
	FLOAT _height;
	FLOAT _nearDepth;
	FLOAT _farDepth;
	CameraData(XMFLOAT4 eye, XMFLOAT3 angles, FLOAT width,FLOAT height,FLOAT nearDepth,FLOAT farDepth)
		:_eye(eye),_angles(angles),_width(width),_height(height),_nearDepth(nearDepth),_farDepth(farDepth){};
};

class CameraManager{
public:
	CameraManager();
	~CameraManager();
	void AddCamera(std::string camName, CameraData camData);
	Camera* CameraManager::RetrieveCamera(std::string);
	void DeleteCamera(std::string camName);

	void SetEye(std::string camName, float x, float y, float z);
	void SetAt(std::string camName, float x, float y, float z);

private: 
	std::map<std::string, Camera*> _cameras;
};