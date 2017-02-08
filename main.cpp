//----------------------------------------------------------------------------------------------------------------------------------//
// main.cpp
// DV1541 3D PROGMRAMMING: PROJECT
//
// THIS APPLICATION IS CREATED BY THE FOLLOWING GROUP MEMBERS FROM THE TA15 CLASS:
// Philip Velandria, Jonathan Sundberg, Linnea Vajda, Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//

#include <windows.h>
#include <d3d11.h>	
#include <d3dcompiler.h>	
#include <iostream>
#include <Windows.h>
#include "Camera.h"
#include "WICTextureLoader.h"
#include "Window.h"

#include "GraphicComponents.h"
#include "BufferComponents.h"
#include "TextureComponents.h"

#include "Terrain.h"

#include "Render.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;

//----------------------------------------------------------------------------------------------------------------------------------//
// WINDOW HANDLE AND CAMERA
//----------------------------------------------------------------------------------------------------------------------------------//
HWND windowHandle;
Camera mCam; 

//----------------------------------------------------------------------------------------------------------------------------------//
// PIPELINE COMPONENTS
//----------------------------------------------------------------------------------------------------------------------------------//
GraphicComponents gHandler;
BufferComponents bHandler;
TextureComponents tHandler;

//----------------------------------------------------------------------------------------------------------------------------------//
// FORWARD DECLARATIONS
//----------------------------------------------------------------------------------------------------------------------------------//
int RunApplication();

//----------------------------------------------------------------------------------------------------------------------------------//
// TECHNIQUE CLASSES
//----------------------------------------------------------------------------------------------------------------------------------//
FbxImport fbxImporter;
Terrain terrain;

