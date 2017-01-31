#include "BufferComponents.h"

BufferComponents::BufferComponents() {

	gVertexBuffer = nullptr;	
	gConstantBuffer = nullptr;	

	depthStencil = nullptr;
	depthState = nullptr;	
	depthView = nullptr;
}

BufferComponents::~BufferComponents() {

	gVertexBuffer->Release();
	gConstantBuffer->Release();
	depthStencil->Release();
	depthState->Release();
	depthView->Release();

}

void BufferComponents::SetupScene(ID3D11Device* &gDevice, Camera &mCam, FbxImport &fbxImporter) {

	CreateVertexBuffer(gDevice);
	CreateSkeletalBuffers(gDevice, fbxImporter);
	CreateConstantBuffer(gDevice, mCam);

}

bool BufferComponents::CreateVertexBuffer(ID3D11Device* &gDevice) {

	HRESULT hr;

	TriangleVertex triangleVertices[6] =
	{

		-0.5f, -0.5f, 0.0f,	//v1 position	(LEFT BOTTOM)
		0.0f, 1.0f,	//v1 uv coordinates

		-0.5f, 0.5f, 0.0f,	//v2 position	(LEFT TOP)
		0.0f, 0.0f,	//v2 uv coordinates

		0.5f, 0.5f, 0.0f, //v3 position	(RIGHT TOP)
		1.0f, 0.0f,	//v3 uv coordinates

		-0.5f, -0.5f, 0.0f,	//v4 pos position	(LEFT BOTTOM)
		0.0f, 1.0f,	//v4 uv coordinates

		0.5f, 0.5f, 0.0f,	//v5 position	(RIGHT TOP)
		1.0f, 0.0f,	//v5 uv coordinates

		0.5f, -0.5f, 0.0f,  //v6 position	(RIGHT BOTTOM)
		1.0f, 1.0f    //v6 uv coordinates
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(triangleVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &gVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool BufferComponents::CreateSkeletalBuffers(ID3D11Device* &gDevice, FbxImport &fbxImporter) {

	HRESULT hr;

	fbxImporter.LoadFBX(&fbxVector);

	VS_SKINNED_DATA skinData;

	for (unsigned int i = 0; i < fbxImporter.meshSkeleton.hierarchy.size(); i++) {

		XMMATRIX boneTransform = fbxImporter.Load4X4JointTransformations(fbxImporter.meshSkeleton.hierarchy[i], i);

		skinData.gBoneTransform[i] = boneTransform;
		fbxImporter.localTransform[i] = boneTransform;

	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// BONE BUFFER DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC boneBufferDesc;

	memset(&boneBufferDesc, 0, sizeof(boneBufferDesc));

	boneBufferDesc.ByteWidth = sizeof(VS_SKINNED_DATA);
	boneBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	boneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	boneBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA boneData;
	boneData.pSysMem = &skinData;
	boneData.SysMemPitch = 0;
	boneData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&boneBufferDesc, &boneData, &fbxImporter.gBoneBuffer);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//

	//----------------------------------------------------------------------------------------------------------------------------------//
	// VERTEX BUFFER DESCRIPTION
	//----------------------------------------------------------------------------------------------------------------------------------//

	D3D11_BUFFER_DESC bufferDesc;

	memset(&bufferDesc, 0, sizeof(bufferDesc));

	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = fbxVector.size() * sizeof(Vertex_Bone);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &fbxVector[0];
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &fbxImporter.gBoneVertexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------------------//

	return true;
}

bool BufferComponents::CreateConstantBuffer(ID3D11Device* &gDevice, Camera &mCam) {	// Function to create the constant buffer

	HRESULT hr;

	// Below I define the constant data used to communicate with the shaders

	//----------------------------------------------------------------------------------------------------------------------------------//

	// The world matrix is responsible for determining the position and orientation of an object in 3D space. This should only represent rotation around the Y-axis.
	// Since we know cos(0) returns 1, the world matrix should resemble an identity matrix
	XMMATRIX worldMatrix = XMMatrixRotationY(0.0f);

	XMMATRIX floorRot = XMMatrixTranspose(XMMatrixRotationX(90.0f));
	//----------------------------------------------------------------------------------------------------------------------------------//

	// The view matrix is used to to transform an object's vertices from world space to view space, written in Row major.
	// Using the following method, the matrix can be computed from the world position of the camera (eye), a global up vector, and a 
	// target point.

	DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat3(&XMFLOAT3(0, 0, 2));
	DirectX::XMVECTOR lookAt = DirectX::XMLoadFloat3(&XMFLOAT3(0, 0, 1));
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&XMFLOAT3(0, 1, 0));

	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePos, lookAt, up);
	mCam.LookAt(eyePos, lookAt, up);

	//----------------------------------------------------------------------------------------------------------------------------------//

	// The projection matrix is the actual camera we are looking through when viewing the world with its far and near clipping plane.
	// It's important to create this since we need it to link between view space and clip space

	float fov = PI * 0.45f;		// We recieve the field of view in radians by multiplying with PI

	float aspectRatio = WIDTH / HEIGHT;		// Using the already defined macros for the width and height of the viewport

	float nearPlane = 0.1f;

	float farPlane = 50.f;

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
	mCam.SetLens(fov, aspectRatio, nearPlane, farPlane);

	//----------------------------------------------------------------------------------------------------------------------------------//

	// Final calculation for the transform matrix and the transpose function rearranging it to "Column Major" before being sent to the GPU
	// (Required for the HLSL to read it correctly, doesn't accept matrices written in "Row Major"

	XMMATRIX finalCalculation = worldMatrix * viewMatrix * projectionMatrix;
	XMMATRIX tWorld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tWorldViewProj = XMMatrixTranspose(finalCalculation);
	transformMatrix = tWorldViewProj;
	tWorldMatrix = tWorld;

	//----------------------------------------------------------------------------------------------------------------------------------//

	// Here we supply the constant buffer data

	GS_CONSTANT_BUFFER GsConstData;

	GsConstData.matrixWorld = { tWorldMatrix };
	GsConstData.matrixView = { XMMatrixTranspose(viewMatrix) };
	GsConstData.matrixProjection = { XMMatrixTranspose(projectionMatrix) };
	GsConstData.worldViewProj = { tWorldViewProj };
	GsConstData.cameraPos = XMFLOAT3(0.0f, 0.0f, 2.0f);
	GsConstData.floorRot = { floorRot };

	// The buffer description is filled in below, mainly so the graphic card understand the structure of it

	D3D11_BUFFER_DESC constBufferDesc;
	constBufferDesc.ByteWidth = sizeof(GS_CONSTANT_BUFFER);
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	// We set the the subresource data

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &GsConstData;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	// Finally after creating description and subresource data, we create the constant buffer

	hr = gDevice->CreateBuffer(&constBufferDesc, &constData, &gConstantBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}