#include "BufferComponents.h"

void importer(vector<OBJStruct> &ImportStruct, MTL_STRUCT &MTLConstandData, int ParserSwitch, bool &fileFound, wstring &OBJTexturePath)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	struct String3
	{
		string x, y, z;
	};

	String3 initialize;
	initialize.x = "";
	initialize.y = "";
	initialize.z = "";
	string* Farr;
	vector<XMFLOAT3> VertexArr;
	vector<XMFLOAT2> VertexTarr;
	vector<XMFLOAT3> VertexNarr;

	OBJStruct Filler;
	Filler.Varr.x = 0;
	Filler.Varr.y = 0;
	Filler.Varr.z = 0;

	Filler.VNarr.x = 0;
	Filler.VNarr.y = 0;
	Filler.VNarr.z = 0;

	Filler.VTarr.x = 0;
	Filler.VTarr.y = 0;


	int FarrSize = 50;

	int Vsize = 0;
	int VTsize = 0;
	int VNsize = 0;
	int Fsize = 0;



	XMFLOAT3 initializeFloat3;
	initializeFloat3.x = 0;
	initializeFloat3.y = 0;
	initializeFloat3.z = 0;


	string delimiter = "/";





	Farr = new string[FarrSize];

	XMFLOAT3 null;
	null.x = 0;
	null.y = 0;
	null.z = 0;

	XMFLOAT2 null1;
	null1.x = 0;
	null1.y = 0;



	string::size_type sz;
	string check;
	string check2;
	string check3;
	string check4;


	if (ParserSwitch == 0)
	{
		fstream file("OBJfiles//test//cube.obj", ios::in | ios::ate);
		if (!file.is_open())
		{
			fileFound = false;
			return;
		}
		fileFound = true;
		string line;


		file.seekg(0, file.end);
		int size = file.tellg();
		file.seekg(0, file.beg);


	


		//----------------------------------------------------------------------------------//
		while (!file.eof())
		{
			line.clear();


			getline(file, line);
			istringstream stringParse(line);


			stringParse >> check;

			if (check == "v")
			{
				stringParse >> check2;
				stringParse >> check3;
				stringParse >> check4;

				VertexArr.push_back(null);
				VertexArr[Vsize].x = stod(check2, &sz);
				VertexArr[Vsize].y = stod(check3, &sz);
				VertexArr[Vsize].z = stod(check4, &sz);
				Vsize++;
			}

			if (check == "vt")
			{
				stringParse >> check2;
				stringParse >> check3;
				VertexTarr.push_back(null1);
				VertexTarr[VTsize].x = stod(check2, &sz);
				VertexTarr[VTsize].y = stod(check3, &sz);
				VTsize++;
			}

			if (check == "vn")
			{
				stringParse >> check2;
				stringParse >> check3;
				stringParse >> check4;
				VertexNarr.push_back(null);
				VertexNarr[VNsize].x = stod(check2, &sz);
				VertexNarr[VNsize].y = stod(check3, &sz);
				VertexNarr[VNsize].z = stod(check4, &sz);
				VNsize++;
			}

			if (check == "f")
			{
				stringParse >> check;
				stringParse >> check2;
				stringParse >> check3;
				//stringParse >> check4;

				Farr[Fsize] = check;
				Fsize++;
				Farr[Fsize] = check2;
				Fsize++;
				Farr[Fsize] = check3;
				Fsize++;
				/*Farr[Fsize] = check4;
				Fsize++;*/
			}

			if (FarrSize - Fsize < 4)
			{
				FarrSize += 50;


				string* temp = new string[FarrSize];


				for (size_t i = 0; i < Fsize; i++)
				{
					temp[i] = Farr[i];
				}

				delete[] Farr;
				Farr = temp;
			}



		}


	
		size_t pos = 0;
		vector<String3> FaceList;
		vector<XMFLOAT3> converter;
	

		for (int i = 0; i < Fsize; i++)
		{
			FaceList.push_back(initialize);
			pos = Farr[i].find(delimiter);
			FaceList[i].x = Farr[i].substr(0, pos);
			Farr[i].erase(0, Farr[i].find(delimiter) + delimiter.length());

			pos = Farr[i].find(delimiter);
			FaceList[i].y = Farr[i].substr(0, pos);
			Farr[i].erase(0, Farr[i].find(delimiter) + delimiter.length());

			FaceList[i].z = Farr[i];
		

		}
		string::size_type typer;

		for (int u = 0; u < Fsize; u++)
		{
			converter.push_back(initializeFloat3);
			converter[u].x = stof(FaceList[u].x, &typer);
			converter[u].y = stof(FaceList[u].y, &typer);
			converter[u].z = stof(FaceList[u].z, &typer);
			converter[u].x -= 1;
			converter[u].y -= 1;
			converter[u].z -= 1;

			cout << converter[u].x << " " << converter[u].y << " " << converter[u].z << endl;
		}

	
		for (size_t i = 0; i < Fsize; i++)
		{
		
			ImportStruct.push_back(Filler);

			ImportStruct[i].Varr.x = VertexArr[converter[i].x].x;
			ImportStruct[i].Varr.y = VertexArr[converter[i].x].y;
			ImportStruct[i].Varr.z = VertexArr[converter[i].x].z;

			ImportStruct[i].VTarr.x = VertexTarr[converter[i].y].x;
			ImportStruct[i].VTarr.y = VertexTarr[converter[i].y].y;

			ImportStruct[i].VNarr.x = VertexNarr[converter[i].z].x;
			ImportStruct[i].VNarr.y = VertexNarr[converter[i].z].y;
			ImportStruct[i].VNarr.z = VertexNarr[converter[i].z].z;





		}
	}

	if (ParserSwitch == 1)
	{
		fstream mtl_File("OBJfiles//test//cube.mtl", ios::in | ios::ate);
	
		
		string temp = "OBJfiles//test//";
		wstring temp2;
		OBJTexturePath = (L"OBJfiles//test//");
		if (!mtl_File.is_open())
		{
			fileFound = false;
			return;
		}
		fileFound = true;
		string mtl_Line;
	
	
		mtl_File.seekg(0, mtl_File.end);
		int mtl_Size = mtl_File.tellg();
		mtl_File.seekg(0, mtl_File.beg);
	
	
	
		//string material;
		float illum;
		XMFLOAT3 Kd;
		XMFLOAT3 Ka;
		XMFLOAT3 Tf;
		XMFLOAT3 Ks;
		float Ni;
	
	
		while (!mtl_File.eof())
		{
			mtl_Line.clear();
	
	
			getline(mtl_File, mtl_Line);
			istringstream mtl_StringParse(mtl_Line);
	
			mtl_StringParse >> check;
			/*if (check == "newmtl")
			{
				mtl_StringParse >> material;
			}*/
			if (check == "illum")
			{
				mtl_StringParse >>check;
				illum = stof(check,&sz);
			}
			else if (check == "Kd")
			{
				mtl_StringParse >> check2;
				mtl_StringParse >> check3;
				mtl_StringParse >> check4;
	
				Kd.x = stof(check2,&sz);
				Kd.y = stof(check3,&sz);
				Kd.z = stof(check4,&sz);
	
			}
			else if (check == "Ka")
			{
				mtl_StringParse >> check2;
				mtl_StringParse >> check3;
				mtl_StringParse >> check4;
	
				Ka.x = stof(check2,&sz);
				Ka.y = stof(check3,&sz);
				Ka.z = stof(check4,&sz);
			}
			else if (check == "Tf")
			{
				mtl_StringParse >> check2;
				mtl_StringParse >> check3;
				mtl_StringParse >> check4;
	
				Tf.x = stof(check2,&sz);
				Tf.y = stof(check3,&sz);
				Tf.z = stof(check4,&sz);
			}
			else if (check == "Ni")
			{
				mtl_StringParse >> check;
	
				Ni = stof(check,&sz);
			}
			else if (check == "Ks")
			{
				mtl_StringParse >> check;
				mtl_StringParse >> check2;
				mtl_StringParse >> check3;

				Ks.x = stof(check, &sz);
				Ks.y = stof(check2, &sz);
				Ks.z = stof(check3, &sz);
			}
			else if (check == "map_Kd")
			{
				mtl_StringParse >> check;
				temp2.assign(check.begin(), check.end());
				OBJTexturePath.append(temp2);
				
			}
	
		}

		//--------- MTLCONSTANT DATA ASSIGNMENTS ---------------//
		MTLConstandData.Illum = illum;
		MTLConstandData.Ka.x = Ka.x;
		MTLConstandData.Ka.y = Ka.y;
		MTLConstandData.Ka.z = Ka.z;
		MTLConstandData.Kd.x = Kd.x;
		MTLConstandData.Kd.y = Kd.y;
		MTLConstandData.Kd.z = Kd.z;
		MTLConstandData.Tf.x = Tf.x;
		MTLConstandData.Tf.y = Tf.y;
		MTLConstandData.Tf.z = Tf.z;
		MTLConstandData.Ni = Ni;
		MTLConstandData.Ks.x = Ks.x;
		MTLConstandData.Ks.y = Ks.y;
		MTLConstandData.Ks.z = Ks.z;
		
	//-----------------------------------------------------------------//
		//cout << "material: " << material << endl;
		cout << "illum: " << illum << endl;
		cout << "kd: " << Kd.x << " " << Kd.y << " " << Kd.z << endl;
		cout << "ka: " << Ka.x << " " << Ka.y << " " << Ka.z << endl;
		cout << "Tf: " << Tf.x << " " << Tf.y << " " << Tf.z << endl;
		cout << "Ni: " << Ni << endl;
		cout << "Ks: " << Ks.x << " " << Ks.y << " " << Ks.z << endl;
	}

	

	delete[] Farr;


	//----------------------------------------------------------------------------------//





	



}

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
	CreateTerrainBuffer(gDevice);
	CreateOBJBuffer(gDevice);
	CreateRasterizerState(gDevice);

}

