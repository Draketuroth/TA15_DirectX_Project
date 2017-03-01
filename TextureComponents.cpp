
#include "TextureComponents.h"
#include "BufferComponents.h"

TextureComponents::TextureComponents() {

	standardResource = nullptr;
	boneResource = nullptr;
	terrainResource = nullptr;
	grassResource = nullptr;

	geometryTexture = nullptr;
	geometryTextureRTV = nullptr;
	geometryTextureSRV = nullptr;

	textureBlur_HB = nullptr;
	blurredResultUAV_HB = nullptr;
	blurredResultSRV_HB = nullptr;

	textureBlur_HV = nullptr;
	blurredResultUAV_HV = nullptr;
	blurredResultSRV_HV = nullptr;

	shadowSampler = nullptr;
	texSampler = nullptr;
	pShadowMap = nullptr;
	pSmDepthView = nullptr;
	pSmSRView = nullptr;
}

TextureComponents::~TextureComponents() {
	

}

void TextureComponents::ReleaseAll() {

	SAFE_RELEASE(standardResource);
	SAFE_RELEASE(boneResource);
	SAFE_RELEASE(terrainResource);
	SAFE_RELEASE(grassResource);

	SAFE_RELEASE(geometryTexture);
	SAFE_RELEASE(geometryTextureRTV);
	SAFE_RELEASE(geometryTextureSRV);

	SAFE_RELEASE(textureBlur_HB);
	SAFE_RELEASE(blurredResultUAV_HB);
	SAFE_RELEASE(blurredResultSRV_HB);

	SAFE_RELEASE(textureBlur_HV);
	SAFE_RELEASE(blurredResultUAV_HV);
	SAFE_RELEASE(blurredResultSRV_HV);

	SAFE_RELEASE(shadowSampler);
	SAFE_RELEASE(texSampler);
	SAFE_RELEASE(pShadowMap);
	SAFE_RELEASE(pSmDepthView);
	SAFE_RELEASE(pSmSRView);
}

bool TextureComponents::CreateTexture(ID3D11Device* &gDevice,BufferComponents &bHandler) {

	HRESULT hr;

	// In order to create the texture, we require a sampler state to determine how the texture data should be sampled using filtering, addressing modes 
	// and level of detail

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SAMPLER STATE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = gDevice->CreateSamplerState(&sampDesc, &texSampler);

	if (FAILED(hr)) {

		return false;
	}

	ID3D11Texture2D* texture = nullptr;

	

	CoInitialize(NULL);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\BrickTexture.png", NULL, &standardResource, 256);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\chess.jpg", NULL, &boneResource, 512);
	CreateWICTextureFromFile(gDevice,NULL, bHandler.OBJTexturePath.c_str(), NULL,&terrainResource,256);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\terrain.png", NULL, &grassResource);
	
	if (SUCCEEDED(hr) && texture != 0) {

		gDevice->CreateShaderResourceView(texture, nullptr, &standardResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &boneResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &terrainResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &grassResource);

		if (FAILED(hr)) {

			texture->Release();
		}
	}

	return true;
}

