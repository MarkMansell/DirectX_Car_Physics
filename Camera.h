#pragma once
#include <windows.h>
//#include <xnamath.h>

//#include <d3d11.h>
//#include <d3dx11.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <string>
using namespace DirectX;

class Camera{
private:
	std::string _name;
	XMFLOAT4 _eye;
	XMFLOAT4 _at;
	XMFLOAT4 _up;

	XMFLOAT3 _eulerAngles;//Although roll isnt used. Not bothering with quaternions, they're overkill for a simple FPS camera like this

	FLOAT _windowWidth;
	FLOAT _windowHeight;
	FLOAT _nearDepth;
	FLOAT _farDepth;

	XMFLOAT4X4 _view;
	XMFLOAT4X4 _projection;

public:
	Camera(std::string name, XMFLOAT4 eye, XMFLOAT3 angles, FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
	~Camera();



	void Update();
	void Forward(float speed);
	void Vertical(float speed);
	void Strafe(float speed);
	void CalculateViewProjection();

	XMFLOAT4X4 GetView() const { return _view; }
	XMFLOAT4X4 GetProjection() const { return _projection; }

	XMFLOAT4X4 GetViewProjection() const;

	std::string GetName() const { return _name; }
	XMFLOAT3 GetAngles() const { return _eulerAngles; }
	XMFLOAT4 GetEye() const { return _eye; }
	XMFLOAT4 GetAt() const { return _at; }
	XMFLOAT4 GetUp() const { return _up; }

	void SetAngles(XMFLOAT3 angles) { _eulerAngles = angles; }
	void SetAnglesX(float x) { _eulerAngles.x = x; }
	void SetAnglesY(float y) { _eulerAngles.y = y; }
	void SetAnglesZ(float z) { _eulerAngles.z = z; }
	void AppendAnglesX(float x) { _eulerAngles.x += x; }
	void AppendAnglesY(float y) { _eulerAngles.y += y; }
	void AppendAnglesZ(float z) { _eulerAngles.z += z; }
	void SetEye(XMFLOAT4 eye) { _eye = eye; }
	void SetAt(XMFLOAT4 at) { _at = at; }
	void SetUp(XMFLOAT4 up) { _up = up; }

	void Reshape(FLOAT windowWidth, FLOAT windowHeight, FLOAT nearDepth, FLOAT farDepth);
};