
#include "Camera.h"
#include "Terrain.h"

// Constructor and destructor
Camera::Camera() {
	
}

Camera::~Camera() {

	
}

bool Camera::Collotion()
{
	bool collition = false; 
	float CAMx; 
	float CAMz;

	CAMx = GetX(); 
	CAMz = GetZ(); 

	if (CAMx >= -31.5 && CAMx <= 31.5)
	{
		if (CAMz >= -31.5&& CAMz <= 31.5)
		{
			collition = true; 
		}
	}
	
	return collition; 
}

// Get/Set Camera Properties
XMVECTOR Camera::GetPositionXM()const {

	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition()const {

	return mPosition;
}

float Camera::GetX()const
{
	float HX = mPosition.x; 
	return HX; 
}

float Camera::GetZ()const
{
	float HZ = mPosition.z;
	return HZ;
}

void Camera::SetPosition(float x, float y, float z) {

	mPosition = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3& v) {

	mPosition = v;
}

// Get Camera Vectors
XMVECTOR Camera::GetRightXM()const {

	return XMLoadFloat3(&mRight);
}

XMFLOAT3 Camera::GetRight()const {

	return mRight;
}

XMVECTOR Camera::GetUpXM()const {

	return XMLoadFloat3(&mUp);
}

XMFLOAT3 Camera::GetUp()const {

	return mUp;
}

XMVECTOR Camera::GetLookXM()const {

	return XMLoadFloat3(&mLook);
}

XMFLOAT3 Camera::GetLook()const {

	return mLook;
}

// Get Frustum Properties
float Camera::GetNearZ()const {

	return mNearZ;
}

float Camera::GetFarZ()const {

	return mFarZ;
}

float Camera::GetAspect()const {

	return mAspect;
}

float Camera::GetFovY()const {

	return mFovY;
}

float Camera::GetFovX()const {

	float halfWidth = 0.5f * GetNearWindowWidth();

	return 2.0f*atan(halfWidth / mNearZ);
}

// Get Near and Far Plane dimensions in View Space coordinates
float Camera::GetNearWindowWidth()const {

	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const {

	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth()const {

	return mAspect * mFarWindowHeight;
}

float Camera::GetFarWindowHeight()const {

	return mFarWindowHeight;
}

// Set frustum
void Camera::SetLens(float fovY, float aspect, float zn, float zf) {

	// Cache properties
	mFovY = fovY;
	mAspect = aspect;

	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f*mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f*mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProj, P);
}

// Define Camera Space via Look At parameters
void Camera::LookAt(XMVECTOR pos, XMVECTOR target, XMVECTOR worldUp) {

	XMStoreFloat3(&mPosition, pos);
	XMStoreFloat3(&mLook, target);
	XMStoreFloat3(&mUp, worldUp);
	XMStoreFloat3(&mRight, XMVector3Cross(pos - target, worldUp));
	
	XMMATRIX viewMatrix = XMMatrixLookAtLH(pos, target, worldUp);
	XMStoreFloat4x4(&mView, viewMatrix);

}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up) {

	mPosition = pos;
	mLook = target;
	mUp = up;
	
	DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat3(&pos);
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR upDirection = DirectX::XMLoadFloat3(&up);

	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePos, lookAt, upDirection);
	XMStoreFloat4x4(&mView, viewMatrix);
}

// Get View / Proj matrices
XMMATRIX Camera::View()const {

	return XMLoadFloat4x4(&mView);
}

XMMATRIX Camera::Proj()const {

	return XMLoadFloat4x4(&mProj);
}

XMMATRIX Camera::ViewProj()const {

	XMMATRIX aView = XMLoadFloat4x4(&mView);
	XMMATRIX aProj = XMLoadFloat4x4(&mProj);

	return aView * aProj;
}

// Strafe / Walk the camera a distance d
void Camera::Strafe(float d) {

	// mPosition += deltaTime * mRight
	// If creating a vector from a single scalar variable, we use VectorReplicate
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
}

void Camera::Walk(float d) {

	// mPosition += deltaTime * mLook
	// If creating a vector from a single scalar variable, we use VectorReplicate
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
}

