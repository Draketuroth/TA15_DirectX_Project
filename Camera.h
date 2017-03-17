#ifndef CAMERA_H
#define CAMERA_H

#include <windows.h>

#include <d3d11.h>	// We require the d3d11 header for Direct3D functions
#include <d3dcompiler.h>	// We also need the D3DCompiler header to compile shaders
#include <DirectXMath.h>
#include <iostream>
#include <DirectXCollision.h>

// Necessary lib files kan be linked here, but also be added by going to:
// Properties->Linker->Input->Additional Dependencies
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace std;
struct Plane
{
	XMFLOAT3 Normal;
	float Distance;

};
class Camera {

public:

	// Constructor and destructor
	Camera();
	~Camera();

	POINT mLastMousePos;

	//PLane for frustum
	Plane Frustum[6];
	BoundingFrustum testFrust;
	// Get/Set Camera Properties
	XMVECTOR GetPositionXM()const;
	XMFLOAT3 GetPosition()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);

	// Get Camera Vectors
	XMVECTOR GetRightXM()const;
	XMFLOAT3 GetRight()const;
	XMVECTOR GetUpXM()const;
	XMFLOAT3 GetUp()const;
	XMVECTOR GetLookXM()const;
	XMFLOAT3 GetLook()const;

	// Get Frustum Properties
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get Near and Far Plane dimensions in View Space coordinates
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	// Set frustum
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define Camera Space via Look At parameters
	void LookAt(XMVECTOR pos, XMVECTOR target, XMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	// Get View / Proj matrices
	XMMATRIX View()const;
	XMMATRIX Proj()const;
	XMMATRIX ViewProj()const;

	// Strafe / Walk the camera a distance d
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera
	void Pitch(float angle);
	void RotateY(float angle);

	// Update View Matrix after every frame 
	void UpdateViewMatrix();

	void OnMouseMove(WPARAM btnState, int x, int y); // Update mouse movement after every frame


	//hightMap
	bool Collotion();
//	XMFLOAT3 GetHeightPosition()const;

	float GetX()const; 
	float GetZ()const; 

	void testCreate();
	void CreateFrustum();
private:

	// Camera coordinate system with coordinates relative to World Space
	XMFLOAT3 mPosition;	// View Space Origin
	XMFLOAT3 mRight;	// View Space X-Axis
	XMFLOAT3 mUp;	// View Space Y-Axis
	XMFLOAT3 mLook;	// ViewSpace Z-Axis

	// Cache Frustum Properties
	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	// Cache View/Proj Matrices 
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;

};

#endif CAMERA_H