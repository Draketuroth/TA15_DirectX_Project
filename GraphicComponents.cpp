
#include "GraphicComponents.h"

GraphicComponents::GraphicComponents() {

	gSwapChain = nullptr;
	gDevice = nullptr;	
	gDeviceContext = nullptr;	
	gBackbufferRTV = nullptr;	

	gVertexTerrainLayout = nullptr;
	gVertexTerrainShader = nullptr;
	gPixelTerrainShader = nullptr;
	gGeometryTerrainShader = nullptr;

	gVertexLayout = nullptr;
	gVertexShader = nullptr;
	gPixelShader = nullptr;
	gGeometryShader = nullptr;

	gVertexBoneLayout = nullptr;
	gVertexBoneShader = nullptr;
	gPixelBoneShader = nullptr;
	gGeometryBoneShader = nullptr;
}

GraphicComponents::~GraphicComponents() {

	gBackbufferRTV->Release();
	gSwapChain->Release();
	gDevice->Release();
	gDeviceContext->Release();

	gVertexTerrainLayout->Release();
	gVertexTerrainShader->Release();
	gPixelTerrainShader->Release();
	gGeometryTerrainShader->Release();

	gVertexLayout->Release();
	gVertexShader->Release();
	gPixelShader->Release();
	gGeometryShader->Release();

	gVertexBoneLayout->Release();
	gVertexBoneShader->Release();
	gPixelBoneShader->Release();
	gGeometryBoneShader->Release();

}

bool GraphicComponents::InitalizeDirect3DContext(HWND &windowHandle, BufferComponents &bHandler) {

	if (!CreateSwapChainAndDevice(windowHandle)) {

		return false;
	}
	if (!CreateDepthStencil(bHandler))
	{
		return false;
	}

	if (!CreateRenderTargetView(bHandler)) {

		return false;
	}

	SetViewport();

	if (!CreateStandardShaders()) {

		return false;
	}

	if (!CreateTerrainShaders()) {

		return false;
	}
	if (!CreateShadowMapShader())
	{
		return false;
	}

	if (!CreateBoneShaders()) {

		return false;
	}

	return true;
}

bool GraphicComponents::CreateSwapChainAndDevice(HWND &windowHandle) {

	// CreateSwapChainAndDevice function is responsible for:
	// - giving us the back buffer from the created swap chain
	// - create a render target view of it to use as the target for rendering
	// - We need to make sure that we create the swap chain before we ever call on our CreateDepthBuffer function

	// A description of our swap chain will be defined with different resources and settings
	// Create the swap chain struct to hold all required information
	DXGI_SWAP_CHAIN_DESC chainDescription;

	// Just as a safety routine, we clear out the struct of any data to prepare it 
	ZeroMemory(&chainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

	// The most important settings are filled in below
	chainDescription.BufferCount = 1;									// There is only one back buffer
	chainDescription.BufferDesc.Height = HEIGHT;
	chainDescription.BufferDesc.Width = WIDTH;
	chainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// We are using 32-bit color
	chainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		// Defines use of swap chain. It now uses the surface or resource as an output render target.
	chainDescription.OutputWindow = windowHandle;						// The window to be used defined in "Global Window Variables"
	chainDescription.SampleDesc.Count = 4;								// How many multisamples to be used and default seems to be 4
	chainDescription.Windowed = LaunchInWindowedMode;					// Specify whether to launch application window in windowed or fullscreen mode		

																		// A trick here is that we can take advantage of a debug layer, and therefore Debug should be defined as a build option

	UINT createDeviceFlags = 0;
#if defined (DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL supportedFeatureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(

		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&chainDescription,
		&gSwapChain,
		&gDevice,
		&supportedFeatureLevel,
		&gDeviceContext);

	if (FAILED(hr)) {

		cout << "Device Creation Error: Device, DeviceContext and Swap Chain could not be created" << endl;
		return false;
	}

	if (supportedFeatureLevel < D3D_FEATURE_LEVEL_11_0) {

		cout << "DirectX Error: DirectX 11 is not supported" << endl;
		return false;
	}

	return true;
}

bool GraphicComponents::CreateShadowMapShader()
{

	HRESULT hr;
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\ShadowShaders\\ShadowVS.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		0,
		0,
		&vsBlob,
		&vsErrorBlob
	);
	if (FAILED(hr))
	{
		cout << "Shadow Map Vertex Shader Error:  Vertex Shader could not be compiled or loaded from file" << endl;
		if (vsErrorBlob)
		{
			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}
		return false;
	}
	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gShadowVS);
	if (FAILED(hr))
	{
		cout << "Shadow Map Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}
	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {
		{"POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}	
	};
	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	hr = gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gShadowVsLayout);
	
	if (FAILED(hr))
	{
		cout << "Shadow Vertex Shader Error: Vertex layout could not be created" << endl;
	}
	vsBlob->Release();


	//Shadow PS
	

	return true;
}

