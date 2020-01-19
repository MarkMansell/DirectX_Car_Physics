#include "Camera.h"

Camera::Camera(std::string name, XMFLOAT4 eye, XMFLOAT3 angles, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth)
	: _name(name), _eye(eye), _eulerAngles(angles), _windowWidth(windowWidth), _windowHeight(windowHeight), _nearDepth(nearDepth), _farDepth(farDepth){
		Update();//to initialise _at and _up correctly
}

Camera::~Camera(){

}

void Camera::Update(){
	XMVECTOR DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
	XMVECTOR DefaultUp = XMVectorSet(0.0f,1.0f,0.0f, 0.0f);

	XMMATRIX rotateMatrix = XMMatrixRotationRollPitchYaw(_eulerAngles.x, _eulerAngles.y, 0);
	XMVECTOR newForward = XMVector3TransformCoord(DefaultForward, rotateMatrix);
	newForward = XMVector3Normalize(newForward);
	XMStoreFloat4(&_at, newForward);
	XMVECTOR newUp = XMVector3TransformCoord(DefaultUp, rotateMatrix);
	newUp = XMVector3Normalize(newUp);
	XMStoreFloat4(&_up, newUp);
}

void Camera::Forward(float speed){
	XMFLOAT2 forward(_at.x, _at.z);
	float mag = sqrt((forward.x * forward.x)+(forward.y *forward.y));
	forward.x = forward.x / mag;
	forward.y = forward.y / mag;
	_eye.x += forward.x * speed;
	_eye.z += forward.y * speed;
}

void Camera::Vertical(float speed){
	_eye.y += 1 * speed;
}

void Camera::Strafe(float speed){
	XMFLOAT2 orth(-_at.z, _at.x);
	float mag = sqrt((orth.x * orth.x)+(orth.y *orth.y));
	orth.x = orth.x / mag;
	orth.y = orth.y / mag;

	_eye.x += orth.x * speed;
	_eye.z += orth.y * speed;
}

void Camera::CalculateViewProjection(){
    // Initialize the view matrix
	XMVECTOR Eye = XMLoadFloat4(&_eye); //XMMATRIX and XMVECTOR are not allowed in header files since they're hardware registers. Use XMFLOAT4/3 and XMFLOAT4X4 instead.
	XMVECTOR At = XMLoadFloat4(&_at); 
	XMVECTOR Up = XMLoadFloat4(&_up); 

	XMStoreFloat4x4(&_view, XMMatrixLookToLH(Eye, At, Up));//Changed 'At' from a focal point to a camera direction vector for easier look control

    // Initialize the projection matrix
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV4, _windowWidth / _windowHeight, _nearDepth, _farDepth));
}

void Camera::Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth){
	_windowWidth = windowWidth;
	_windowHeight = windowHeight;
	_nearDepth = nearDepth;
	_farDepth = farDepth;
}

XMFLOAT4X4 Camera::GetViewProjection() const { 
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

	XMFLOAT4X4 viewProj;

	XMStoreFloat4x4(&viewProj, view * projection);

	return viewProj;
}