bool BufferComponents::CreateTerrainBuffer(ID3D11Device* &gDevice) {

	
	
	importer(ImportStruct,MTLConstantData,0,fileFound,OBJTexturePath);

	HRESULT hr;

	

	if (fileFound == true)
	{
		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = ImportStruct.size() * sizeof(OBJStruct);

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = &ImportStruct[0];
		hr = gDevice->CreateBuffer(&bufferDesc, &data, &gTerrainBuffer);

		if (FAILED(hr)) {

			return false;
		}

	}
	

	
	return true;
}

bool BufferComponents::CreateVertexBuffer(ID3D11Device* &gDevice) {

	HRESULT hr;

	TriangleVertex triangleVertices[1] =
	{

		-0.5f, 3.0f, 0.0f,	//v1 position	(LEFT BOTTOM)
		0.0f, 1.0f,	//v1 uv coordinates

		
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
		fbxImporter.invertedBindPose[i] = boneTransform;

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
	boneBufferDesc.MiscFlags = 0;
	boneBufferDesc.StructureByteStride = 0;

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

	D3D11_BUFFER_DESC bufferDesc = {};

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
	

	XMMATRIX viewMatrixInverse = XMMatrixInverse(NULL,viewMatrix);
	mCam.LookAt(eyePos, lookAt, up);

	//----------------------------------------------------------------------------------------------------------------------------------//

	// The projection matrix is the actual camera we are looking through when viewing the world with its far and near clipping plane.
	// It's important to create this since we need it to link between view space and clip space

	float fov = PI * 0.45f;		// We recieve the field of view in radians by multiplying with PI

	float aspectRatio = WIDTH / HEIGHT;		// Using the already defined macros for the width and height of the viewport

	float nearPlane = 0.1f;

	float farPlane = 500.f;

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
	mCam.SetLens(fov, aspectRatio, nearPlane, farPlane);


	//Matrices for the light, worldViewProjection, to use it for shadowmapping

	XMVECTOR lightPos = XMLoadFloat4(&XMFLOAT4(0, 20, 20, 1));
	XMVECTOR lightVec = XMLoadFloat4(&XMFLOAT4(0, 0, 0, 1));
	XMVECTOR upVector = XMLoadFloat4(&XMFLOAT4(0, 1, 0, 0));

	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, lightVec, upVector);

	//Light View matrix
	float lFov = PI * 0.20f;

	float lAspect = WIDTH / HEIGHT;

	float lNearPlane = 0.1f;

	float lFarPlane = 50.0f;

	//XMMATRIX lightProj = XMMatrixPerspectiveFoLH(lFov, lAspect, lNearPlane, lFarPlane);
	XMMATRIX lightProj = XMMatrixOrthographicLH(WIDTH / 100, HEIGHT / 100, lNearPlane, lFarPlane);
	XMMATRIX lightViewProj = lightView * lightProj;

	

	//----------------------------------------------------------------------------------------------------------------------------------//

	// Final calculation for the transform matrix and the transpose function rearranging it to "Column Major" before being sent to the GPU
	// (Required for the HLSL to read it correctly, doesn't accept matrices written in "Row Major"

	XMMATRIX finalCalculation = worldMatrix * viewMatrix * projectionMatrix;
	XMMATRIX tWorld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tWorldViewProj = XMMatrixTranspose(finalCalculation);
	XMMATRIX finalLightViewProj = XMMatrixTranspose(lightViewProj);
	transformMatrix = tWorldViewProj;
	tWorldMatrix = tWorld;
	tLightViewProj = finalLightViewProj;
	//----------------------------------------------------------------------------------------------------------------------------------//

	// Here we supply the constant buffer data

	GS_CONSTANT_BUFFER GsConstData;

	GsConstData.matrixWorld = { tWorldMatrix };
	GsConstData.matrixView = { XMMatrixTranspose(viewMatrix) };
	GsConstData.matrixProjection = { XMMatrixTranspose(projectionMatrix) };
	GsConstData.worldViewProj = { tWorldViewProj };
	GsConstData.cameraPos = XMFLOAT3(0.0f, 0.0f, 2.0f);
	GsConstData.floorRot = { floorRot };
	GsConstData.lightViewProj = { finalLightViewProj };
	GsConstData.matrixViewInverse = { viewMatrixInverse };
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

bool BufferComponents::CreateOBJBuffer(ID3D11Device* &gDevice)
{
	HRESULT hr;

	

	//----------------------------------------------------------------------------------------------------------------------------------//

	// Here we supply the constant buffer data

	

	importer(ImportStruct,MTLConstantData,1,fileFound,OBJTexturePath);

	// The buffer description is filled in below, mainly so the graphic card understand the structure of it



	
	
	D3D11_BUFFER_DESC MTLBufferDesc;
	MTLBufferDesc.ByteWidth = sizeof(MTL_STRUCT);
	MTLBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	MTLBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	MTLBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	MTLBufferDesc.MiscFlags = 0;
	MTLBufferDesc.StructureByteStride = 0;

	// We set the the subresource data

	D3D11_SUBRESOURCE_DATA MTLData;
	MTLData.pSysMem = &MTLConstantData;
	MTLData.SysMemPitch = 0;
	MTLData.SysMemSlicePitch = 0;

	// Finally after creating description and subresource data, we create the constant buffer

	hr = gDevice->CreateBuffer(&MTLBufferDesc, &MTLData, &gMTLBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool BufferComponents::CreateRasterizerState(ID3D11Device* &gDevice) {

	HRESULT hr;

	D3D11_RASTERIZER_DESC rasterizerDesc;

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;

	hr = gDevice->CreateRasterizerState(&rasterizerDesc, &gRasteriserState);

	if (FAILED(hr)) {

		return false;
	}

	return true;

}