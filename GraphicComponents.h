#ifndef GRAPHICCOMPONENTS_H
#define GRAPHICCOMPONENTS_H

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <iostream>
#include <DirectXMath.h>
#include "MacroDefinitions.h"
#include "BufferComponents.h"

using namespace std;

class GraphicComponents {

public:

	GraphicComponents();
	~GraphicComponents();
	void ReleaseAll();

	IDXGISwapChain* gSwapChain;	// Swap Chain responsible for switching between a collection of buffers and show images to the user
	ID3D11Device* gDevice;	// Graphic Device
	ID3D11DeviceContext* gDeviceContext;	// Graphic Contect
	ID3D11RenderTargetView* gBackbufferRTV;	// Render Target View

	ID3D11InputLayout* gVertexLayout;	
	ID3D11VertexShader* gVertexShader;	
	ID3D11PixelShader* gPixelShader;	
	ID3D11GeometryShader* gGeometryShader; 
	
	ID3D11ComputeShader* gComputeShaderHB;
	ID3D11ComputeShader* gComputeShaderHV;

	ID3D11InputLayout* gShadowVsLayout;
	ID3D11VertexShader* gShadowVS;
	ID3D11PixelShader* gShadowPS;

	ID3D11InputLayout* gVertexTerrainLayout;	
	ID3D11VertexShader* gVertexTerrainShader;	
	ID3D11PixelShader* gPixelTerrainShader;	
	ID3D11GeometryShader* gGeometryTerrainShader;

	ID3D11InputLayout* gVertexBoneLayout;
	ID3D11VertexShader* gVertexBoneShader;
	ID3D11PixelShader* gPixelBoneShader;
	ID3D11GeometryShader* gGeometryBoneShader;

	ID3D11VertexShader* gQuadVertexShader;
	ID3D11PixelShader* gQuadPixelShader;

	ID3D11InputLayout* gCylinderLayout;
	ID3D11VertexShader* gCylinderVertexShader;
	ID3D11PixelShader* gCylinderFragmentShader;

	ID3D11InputLayout* gVertexQTreeLayout;
	ID3D11VertexShader* gVertexQTreeShader;
	ID3D11PixelShader* gPixelQTreeShader;

	bool InitalizeDirect3DContext(HWND &windowHandle, BufferComponents &bHandler);	// Function called to initalize the necessary components, as listen below
	bool CreateRenderTargetView(BufferComponents &bHandler);	// We require a render target view for rendering and we create this by taking the back buffer from the swap chain
	bool CreateSwapChainAndDevice(HWND &windowHandle);	// Function to create the graphic device responsible for interactions with the graphic card and the swap chain to switch between back & front buffer

	bool CreateStandardShaders();
	bool CreateTerrainShaders();	// Function to create all the potential shaders to be used in the application
	bool CreateBoneShaders();
	bool CreateShadowMapShader();

	bool CreateComputeShaders();
	bool CreateQuadShader();

	bool CreateCylinderShaders();
	bool CreateQTreeShaders();

	bool CreateDepthStencil(BufferComponents &bHandler);
	void SetViewport();		// Functions to define the properties of our viewport

};

#endif GRAPHICCOMPONENTS_H
