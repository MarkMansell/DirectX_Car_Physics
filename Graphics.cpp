#include "Graphics.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message){
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application(){
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pConstantBuffer = nullptr;

	_pSamplerAnisotropic = nullptr;

	_depthStencilView = nullptr;
	_depthStencilBuffer = nullptr;

	_bothSides = nullptr;
	_wireFrame = nullptr;

	blendState = nullptr;

	_time = 0.0f;

}

Application::~Application(){
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow){
	//HRESULT is an errorcode, useful on return functions
    if (FAILED(InitWindow(hInstance, nCmdShow))){
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice())){
        Cleanup();
        return E_FAIL;
    }

	LoadResources();

	//Initdevice has finished so the buffers are set up already;
	//modelArray[0] = new Model(_pVertexBuffer, 8, _pIndexBuffer, 36);


	return S_OK;
}

void Application::LoadResources(){
	// Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	_pd3dDevice->CreateSamplerState( &sampDesc, &_pSamplerAnisotropic);

}

HRESULT Application::InitShadersAndInputLayout(){
	HRESULT hr;
	///////////////////////
	/////VERTEX SHADER/////
	///////////////////////
    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"Lighting.fx", "VS", "vs_4_0", &pVSBlob);
    if (FAILED(hr)){
		//Prefixing a string with 'L' switches between unicode and multibyte. LPCSTRINGS can be entered normally, WCHAR* must be prefixed with L.
        MessageBox(nullptr,L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);
	if (FAILED(hr)){	
		pVSBlob->Release();
        return hr;
	}

	//////////////////////
	/////PIXEL SHADER/////
	//////////////////////
	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"Lighting.fx", "PS", "ps_4_0", &pPSBlob);
    if (FAILED(hr)){
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();
    if (FAILED(hr))
        return hr;

	///////////////
	/////INPUT/////
	///////////////
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] ={
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",0, DXGI_FORMAT_R32G32_FLOAT,0,24, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"FGGS Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
   // hr = D3DX11CompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, nullptr, ppBlobOut, &pErrorBlob, nullptr);
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr)){
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice(){
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] ={
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] ={
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	//BACK BUFFER
    DXGI_SWAP_CHAIN_DESC sd; //Swap chain is a method of manually double-buffering
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++){
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

	//DEPTH / STENCIL BUFFER TEXTURE (after hardware setup, but before OMTarget call)
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = _WindowWidth;
	depthStencilDesc.Height    = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;//24 bit depth, 8 bit stencil. Z-fighting can be caused by having too small amount of bits in a depth buffer, or stretching those bits too far across an enourmous view frustrum
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);//create the texture
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);//create a view based on the textures values


    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);//Add the depth stencil to OMtargets

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	//INIT BOTH SHADERS AND INPUT
	InitShadersAndInputLayout();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof (D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; //0x0f;
	// set blending
	_pd3dDevice->CreateBlendState(&blendDesc, &blendState);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	//Create rasterizer state
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;//D3D11_FILL_WIREFRAME or D3D11_FILL_SOLID 
	wfdesc.CullMode = D3D11_CULL_NONE;//D3D11_CULL_NONE, D3D11_CULL_FRONT, or D3D11_CULL_BACK
	wfdesc.DepthClipEnable = false;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);
	//Create rasterizer state
	D3D11_RASTERIZER_DESC bothSides;
	ZeroMemory(&bothSides, sizeof(D3D11_RASTERIZER_DESC));
	bothSides.FillMode = D3D11_FILL_SOLID ;
	bothSides.CullMode = D3D11_CULL_NONE;
	bothSides.DepthClipEnable = false;
	hr = _pd3dDevice->CreateRasterizerState(&bothSides, &_bothSides);


    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup(){
	//Free all COM objects used
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();

	if (_pSamplerAnisotropic) _pSamplerAnisotropic->Release();

	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	if (_wireFrame) _wireFrame->Release();
	if (_bothSides) _bothSides->Release();

	if (blendState) blendState->Release();

}

void Application::Draw(SGnode* baseNode, Model* skyBox, Camera* cam){

    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
	//float ClearColor[4] = { 0.0f, 0.0f, 0.3f, 1.0f }; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);//clear render view
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);//clear depth view to depth value 1.0F

	cam->CalculateViewProjection();
	//Convert from XMfloats to usable XMmatrices
	XMMATRIX view = XMLoadFloat4x4(&cam->GetView());
	XMMATRIX projection = XMLoadFloat4x4(&cam->GetProjection());

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);

    _pImmediateContext->PSSetSamplers( 0, 1, &_pSamplerAnisotropic );

	if (GetKeyState(0x5A)){//Press Z for wireframe
		_pImmediateContext->RSSetState(_wireFrame);
	}
	else{
		_pImmediateContext->RSSetState(_bothSides);
	}

	float blendFactor[4] = {1,1,1, 1 };
	_pImmediateContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
	
	ConstantBuffer cb;
	cb.world = XMMatrixIdentity();
	cb.view = XMMatrixTranspose(view);
	cb.projection = XMMatrixTranspose(projection);

	cb.eyePosW = cam->GetEye();

	cb.lightVecW = XMFLOAT4(-9000.0f, 6000.0f, 0.0f, 0.0f);
	cb.specularLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.ambientLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	for(int i = 0; i < skyBox->_priorityGroups.size(); i++){
		Group* currentGroup = skyBox->_groups[skyBox->_priorityGroups[i]];

		cb.diffuseMaterial = currentGroup->mat->diffuse;
		cb.ambientMaterial = currentGroup->mat->ambient;
		cb.specularMtrl = currentGroup->mat->specular;
		cb.spec = currentGroup->mat->specularExponent;
		cb.transparency = XMFLOAT4(currentGroup->mat->transparency, 1.0f, 1.0f, 1.0f);

		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		currentGroup->Draw(_pImmediateContext, skyBox->mesh);
	}

	cb.ambientLight = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	
	XMMATRIX base = XMMatrixIdentity();
	DrawModel(baseNode, &base, &cb);

    // Present our back buffer to our front buffer
    _pSwapChain->Present(1, 0);
}

//recursive function to draw the whole scene graph
void Application::DrawModel(SGnode* currentNode, XMMATRIX* parentWorld, ConstantBuffer* cb){
	XMMATRIX finalWorld =  XMLoadFloat4x4(&currentNode->_model->world) * *parentWorld;

	cb->world = XMMatrixTranspose(finalWorld);

	for(int i = 0; i < currentNode->_model->_priorityGroups.size(); i++){
		Group* currentGroup = currentNode->_model->_groups[currentNode->_model->_priorityGroups[i]];

		cb->diffuseMaterial = currentGroup->mat->diffuse;
		cb->ambientMaterial = currentGroup->mat->ambient;
		cb->specularMtrl = currentGroup->mat->specular;
		cb->spec = currentGroup->mat->specularExponent;
		cb->transparency = XMFLOAT4(currentGroup->mat->transparency, 1.0f, 1.0f, 1.0f);

		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, cb, 0, 0);
		currentGroup->Draw(_pImmediateContext, currentNode->_model->mesh);
	}

	for(int i = 0; i < currentNode->_children.size(); i++){
		DrawModel(&currentNode->_children[i], &finalWorld, cb);
	}
	
};