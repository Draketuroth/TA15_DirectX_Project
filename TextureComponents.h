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
	ID3D11ShaderResourceView* fireflyResource;

	ID3D11Texture2D* geometryTexture;
	ID3D11RenderTargetView* geometryTextureRTV;
	ID3D11ShaderResourceView* geometryTextureSRV; // Geometry texture from first pass is available here (SRV)

	ID3D11Texture2D* textureBlur_HB;	// Instead of reading and writing to the same texture, we make a middle hand texture to pass the result from the Compute Shader
	ID3D11UnorderedAccessView* blurredResultUAV_HB; // Texture result to be written from Compute Shader
	ID3D11ShaderResourceView* blurredResultSRV_HB; // Texture result to be written from Compute Shader in a Shader Resource View

	ID3D11Texture2D* textureBlur_HV;	// Instead of reading and writing to the same texture, we make a middle hand texture to pass the result from the Compute Shader
	ID3D11UnorderedAccessView* blurredResultUAV_HV; // Texture result to be written from Compute Shader
	ID3D11ShaderResourceView* blurredResultSRV_HV; // Texture result to be written from Compute Shader in a Shader Resource View

	ID3D11SamplerState* shadowSampler;
	ID3D11SamplerState* texSampler;
	ID3D11BlendState* blendState;
	ID3D11Texture2D* pShadowMap;
	ID3D11DepthStencilView* pSmDepthView;
	ID3D11ShaderResourceView* pSmSRView;

	bool CreateTexture(ID3D11Device* &gDevice, BufferComponents &bHandler);
	bool CreateShadowMap(ID3D11Device* &gDevice);
	bool InitializeComputeShaderResources(ID3D11Device* &gDevice);
	bool CreateRenderTargetViewTexture(ID3D11Device* &gDevice); // For first pass
	bool CreateUAVTextureHB(ID3D11Device* &gDevice); // For second pass
	bool CreateUAVTextureHV(ID3D11Device* &gDevice); // For second pass

};

#endif TEXTURECOMPONENTS_H
