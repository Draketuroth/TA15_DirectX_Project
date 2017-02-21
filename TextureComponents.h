#ifndef TEXTURECOMPONENTS_H
#define TEXTURECOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include "MacroDefinitions.h"
#include "WICTextureLoader.h"
#include "BufferComponents.h"

using namespace std;
using namespace DirectX;

class TextureComponents {

public:

	TextureComponents();
	~TextureComponents();

	ID3D11ShaderResourceView* standardResource;	// Shader resource views are required for accessing data in a resource
	ID3D11ShaderResourceView* boneResource;	// Shader resource views are required for accessing data in a resource
	ID3D11ShaderResourceView* terrainResource;
	ID3D11ShaderResourceView* grassResource;


	ID3D11SamplerState* shadowSampler;
	ID3D11SamplerState* texSampler;
	ID3D11Texture2D* pShadowMap;
	ID3D11DepthStencilView* pSmDepthView;
	ID3D11ShaderResourceView* pSmSRView;

	ID3D11Texture2D* sampleTexture;
	ID3D11Texture2D* computeTexture;

	ID3D11ShaderResourceView* sampleSRV;	// Sample texture from first pass to be used as a Shader Resource View
	ID3D11ShaderResourceView* computePixelInput;
	ID3D11UnorderedAccessView* sampleUAV;	// Output texture from the compute shader as an UnorderedAccessView
	ID3D11RenderTargetView* sampleRTV;

	bool CreateTexture(ID3D11Device* &gDevice, BufferComponents &bHandler);
	bool CreateShadowMap(ID3D11Device* &gDevice);
	bool CreateComputeRenderTarget(ID3D11Device* &gDevice);

};

#endif TEXTURECOMPONENTS_H