int main() {

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag

																	// We always want to keep our eyes open for terminal errors, which mainly occur when the window isn't created

	if (!WindowInitialize(windowHandle)) {

		// If the window cannot be created during startup, it's more known as a terminal error
		// The MessageBox function will display a message and inform us of the problem
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Window couldn't be initialized, investigate window initializr function\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	// If also DirectX cannot be initialized on startup, nothing can be drawn to the screen. We use MessageBox again to display the error. 
	if (!gHandler.InitalizeDirect3DContext(windowHandle, bHandler)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: DirectX couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}


	bHandler.SetupScene(gHandler.gDevice, mCam, fbxImporter);

	terrain.LoadRAW(); 
	terrain.BuildHeightmapSRV(gHandler.gDevice);
	terrain.BuildQuadPatchIB(gHandler.gDevice);
	terrain.BuildQuadPatchVB(gHandler.gDevice);

	if (!tHandler.CreateTexture(gHandler.gDevice)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Textures couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	return RunApplication();
}

int RunApplication() {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// PREDEFINED VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag

	MSG windowMessage = { 0 };

	SetCapture(windowHandle);

	mCam.mLastMousePos.x = 0;
	mCam.mLastMousePos.y = 0;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE boneMappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	// Starting angle for the rotation matrix is stored in the angle variable

	float angle = 0.05f;
	fbxImporter.frameIndex = 0;

	const int MAX_STEPS = 6;
	const float MAX_DELTA_TIME = 1.0f;
	const float DESIRED_FPS = 60.0f;
	const float MS_PER_SECOND = 1000;
	const float DESIRED_FRAMETIME = MS_PER_SECOND / DESIRED_FPS;
	float previousTicks = GetTickCount();

	while (windowMessage.message != WM_QUIT) {

		if (PeekMessage(&windowMessage, NULL, NULL, NULL, PM_REMOVE)) {

			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);
		}

		// If there are no messages to handle, the application will continue by running a frame

		else {

			//----------------------------------------------------------------------------------------------------------------------------------//
			// DELTA TIMING
			//----------------------------------------------------------------------------------------------------------------------------------//

			VS_SKINNED_DATA* boneBufferPointer = (VS_SKINNED_DATA*)boneMappedResource.pData;
			gHandler.gDeviceContext->Map(fbxImporter.gBoneBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &boneMappedResource);

			float newTicks = GetTickCount();
			float frameTime = newTicks - previousTicks;
			previousTicks = newTicks;
			float totalDeltaTime = frameTime / DESIRED_FRAMETIME;

			int i = 0;

			while (totalDeltaTime > 0.0f && i < MAX_STEPS) {

				float deltaTime = min(totalDeltaTime, MAX_DELTA_TIME);

				if (GetAsyncKeyState('W') & 0x8000) {

					mCam.Walk(-0.05f * deltaTime);
				}

				if (GetAsyncKeyState('S') & 0x8000) {

					mCam.Walk(0.05f * deltaTime);
				}

				if (GetAsyncKeyState('A') & 0x8000) {

					mCam.Strafe(0.05f * deltaTime);
				}

				if (GetAsyncKeyState('D') & 0x8000) {

					mCam.Strafe(-0.05f * deltaTime);
				}

				fbxImporter.UpdateAnimation(boneBufferPointer, deltaTime);

				POINT p;
				GetCursorPos(&p);

				mCam.OnMouseMove(WM_LBUTTONDOWN, p.x, p.y);

				totalDeltaTime -= deltaTime;
				i++;

			}

			gHandler.gDeviceContext->Unmap(fbxImporter.gBoneBuffer, 0);

			//----------------------------------------------------------------------------------------------------------------------------------//
			// CAMERA UPDATE
			//----------------------------------------------------------------------------------------------------------------------------------//

			mCam.UpdateViewMatrix();	// Update Camera View and Projection Matrix for each frame

			XMMATRIX tCameraViewProj = XMMatrixTranspose(mCam.ViewProj());	// Camera View Projection Matrix
			XMMATRIX tCameraProjection = XMMatrixTranspose(mCam.Proj());
			XMMATRIX tCameraView = XMMatrixTranspose(mCam.View());		// Camera View Matrix

			//----------------------------------------------------------------------------------------------------------------------------------//
			// CONSTANT BUFFER UPDATE
			//----------------------------------------------------------------------------------------------------------------------------------//

			// Here we disable GPU access to the vertex buffer data so I can change it on the CPU side and update it by sending it back when finished

			gHandler.gDeviceContext->Map(bHandler.gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			// We create a pointer to the constant buffer containing the world matrix that requires to be multiplied with the rotation matrix

			GS_CONSTANT_BUFFER* cBufferPointer = (GS_CONSTANT_BUFFER*)mappedResource.pData;

			// Here We access the world matrix and update it. The angle of the rotation matrix is updated for every frame with a rotation matrix 
			// constructed to rotate the triangles around the y-axis

			// Both matrices must recieve the same treatment from the rotation matrix, no matter if we want to preserve its original space or not

			cBufferPointer->worldViewProj = (bHandler.tWorldMatrix  * tCameraViewProj);
			cBufferPointer->matrixWorld = bHandler.tWorldMatrix;
			cBufferPointer->matrixView = bHandler.tWorldMatrix * tCameraView;
			cBufferPointer->matrixProjection = tCameraProjection;

			cBufferPointer->cameraPos = mCam.GetPosition();
			cBufferPointer->floorRot = bHandler.tFloorRot;

			// At last we have to reenable GPU access to the vertex buffer data

			gHandler.gDeviceContext->Unmap(bHandler.gConstantBuffer, 0);

			//----------------------------------------------------------------------------------------------------------------------------------//
			// RENDER
			//----------------------------------------------------------------------------------------------------------------------------------//

			// Now we can render using the new updated buffers on the GPU

			Render(gHandler, bHandler, tHandler, fbxImporter, terrain);

			// When everythig has been drawn out, finish by presenting the final result on the screen by swapping between the back and front buffers

			gHandler.gSwapChain->Present(0, 0); // Change front and back buffer

			angle = angle - 0.001;	// Angle is updated for every frame
		}

	}

	bHandler.~BufferComponents();
	gHandler.~GraphicComponents();
	tHandler.~TextureComponents();
	fbxImporter.~FbxImport();

	DestroyWindow(windowHandle);

	return static_cast<int>(windowMessage.wParam);
}

