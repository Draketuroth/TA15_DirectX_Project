
#include "TextureComponents.h"

TextureComponents::TextureComponents() {

	ID3D11ShaderResourceView* standardResource = nullptr;
	ID3D11ShaderResourceView* boneResource = nullptr;
	ID3D11SamplerState* texSampler = nullptr;
}

TextureComponents::~TextureComponents() {

	standardResource->Release();
	boneResource->Release();
	texSampler->Release();
}

bool TextureComponents::CreateTexture(ID3D11Device* &gDevice) {

	HRESULT hr;

	// In order to create the texture, we require a sampler state to determine how the texture data should be sampled using filtering, addressing modes 
	// and level of detail

	//----------------------------------------------------------------------------------------------------------------------------------//
	// SAMPLER STATE DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = gDevice->CreateSamplerState(&sampDesc, &texSampler);

	if (FAILED(hr)) {

		return false;
	}

	ID3D11Texture2D* texture = nullptr;

	CoInitialize(NULL);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\BrickTexture.png", NULL, &standardResource, 256);
	CreateWICTextureFromFile(gDevice, NULL, L"Textures\\chess.jpg", NULL, &boneResource, 1024);
	CreateWICTextureFromFile(gDevice,NULL, L"Textures\\small.jpg", NULL,&terrainResource,256);

	if (SUCCEEDED(hr) && texture != 0) {

		gDevice->CreateShaderResourceView(texture, nullptr, &standardResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &boneResource);
		gDevice->CreateShaderResourceView(texture, nullptr, &terrainResource);

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