bool TextureComponents::CreateShadowMap(ID3D11Device* &gDevice)
{
	HRESULT hr;


	D3D11_SAMPLER_DESC shadowSamp;
	ZeroMemory(&shadowSamp, sizeof(shadowSamp));
	shadowSamp.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	shadowSamp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	shadowSamp.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	shadowSamp.MinLOD = 0;
	shadowSamp.MaxLOD = D3D11_FLOAT32_MAX;
	hr = gDevice->CreateSamplerState(&shadowSamp, &shadowSampler);

	if (FAILED(hr)) {

		return false;
	}


	//Shadow map texture description
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = WIDTH;
	texDesc.Height = HEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	//Depth stencil view description
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	//Shader resource view description
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

	hr = gDevice->CreateTexture2D(&texDesc, NULL, &this->pShadowMap);
	if (FAILED(hr))
	{
		return false;
	}
	hr = gDevice->CreateDepthStencilView(pShadowMap, &descDSV, &pSmDepthView);
	if (FAILED(hr))
	{
		return false;
	}
	hr = gDevice->CreateShaderResourceView(pShadowMap, &srvDesc, &pSmSRView);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool TextureComponents::InitializeComputeShaderResources(ID3D11Device* &gDevice) {

	if (!CreateRenderTargetViewTexture(gDevice)) {

		return false;
	}

	if (!CreateUAVTextureHB(gDevice)) {

		return false;
	}

	if (!CreateUAVTextureHV(gDevice)) {

		return false;
	}

	return true;
	
}

bool TextureComponents::CreateRenderTargetViewTexture(ID3D11Device* &gDevice) {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// First, we must create a new Render Target View and make sure a Shader Resource View holds the texture as well
	//----------------------------------------------------------------------------------------------------------------------------------//

	HRESULT hr;

	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Width = WIDTH;
	textureDesc.Height = HEIGHT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// Create the empty texture resource
	//----------------------------------------------------------------------------------------------------------------------------------//

	hr = gDevice->CreateTexture2D(&textureDesc, NULL, &geometryTexture);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// Bind the geometry texture resource to Render Target View
	//----------------------------------------------------------------------------------------------------------------------------------//

	hr = gDevice->CreateRenderTargetView(geometryTexture, NULL, &geometryTextureRTV);

	if (FAILED(hr)) {

		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;
	shaderResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// Bind the geometry texture resource to a Shader Resource View
	//----------------------------------------------------------------------------------------------------------------------------------//

	hr = gDevice->CreateShaderResourceView(geometryTexture, &shaderResourceDesc, &geometryTextureSRV);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool TextureComponents::CreateUAVTextureHB(ID3D11Device* &gDevice) {

	HRESULT hr;

	D3D11_TEXTURE2D_DESC uavTexDesc;

	uavTexDesc.Width = WIDTH;
	uavTexDesc.Height = HEIGHT;
	uavTexDesc.MipLevels = 1;
	uavTexDesc.ArraySize = 1;
	uavTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavTexDesc.SampleDesc.Count = 1;
	uavTexDesc.SampleDesc.Quality = 0;
	uavTexDesc.Usage = D3D11_USAGE_DEFAULT;
	uavTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	uavTexDesc.CPUAccessFlags = 0;
	uavTexDesc.MiscFlags = 0;

	hr = gDevice->CreateTexture2D(&uavTexDesc, NULL, &textureBlur_HB);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// Make sure we have a second texture for passing result from Compute Shader. It wil be bound as:
	// Unordenend Access View (Results written from Compute Shader)
	// Shader Resource View (Results written from Compute Shader to be used as a Shader Resource for a Pixel Shader)
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = gDevice->CreateUnorderedAccessView(textureBlur_HB, &uavDesc, &blurredResultUAV_HB);

	if (FAILED(hr)) {

		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = gDevice->CreateShaderResourceView(textureBlur_HB, &srvDesc, &blurredResultSRV_HB);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool TextureComponents::CreateUAVTextureHV(ID3D11Device* &gDevice) {

	HRESULT hr;

	D3D11_TEXTURE2D_DESC uavTexDesc;

	uavTexDesc.Width = WIDTH;
	uavTexDesc.Height = HEIGHT;
	uavTexDesc.MipLevels = 1;
	uavTexDesc.ArraySize = 1;
	uavTexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavTexDesc.SampleDesc.Count = 1;
	uavTexDesc.SampleDesc.Quality = 0;
	uavTexDesc.Usage = D3D11_USAGE_DEFAULT;
	uavTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	uavTexDesc.CPUAccessFlags = 0;
	uavTexDesc.MiscFlags = 0;

	hr = gDevice->CreateTexture2D(&uavTexDesc, NULL, &textureBlur_HV);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// Make sure we have a second texture for passing result from Compute Shader. It wil be bound as:
	// Unordenend Access View (Results written from Compute Shader)
	// Shader Resource View (Results written from Compute Shader to be used as a Shader Resource for a Pixel Shader)
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));

	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = gDevice->CreateUnorderedAccessView(textureBlur_HV, &uavDesc, &blurredResultUAV_HV);

	if (FAILED(hr)) {

		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = gDevice->CreateShaderResourceView(textureBlur_HV, &srvDesc, &blurredResultSRV_HV);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

