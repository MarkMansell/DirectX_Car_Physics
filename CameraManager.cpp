#include "CameraManager.h"

CameraManager::CameraManager(){

}

CameraManager::~CameraManager(){

}

void CameraManager::AddCamera(std::string camName, CameraData camData){
	_cameras[camName] = new Camera(camName, camData._eye, camData._angles, camData._width, camData._height, camData._nearDepth, camData._farDepth);
}

Camera* CameraManager::RetrieveCamera(std::string camName){
	if(_cameras.count(camName) > 0){
		return _cameras[camName];
	}
	return nullptr;
}

void CameraManager::DeleteCamera(std::string camName){
	if(_cameras.count(camName) > 0){
		_cameras.erase(camName);
	}
}

void CameraManager::SetEye(std::string camName, float x, float y, float z){
	if(_cameras.find(camName) == _cameras.end()){
		return;
	}
	_cameras[camName]->SetEye(XMFLOAT4(x, y, z, 0.0f));
}

void CameraManager::SetAt(std::string camName, float x, float y, float z){
	if(_cameras.find(camName) == _cameras.end()){
		return;
	}
	_cameras[camName]->SetAt(XMFLOAT4(x, y, z, 0.0f));
}
