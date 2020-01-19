#pragma once

#include <windows.h>
#include <d3d11.h>
//#include <d3dx11.h>
#include <d3d11_1.h>
#include <directxmath.h>
#include <d3dcompiler.h>
//#include <xnamath.h>
#include "resource.h"

#include "Model.h"
#include "Camera.h"

#include <vector>
#include <math.h>
using namespace DirectX;

struct ConstantBuffer{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;

	XMFLOAT4 diffuseMaterial;
	XMFLOAT4 diffuseLight;
	XMFLOAT4 lightVecW;//needs to be 4 to stop offset affecting colour
	XMFLOAT4 spec;

	XMFLOAT4 ambientMaterial;
	XMFLOAT4 ambientLight;

	XMFLOAT4 specularMtrl;
	XMFLOAT4 specularLight;
	XMFLOAT4 eyePosW;

	XMFLOAT4 transparency;
};


class Application{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
public: 
	ID3D11Device*           _pd3dDevice;
	UINT _WindowHeight;
	UINT _WindowWidth;
	ID3D11DeviceContext*    _pImmediateContext;
private:	
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pConstantBuffer;

	ID3D11SamplerState* _pSamplerAnisotropic; 

	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;

	ID3D11RasterizerState* _wireFrame;
	ID3D11RasterizerState* _bothSides;

	ID3D11BlendState* Transparency;
	ID3D11BlendState* blendState;

	float _time;

	//Using XMFLOAT4x4 instead of XMMATRIX, as xmmatrix is a hardware register can cannot be used in a header file
	
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();

	void LoadResources();

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Draw(SGnode* baseNode, Model* skyBox, Camera* cam);//upgrade to multiple models
	void DrawModel(SGnode* currentNode, XMMATRIX* parentWorld, ConstantBuffer* cb);
};

