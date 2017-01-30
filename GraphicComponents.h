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

	IDXGISwapChain* gSwapChain;	// Swap Chain responsible for switching between a collection of buffers and show images to the user
	ID3D11Device* gDevice;	// Graphic Device
	ID3D11DeviceContext* gDeviceContext;	// Graphic Contect
	ID3D11RenderTargetView* gBackbufferRTV;	// Render Target View

	ID3D11InputLayout* gVertexTerrainLayout;	// Vertex Layout
	ID3D11VertexShader* gVertexTerrainShader;	// Vertex Shader
	ID3D11PixelShader* gPixelTerrainShader;	// Pixel Shader
	ID3D11GeometryShader* gGeometryTerrainShader; // Geometry Shader

	ID3D11InputLayout* gVertexBoneLayout;
	ID3D11VertexShader* gVertexBoneShader;
	ID3D11PixelShader* gPixelBoneShader;
	ID3D11GeometryShader* gGeometryBoneShader;

	bool InitalizeDirect3DContext(HWND &windowHandle, BufferComponents &bHandler);	// Function called to initalize the necessary components, as listen below
	bool CreateRenderTargetView(BufferComponents &bHandler);	// We require a render target view for rendering and we create this by taking the back buffer from the swap chain
	bool CreateSwapChainAndDevice(HWND &windowHandle);	// Function to create the graphic device responsible for interactions with the graphic card and the swap chain to switch between back & front buffer

	bool CreateTerrainShaders();	// Function to create all the potential shaders to be used in the application
	bool CreateBoneShaders();

	bool CreateDepthStencil(BufferComponents &bHandler);
	void SetViewport();		// Functions to define the properties of our viewport

};

#endif GRAPHICCOMPONENTS_H