bool GraphicComponents::CreateRenderTargetView(BufferComponents &bHandler){

	// CreateRenderTargetView function is responsible for:
	// - Giving us the back buffer from the created swap chain
	// - Create a render target view of it to use as the target for rendering

	ID3D11Texture2D* backBuffer;

	HRESULT hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);

	if (FAILED(hr)) {

		// This error will arise when the back buffer could not be retrieved
		cout << "Buffer Error: Back buffer could not be retrieved" << endl;
		return false;
	}

	hr = gDevice->CreateRenderTargetView(backBuffer, nullptr, &gBackbufferRTV);

	if (FAILED(hr)) {

		// This error is given because the render target view creation failed
		cout << "Render Target View Error: Render target view could not be created" << endl;
		return false;
	}

	backBuffer->Release();
	
	gDeviceContext->OMSetDepthStencilState(bHandler.depthState, 1);
	gDeviceContext->OMSetRenderTargets(1, &gBackbufferRTV, bHandler.depthView);

	return true;
}

void GraphicComponents::SetViewport() {

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0.0f;	// This is the top left corner's x coordinate in pixels from the window's top left corner
	viewport.TopLeftY = 0.0f;	// This is the top left corner's y coordinate in pixels from the window's top left corner
	viewport.MinDepth = 0.0f;	// This is the minimum depth value used by DirectX
	viewport.MaxDepth = 1.0f;	// This is the maximum depth value used by Direct3D
	viewport.Width = WIDTH;		// This viewport will use a width of user defined pixels. We are using the already defined macro values
	viewport.Height = HEIGHT;	// This viewport will use a height of user defined pixels. We are using the already defined macro values

	gDeviceContext->RSSetViewports(1, &viewport);	// Sets the viewport to be used
}

bool GraphicComponents::CreateStandardShaders() {

	HRESULT hr;

	ID3DBlob* vsBlob = nullptr; // lvl 1 blue slime
	ID3DBlob* vsErrorBlob = nullptr; 

	hr = D3DCompileFromFile(
		L"Shaders\\StandardShaders\\Vertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		0,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gVertexShader);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gVertexLayout);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();


	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\StandardShaders\\Fragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		0,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gPixelShader);

	if (FAILED(hr)) {

		cout << "Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	ID3DBlob* gsBlob = nullptr;
	ID3DBlob* gsErrorBlob = nullptr;
	hr = D3DCompileFromFile(
		L"Shaders\\StandardShaders\\Geometry.hlsl",
		nullptr,
		nullptr,
		"GS_main",
		"gs_5_0",
		0,
		0,
		&gsBlob,
		&gsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Geometry Shader Error: Geometry Shader could not be compiled or loaded from file" << endl;

		if (gsErrorBlob) {

			OutputDebugStringA((char*)gsBlob->GetBufferPointer());
			gsErrorBlob->Release();
		}

	}

	hr = gDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &gGeometryShader);

	if (FAILED(hr)) {

		cout << "Geometry Shader Error: Geometry Shader could not be created" << endl;
		return false;
	}

	gsBlob->Release();

	return true;
}

bool GraphicComponents::CreateTerrainShaders() {

	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	

	hr = D3DCompileFromFile(
		L"Shaders\\TerrainShaders\\TerrainVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		0,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gVertexTerrainShader);

	if (FAILED(hr)) {

		cout << "Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		
	};


	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gVertexTerrainLayout);

	if (FAILED(hr)) {

		cout << "Terrain Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();


	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\TerrainShaders\\TerrainFragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		0,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Terrain Pixel Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gPixelTerrainShader);

	if (FAILED(hr)) {

		cout << "Terrain Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	ID3DBlob* gsBlob = nullptr;
	ID3DBlob* gsErrorBlob = nullptr;
	hr = D3DCompileFromFile(
		L"Shaders\\TerrainShaders\\TerrainGeometry.hlsl",
		nullptr,
		nullptr,
		"GS_main",
		"gs_5_0",
		0,
		0,
		&gsBlob,
		&gsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Terrain Geometry Shader Error: Geometry Shader could not be compiled or loaded from file" << endl;

		if (gsErrorBlob) {

			OutputDebugStringA((char*)gsBlob->GetBufferPointer());
			gsErrorBlob->Release();
		}

	}

	hr = gDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &gGeometryTerrainShader);

	if (FAILED(hr)) {

		cout << "Terrain Geometry Shader Error: Geometry Shader could not be created" << endl;
		return false;
	}

	gsBlob->Release();

	return true;
}