// Rotate the camera
void Camera::Pitch(float angle) {

	// Rotate up and look vector about the right vector

	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

void Camera::RotateY(float angle) {

	// Rotate the basic vectors about the world y-axis

	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));

}

// Update View Matrix after every frame 
void Camera::UpdateViewMatrix() {

	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mPosition);

	// Orthonormalize the right, up and look vectors

	// Make look vector unit length

	L = XMVector3Normalize(L);

	// Compute a new corrected "up" vector and normalize it

	U = XMVector3Normalize(XMVector3Cross(L, R));

	// Compute a new corrected "right" vector. U and L are already ortho-normal, so no need to normalize cross product
	// || up x look || = || up || || look || sin90 = l
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);
	XMStoreFloat3(&mLook, L);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;
}

void Camera::OnMouseMove(WPARAM btnState, int x, int y) {

	if ((btnState & MK_LBUTTON) != 0) {

		// Make each pixel to correspond to a quarter of a degree

		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));

		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		Pitch(dy);
		RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

void Camera::CreateFrustum()
{
	XMMATRIX ViewMatrix, ProjMatrix;
	XMFLOAT4X4 M;
	ViewMatrix = this->View();
	ProjMatrix = this->Proj();
	XMMATRIX VP = XMMatrixMultiply(ViewMatrix, ProjMatrix);
	XMVECTOR vecTest;
	
	XMStoreFloat4x4(&M, VP);
	
	XMVECTORF32 tempVec = { 0.0f, 0.0f, 0.0f, 0.0f };


	//Left plane
	
	this->Frustum[0].Normal.x = -(M._14 + M._11);
	this->Frustum[0].Normal.y = -(M._24 + M._21);
	this->Frustum[0].Normal.z = -(M._34 + M._41);
	this->Frustum[0].Distance = -(M._44 + M._41);

	//Right plane
	this->Frustum[1].Normal.x = -(M._14 + M._11);
	this->Frustum[1].Normal.y = -(M._24 + M._21);
	this->Frustum[1].Normal.z = -(M._34 + M._41);
	this->Frustum[1].Distance = -(M._44 + M._41);

	//Top plane
	this->Frustum[2].Normal.x = -(M._14 - M._12);
	this->Frustum[2].Normal.y = -(M._24 - M._22);
	this->Frustum[2].Normal.z = -(M._34 - M._42);
	this->Frustum[2].Distance = -(M._44 - M._42);

	//Bot Plane
	this->Frustum[3].Normal.x = -(M._14 + M._12);
	this->Frustum[3].Normal.y = -(M._24 + M._22);
	this->Frustum[3].Normal.z = -(M._34 + M._42);
	this->Frustum[3].Distance = -(M._44 + M._42);

	//Near Plane
	this->Frustum[4].Normal.x = -(M._14 + M._13);
	this->Frustum[4].Normal.y = -(M._24 + M._23);
	this->Frustum[4].Normal.z = -(M._34 + M._43);
	this->Frustum[4].Distance = -(M._44 + M._43);
				  
	//Far plane
	this->Frustum[5].Normal.x = -(M._14 - M._13);
	this->Frustum[5].Normal.y = -(M._24 - M._23);
	this->Frustum[5].Normal.z = -(M._34 - M._43);
	this->Frustum[5].Distance = -(M._44 - M._43);
	
	//Normalize all the planes
	for (size_t i = 0; i < 6; i++)
	{
		XMFLOAT3 pNorm = this->Frustum[i].Normal;
		XMVECTOR planeNorm = XMLoadFloat3(&pNorm);
		XMVector3Normalize(planeNorm);
		XMStoreFloat3(&this->Frustum[i].Normal, planeNorm);

	}
}
void Camera::testCreate()
{
	BoundingFrustum tempFrust;
	tempFrust.CreateFromMatrix(this->testFrust, this->Proj());
	XMMATRIX tempWorld;
	tempWorld = XMMatrixIdentity();
	XMStoreFloat4x4(&this->cWorld, tempWorld);
	this->testFrust.Transform(this->testFrust, tempWorld);

}