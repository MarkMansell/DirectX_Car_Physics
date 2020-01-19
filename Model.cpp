#include "Model.h"

Model::Model(MeshData* m){
	mesh = m;

	XMStoreFloat4x4(&world, XMMatrixIdentity());
	XMStoreFloat4x4(&scale, XMMatrixIdentity());
	XMStoreFloat4x4(&rotate, XMMatrixIdentity());
	XMStoreFloat4x4(&translate, XMMatrixIdentity());
	_translateVector.x = 0.0f;
	_translateVector.y = 0.0f;
	_translateVector.z = 0.0f;
	_rotateAngles.x = 0.0f;
	_rotateAngles.y = 0.0f;
	_rotateAngles.z = 0.0f;
}

Model::~Model(){

}

//Traverse the scene graph to update all groups world matrices.
void Model::Update(){

	XMMATRIX s = XMLoadFloat4x4(&scale);
	XMMATRIX r = XMMatrixRotationRollPitchYaw(_rotateAngles.x, _rotateAngles.y, _rotateAngles.z);
	XMMATRIX t = XMMatrixTranslationFromVector(XMLoadFloat3(&_translateVector));

	//XMMATRIX s = XMLoadFloat4x4(&scale);
	//XMMATRIX r = XMLoadFloat4x4(&rotate);
	//XMMATRIX t = XMLoadFloat4x4(&translate);

	XMStoreFloat4x4(&world, s * r * t);

}

//Traverse the scene graph to draw the entire model.
void Model::Draw(ID3D11DeviceContext* context){
	for(int i = 0; i < _groups.size(); i++){
		_groups[i]->Draw(context, mesh);
	}
}