bool GraphicComponents::CreateBoneShaders() {

	HRESULT hr;

	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* vsErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\BoneShaders\\BoneVertex.hlsl",
		nullptr,
		nullptr,
		"VS_main",
		"vs_5_0",
		0,
		0,
		&vsBlob,
		&vsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Bone Vertex Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (vsErrorBlob) {

			OutputDebugStringA((char*)vsErrorBlob->GetBufferPointer());
			vsErrorBlob->Release();
		}

		return false;
	}


	hr = gDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &gVertexBoneShader);

	if (FAILED(hr)) {

		cout << " Bone Vertex Shader Error: Vertex Shader could not be created" << endl;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexInputDesc[] = {

		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",		0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",			0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT",			0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES",	0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	int inputLayoutSize = sizeof(vertexInputDesc) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	gDevice->CreateInputLayout(vertexInputDesc, inputLayoutSize, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &gVertexBoneLayout);

	if (FAILED(hr)) {

		cout << "Bone Vertex Shader Error: Shader Input Layout could not be created" << endl;
	}

	vsBlob->Release();


	ID3DBlob* psBlob = nullptr;
	ID3DBlob* psErrorBlob = nullptr;

	hr = D3DCompileFromFile(
		L"Shaders\\BoneShaders\\BoneFragment.hlsl",
		nullptr,
		nullptr,
		"PS_main",
		"ps_5_0",
		0,
		0,
		&psBlob,
		&psErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Bone Pixel Shader Error: Vertex Shader could not be compiled or loaded from file" << endl;

		if (psErrorBlob) {

			OutputDebugStringA((char*)psErrorBlob->GetBufferPointer());
			psErrorBlob->Release();
		}

		return false;
	}

	hr = gDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &gPixelBoneShader);

	if (FAILED(hr)) {

		cout << "Bone Pixel Shader Error: Pixel Shader could not be created" << endl;
		return false;
	}

	psBlob->Release();

	ID3DBlob* gsBlob = nullptr;
	ID3DBlob* gsErrorBlob = nullptr;
	hr = D3DCompileFromFile(
		L"Shaders\\BoneShaders\\BoneGeometry.hlsl",
		nullptr,
		nullptr,
		"GS_main",
		"gs_5_0",
		0,
		0,
		&gsBlob,
		&gsErrorBlob
	);

	if (FAILED(hr)) {

		cout << "Bone Geometry Shader Error: Geometry Shader could not be compiled or loaded from file" << endl;

		if (gsErrorBlob) {

			OutputDebugStringA((char*)gsBlob->GetBufferPointer());
			gsErrorBlob->Release();
		}

	}

	hr = gDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &gGeometryBoneShader);

	if (FAILED(hr)) {

		cout << "Bone Geometry Shader Error: Geometry Shader could not be created" << endl;
		return false;
	}

	gsBlob->Release();

	return true;
}

bool GraphicComponents::CreateDepthStencil(BufferComponents &bHandler)
{
		HRESULT hr;

		// Depth Texture
		//----------------------------------------------------------------------------------------------------------------------------------//

		// THe depth buffer texture is used to store the distance of each fragment to the camera

		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = WIDTH;
		descDepth.Height = HEIGHT;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 4;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		hr = gDevice->CreateTexture2D(&descDepth, nullptr, &bHandler.depthStencil);

		if (FAILED(hr)) {

			return false;
		}

		//----------------------------------------------------------------------------------------------------------------------------------//


		// Depth View Description
		//----------------------------------------------------------------------------------------------------------------------------------//

		// Create depth view description. A depth-stencil-view interface process a texture resource during the depth-stencil testing.
		D3D11_DEPTH_STENCIL_VIEW_DESC dViewDesc;
		dViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dViewDesc.Texture2D.MipSlice = 0;
		dViewDesc.Flags = 0;

		//Create depth stencil view
		hr = gDevice->CreateDepthStencilView(bHandler.depthStencil, &dViewDesc, &bHandler.depthView);

		if (FAILED(hr)) {

			return false;
		}

		// Depth State Description
		//----------------------------------------------------------------------------------------------------------------------------------//

		D3D11_DEPTH_STENCIL_DESC stencilDesc;

		// Depth test
		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		// Stencil test
		stencilDesc.StencilEnable = true;
		stencilDesc.StencilReadMask = 0xFF;
		stencilDesc.StencilWriteMask = 0xFF;

		// Stencil operations if the pixel is facing forward

		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if the pixel is facing backward

		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create Depth State
		hr = gDevice->CreateDepthStencilState(&stencilDesc, &bHandler.depthState);

		if (FAILED(hr)) {

			return false;
		}
		
		//----------------------------------------------------------------------------------------------------------------------------------//

		return true;
	
}
