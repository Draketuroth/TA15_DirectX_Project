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
#include "Quadtree.h"

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
Quadtree QTree;
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

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);	// Memory leak detection flag
	//_CrtSetBreakAlloc(207);
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
			L"CRITICAL ERROR: DirectX Context couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}


	if (!bHandler.SetupScene(gHandler.gDevice, mCam, fbxImporter)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Buffers couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	//hightMap
	terrain.LoadRAW(); 
	terrain.BuildQuadPatchVB(gHandler.gDevice);
	terrain.BuildQuadPatchIB(gHandler.gDevice);

	if (!tHandler.CreateTexture(gHandler.gDevice,bHandler)) {

		MessageBox(
			NULL,
			L"CRITICAL ERROR: Textures couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}
	if (!tHandler.CreateShadowMap(gHandler.gDevice)) {
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Shadow map couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}


	if (!tHandler.InitializeComputeShaderResources(gHandler.gDevice)) {
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Compute Shader Resources couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}
	
	/*if (!QTree.CreateTree(0, gHandler.gDevice))
	{
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Quadtree couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}*/

	return RunApplication();
}

int RunApplication() {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// FRUSTUM CULLING INITIALIZATION
	//----------------------------------------------------------------------------------------------------------------------------------//

	/*for (int i = 0; i < 8; i++) {

		QTree.checkBoundingBox(bHandler.cubeObjects[i].bbox);
	}*/

	//----------------------------------------------------------------------------------------------------------------------------------//
	// PREDEFINED VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	MSG windowMessage = { 0 };

	SetCapture(windowHandle);

	mCam.mLastMousePos.x = 0;
	mCam.mLastMousePos.y = 0;

	static bool normalMapping = 1;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE VertexBufferResource;

	// Starting angle for the rotation matrix is stored in the angle variable
	int interval = 0;
	float angle = 0.05f;
	fbxImporter.animTimePos = 0.0f;

	// Storing the counts per second
	__int64 countsPerSecond = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	float secondsPerCount = 1.0f / countsPerSecond;

	// Initialize the previous time
	__int64 previousTime = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&previousTime);
	float time = 0;
	XMFLOAT4 PMRand[1000] = {XMFLOAT4(0,0,0,1)};
	

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

			// Capture the current count

			__int64 currentTime = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

			// Calculate the delta time

			float deltaTime = ((currentTime - previousTime) * secondsPerCount);

			float speed = 10.0f;

			if (GetAsyncKeyState('W') & 0x8000) {

				mCam.Walk(speed * deltaTime);
			}

			if (GetAsyncKeyState('S') & 0x8000) {

				mCam.Walk(-speed * deltaTime);
			}

			if (GetAsyncKeyState('A') & 0x8000) {

				mCam.Strafe(-speed * deltaTime);
			}

			if (GetAsyncKeyState('D') & 0x8000) {

				mCam.Strafe(speed * deltaTime);
			}

			showFPS(windowHandle, deltaTime, bHandler);

			fbxImporter.animTimePos += deltaTime * 20;

			if (fbxImporter.animTimePos >= fbxImporter.animationLength){
					
				fbxImporter.animTimePos = 0.0f;
			}

			fbxImporter.UpdateAnimation(gHandler.gDeviceContext);

			POINT p;
			GetCursorPos(&p);

			mCam.OnMouseMove(WM_LBUTTONDOWN, p.x, p.y);

			//----------------------------------------------------------------------------------------------------------------------------------//
			// CAMERA UPDATE
			//----------------------------------------------------------------------------------------------------------------------------------//

			mCam.UpdateViewMatrix();	// Update Camera View and Projection Matrix for each frame

			XMMATRIX tCameraViewProj = XMMatrixTranspose(mCam.ViewProj());	// Camera View Projection Matrix
			XMMATRIX tCameraProjection = XMMatrixTranspose(mCam.Proj());
			XMMATRIX tCameraView = XMMatrixTranspose(mCam.View());		// Camera View Matrix
			
			
			HRESULT hr;
			
			//----------------------------------------------------------------------------------------------------------------------------------//
			// CONSTANT BUFFER UPDATE
			//----------------------------------------------------------------------------------------------------------------------------------//

			// Here we disable GPU access to the vertex buffer data so I can change it on the CPU side and update it by sending it back when finished

			hr = gHandler.gDeviceContext->Map(bHandler.gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			
			
			// We create a pointer to the constant buffer containing the world matrix that requires to be multiplied with the rotation matrix

			GS_CONSTANT_BUFFER* cBufferPointer = (GS_CONSTANT_BUFFER*)mappedResource.pData;

			// Here We access the world matrix and update it. The angle of the rotation matrix is updated for every frame with a rotation matrix 
			// constructed to rotate the triangles around the y-axis

			// Both matrices must recieve the same treatment from the rotation matrix, no matter if we want to preserve its original space or not

			cBufferPointer->worldViewProj = (bHandler.tWorldMatrix * tCameraViewProj);
			cBufferPointer->viewProj = tCameraViewProj;
			cBufferPointer->worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(NULL, bHandler.tWorldMatrix));
			cBufferPointer->matrixWorld = bHandler.tWorldMatrix;
			cBufferPointer->matrixViewInverse = XMMatrixInverse(NULL,tCameraView);
			cBufferPointer->matrixView = bHandler.tWorldMatrix * tCameraView;
			cBufferPointer->matrixProjection = tCameraProjection;
			cBufferPointer->lightViewProj = bHandler.tLightViewProj;

			if (GetAsyncKeyState('N') & 0x8000) {

				normalMapping = !normalMapping;
				Sleep(200);
			}

			cBufferPointer->normalMappingFlag = normalMapping;

			//to folow the hightmap
			if (mCam.Collotion() == true)
			{
				XMFLOAT3 camPos = mCam.GetPosition(); 
				float y = terrain.GetHeight(camPos.x, camPos.z); 
				mCam.SetPosition(camPos.x, y + 5.0f, camPos.z); 
			}
			
			XMStoreFloat4(&cBufferPointer->cameraPos, mCam.GetPositionXM());
			cBufferPointer->floorRot = bHandler.tFloorRot;
			XMStoreFloat4(&cBufferPointer->cameraUp,mCam.GetUpXM());
			
			/*i++;
			if (i < 20000000000)
			{
				cout << mCam.GetPosition().x << " " << mCam.GetPosition().y << " " << mCam.GetPosition().z << endl;
				i = 0;
			}*/

			// At last we have to reenable GPU access to the vertex buffer data

			 gHandler.gDeviceContext->Unmap(bHandler.gConstantBuffer, 0);

			//----------------------------------------------------------------------------------------------------------------------------------//
			// PARTICLE MOVEMENT
			//----------------------------------------------------------------------------------------------------------------------------------//

			time += deltaTime * 2000;

			if (time > 150)
			{
				hr = gHandler.gDeviceContext->Map(bHandler.gVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &VertexBufferResource);
			
				VS_CONSTANT_BUFFER* vBufferPointer = (VS_CONSTANT_BUFFER*)VertexBufferResource.pData;
				time = 0;
				for (size_t i = 0; i < 1000; i++)
				{
					PMRand[i].x = ((float)rand()) / (float)RAND_MAX / 12;
					PMRand[i].y = ((float)rand()) / (float)RAND_MAX / 12;
					PMRand[i].z = ((float)rand()) / (float)RAND_MAX / 12;
					vBufferPointer->particleMovement[i] = PMRand[i];
			
			
				}
			
				gHandler.gDeviceContext->Unmap(bHandler.gVertexConstantBuffer, 0);
			}


			
			/*if (QTree.frustumIntersect(mCam) == INTERSECT  || QTree.frustumIntersect(mCam) == INSIDE)
			{
				for (UINT i = 0; i < 4; i++)
				{

				}
			}*/

			//----------------------------------------------------------------------------------------------------------------------------------//
			// RENDER
			//----------------------------------------------------------------------------------------------------------------------------------//

			// Now we can render using the new updated buffers on the GPU

			Render(gHandler, bHandler, tHandler, fbxImporter, terrain);

			// When everythig has been drawn out, finish by presenting the final result on the screen by swapping between the back and front buffers

			gHandler.gSwapChain->Present(0, 0); // Change front and back buffer

			angle = angle - 0.001;	// Angle is updated for every frame

			// Set previous time to current time after the frame ends

			previousTime = currentTime;

		}

	}

	terrain.ReleaseAll();
	fbxImporter.ReleaseAll();
	tHandler.ReleaseAll();
	bHandler.ReleaseAll();
	gHandler.ReleaseAll();
	DestroyWindow(windowHandle);

	return static_cast<int>(windowMessage.wParam);
}

