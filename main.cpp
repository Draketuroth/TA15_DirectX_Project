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

	if (!tHandler.SetupTextures(gHandler.gDevice, bHandler)) {
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Textures couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}
	
	if (!QTree.CreateTree(0))
	{
		MessageBox(
			NULL,
			L"CRITICAL ERROR: Quadtree couldn't be initialized\nClosing application...",
			L"ERROR",
			MB_OK);
	}

	return RunApplication();
}

int RunApplication() {

	//----------------------------------------------------------------------------------------------------------------------------------//
	// PREDEFINED VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	MSG windowMessage = { 0 };

	SetCapture(windowHandle);

	mCam.mLastMousePos.x = 0;
	mCam.mLastMousePos.y = 0;

	static bool topDownViewFlag = 1;
	HRESULT hr;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE VertexBufferResource;
	D3D11_MAPPED_SUBRESOURCE topDownBufferResource;

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

	for (size_t i = 0; i < CUBECAPACITY; i++)//loop through all objects that needs to be assigned to a node in the quadtree
	{
		QTree.checkBoundingBox(bHandler.cubeObjects[i]);
	}

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

			if (GetAsyncKeyState('J') & 0x8000) {

				topDownViewFlag = !topDownViewFlag;

				Sleep(200);
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

			//----------------------------------------------------------------------------------------------------------------------------------//
			// TOP DOWN PERSPECTIVE SWITCH
			//----------------------------------------------------------------------------------------------------------------------------------//

			if (topDownViewFlag == 0) {

				//to follow the hightmap
				if (mCam.Collotion() == true)
				{
					XMFLOAT3 camPos = mCam.GetPosition();
					float y = terrain.GetHeight(camPos.x, camPos.z);
					mCam.SetPosition(camPos.x, y + 5.0f, camPos.z);
				}

				hr = gHandler.gDeviceContext->Map(bHandler.topDownCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &topDownBufferResource);

				TOPDOWN_CAMERA* cameraPointer = (TOPDOWN_CAMERA*)topDownBufferResource.pData;
				
				cameraPointer->topDownViewTransform = bHandler.topDownCamData.topDownViewTransform;
				
				XMMATRIX P = tCameraProjection;
				XMMATRIX MV = XMMatrixMultiply(bHandler.tWorldMatrix, bHandler.topDownCamData.topDownViewTransform);
				XMMATRIX temp = P * MV;
				XMMATRIX inv = XMMatrixInverse(nullptr, temp);

				XMMATRIX WVP = XMMatrixInverse(nullptr, inv);
				cameraPointer->projectionInverse = WVP;

				gHandler.gDeviceContext->Unmap(bHandler.topDownCameraBuffer, 0);
			}

			else if (topDownViewFlag == 1) {

				//to follow the hightmap
				if (mCam.Collotion() == true)
				{
					XMFLOAT3 camPos = mCam.GetPosition(); 
					float y = terrain.GetHeight(camPos.x, camPos.z); 
					mCam.SetPosition(camPos.x, y + 5.0f, camPos.z); 
				}

				hr = gHandler.gDeviceContext->Map(bHandler.topDownCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &topDownBufferResource);

				TOPDOWN_CAMERA* cameraPointer = (TOPDOWN_CAMERA*)topDownBufferResource.pData;

				cameraPointer->topDownViewTransform = tCameraView;
				
				XMMATRIX P = tCameraProjection;
				XMMATRIX MV = XMMatrixMultiply(bHandler.tWorldMatrix, tCameraView);
				XMMATRIX temp = P * MV;
				XMMATRIX inv = XMMatrixInverse(nullptr, temp);

				XMMATRIX WVP = XMMatrixInverse(nullptr, inv);
				cameraPointer->projectionInverse = WVP;

				gHandler.gDeviceContext->Unmap(bHandler.topDownCameraBuffer, 0);
			}
			
			//----------------------------------------------------------------------------------------------------------------------------------//
			// CONSTANT BUFFER UPDATE
			//----------------------------------------------------------------------------------------------------------------------------------//

			hr = gHandler.gDeviceContext->Map(bHandler.gConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			GS_CONSTANT_BUFFER* cBufferPointer = (GS_CONSTANT_BUFFER*)mappedResource.pData;

			cBufferPointer->worldViewProj = (bHandler.tWorldMatrix * tCameraViewProj);
			cBufferPointer->viewProj = tCameraViewProj;
			cBufferPointer->worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(NULL, bHandler.tWorldMatrix));
			cBufferPointer->matrixWorld = bHandler.tWorldMatrix;
			cBufferPointer->matrixViewInverse = XMMatrixInverse(NULL,tCameraView);
			cBufferPointer->matrixView = bHandler.tWorldMatrix * tCameraView;
			cBufferPointer->matrixProjection = tCameraProjection;
			cBufferPointer->lightViewProj = bHandler.tLightViewProj;
			
			XMStoreFloat4(&cBufferPointer->cameraPos, mCam.GetPositionXM());
			cBufferPointer->floorRot = bHandler.tFloorRot;
			XMStoreFloat4(&cBufferPointer->cameraUp,mCam.GetUpXM());
			
			/*i++;
			if (i < 20000000000)
			{
				cout << mCam.GetPosition().x << " " << mCam.GetPosition().y << " " << mCam.GetPosition().z << endl;
				i = 0;
			}*/

			gHandler.gDeviceContext->Unmap(bHandler.gConstantBuffer, 0);
			//mCam.CreateFrustum();
			//mCam.updateFrustum();
			mCam.BoundingFrustumCreate();
			
			 //----------------------------------------------------------------------------------------------------------------------------------//
			 // QUAD TREE FUNCTIONS
			 //----------------------------------------------------------------------------------------------------------------------------------//

			 QTree.recursiveIntersect(mCam);//Check for frustum intersection


			 QTree.checkRenderObjects();
			// QTree.printIntersections();

			//----------------------------------------------------------------------------------------------------------------------------------//
			// UPDATE FRUSTUM BUFFER
			//----------------------------------------------------------------------------------------------------------------------------------//

			 if(ENABLE_FRUSTUM_DEBUG){

			 bHandler.CreateArrowBuffer(gHandler.gDevice, mCam);

			 }

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

