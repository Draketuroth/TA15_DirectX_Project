#include "BufferComponents.h"
// OBJ �r i IMPORTER
// Particles �r i createVertexBuffer.

void importer(vector<OBJStruct> &ImportStruct, MTL_STRUCT &MTLConstandData, int ParserSwitch, bool &fileFound, wstring &OBJTexturePath)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);


	//----------------------------------------------------------------------------------------------------------------------------------//
	// STEP ONE: INITIALIZING VARIABLES.
	//----------------------------------------------------------------------------------------------------------------------------------//
	struct String3 // a custom "float3" for strings when we parse faces from OBJ.
	{
		string x, y, z;
	};

	String3 initialize; // empty string3 to initialize vectors
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

	string delimiter = "/"; // Delimiter anv�nds f�r att specifiera en karakt�r som vi kommer leta up n�r vi parsar faces.

	Farr = new string[FarrSize];

	// Tomma variabler f�r initsiering
	XMFLOAT3 null;
	null.x = 0;
	null.y = 0;
	null.z = 0;

	XMFLOAT2 null1;
	null1.x = 0;
	null1.y = 0;


	
	string::size_type sz; // anv�nds f�r convertering mellan string till floats.
	string check;
	string check2;
	string check3;
	string check4;



	//----------------------------------------------------------------------------------------------------------------------------------//
	// STEP TWO: PARSING VERTICES.
	//----------------------------------------------------------------------------------------------------------------------------------//
	if (ParserSwitch == 0) // vi kollar om vi parsar vertis information eller material information.
	{
		fstream file("OBJfiles//test//cube.obj", ios::in | ios::ate); // vi �ppnar filen

		if (!file.is_open()) // om filen inte finns eller inte �ppnas s� g�r vi en early return
		{
			fileFound = false;
			return;
		}

		fileFound = true; // annars har vi hittat filen.
		string line;

		
				
		file.seekg(0, file.beg);// vi s�tter parsen till b�rjan av filen.

		//----------------------------------------------------------------------------------//
		while (!file.eof()) // s�l�nge vi inte n�tt slutet av filen...
		{
			line.clear(); // vi rensar v�r string som vi parsar in OBJ str�ngens v�rde i fr�n f�rra loopen

			getline(file, line); // vi h�mtar en rad fr�n file.
			istringstream stringParse(line); // vi stoppar stringen i en istringstream.


			stringParse >> check; // vi skickar in allt fram tills f�rsta mellanslaget till v�r string check som vi parsar.

			if (check == "v") // om det vi tog var V s� vet vi att de n�sta 3 delarna av stringParse kommer vara vertisdata.
			{
				stringParse >> check2;
				stringParse >> check3;
				stringParse >> check4;

				VertexArr.push_back(null); // vi g�r en tom plats i v�r vector
				VertexArr[Vsize].x = stod(check2, &sz);// som vi sedan fyller i. converterar fr�n string till double.
				VertexArr[Vsize].y = stod(check3, &sz);
				VertexArr[Vsize].z = stod(check4, &sz);
				Vsize++;// plusar p� counter som h�ller reda p� hur m�nga vertiser vi parsat in.
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

				Farr[Fsize] = check;
				Fsize++;
				Farr[Fsize] = check2;
				Fsize++;
				Farr[Fsize] = check3;
				Fsize++;
				
			}

			if (FarrSize - Fsize < 4) // expand funktion f�r v�ran Face Array.
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
		vector<String3> FaceList; // vectoren f�r faces vi kommer s�tta in i x y z format.
		vector<XMFLOAT3> converter; // som vi sedan kommer konvertera till xmfloat3
	

		for (int i = 0; i < Fsize; i++)
		{
			FaceList.push_back(initialize); // ger facelist en tom plats

			pos = Farr[i].find(delimiter); // tilldelar pos positionsv�rdet av f�rsta "delimitern" om den �r p� plats 3 s� blir v�rdet 2.

			FaceList[i].x = Farr[i].substr(0, pos); // allting fram till f�rsta delimitern stoppar vi in i facelist.x

			Farr[i].erase(0, Farr[i].find(delimiter) + delimiter.length()); // vi deletar allting fram till och v�ran delimiter s� om det va "1/1/1" innan s� blir det "1/1" nu

			pos = Farr[i].find(delimiter); // hitta n�sta delimiter

			FaceList[i].y = Farr[i].substr(0, pos); //rinse

			Farr[i].erase(0, Farr[i].find(delimiter) + delimiter.length()); // repeat

			FaceList[i].z = Farr[i]; // n�r det inte finns n�gra delimiters kvar s� tar vi allt.
		

		}
		string::size_type typer; // anv�nds f�r convertering.

		for (int u = 0; u < Fsize; u++)
		{
			converter.push_back(initializeFloat3); // skapar en tom plats

			converter[u].x = stof(FaceList[u].x, &typer); // vi konverterar string till float
			converter[u].y = stof(FaceList[u].y, &typer);
			converter[u].z = stof(FaceList[u].z, &typer);
			converter[u].x -= 1; // eftersom OBJ b�rjar p� index 1 men c++ p� index 0 s� tar vi -1 p� alla v�rden.
			converter[u].y -= 1;
			converter[u].z -= 1;

			
		}

	
		for (size_t i = 0; i < Fsize; i++) // vi loopar antalet "faces" det �r inte primitiv faces men det �r ordningen som vertisdatan ska s�ttas ihop i.
										   // ("1/1/1" tar index 0 fr�n v,vt,vn) och det finns 3 s�nna per rad f�r att skapa en triangel som �r en primitiv.
		{
		
			ImportStruct.push_back(Filler);

			ImportStruct[i].Varr.x = VertexArr[converter[i].x].x; // vi stoppar in i Varr, VTarr och VNarr p� xyz fr�n v�ra vector listor med Varr VTarr och VNarr information.
			ImportStruct[i].Varr.y = VertexArr[converter[i].x].y; // vi anv�nder v�ra konverterade faces list f�r att indexera detta
			ImportStruct[i].Varr.z = VertexArr[converter[i].x].z; // converter[i] �r vilken index i vectorlistan vi h�mtar fr�n
																  // medans .X st�r f�r vilken typ vi indexerar f�r (Varr,VNarr,VTarr)
			ImportStruct[i].VTarr.x = VertexTarr[converter[i].y].x;
			ImportStruct[i].VTarr.y = VertexTarr[converter[i].y].y;

			ImportStruct[i].VNarr.x = VertexNarr[converter[i].z].x;
			ImportStruct[i].VNarr.y = VertexNarr[converter[i].z].y;
			ImportStruct[i].VNarr.z = VertexNarr[converter[i].z].z;

		}
	}
	//----------------------------------------------------------------------------------------------------------------------------------//
	// STEP THREE: PARSING MATERIAL ATTRIBUTES.
	//----------------------------------------------------------------------------------------------------------------------------------//

	if (ParserSwitch == 1) // kollar om vi parsar vertisdata eller materialdata
	{


		// HELA DENNA BIT �R IDENTISK I UTF�RANDET SOM VERTISDATA BITEN i utf�randet. 



		fstream mtl_File("OBJfiles//test//cube.mtl", ios::in | ios::ate);
	
		
		string temp = "OBJfiles//test//";
		wstring temp2;
		OBJTexturePath = (L"OBJfiles//test//"); // vi h�mtar destinationen till d�r v�ran textur ska ligga.

		if (!mtl_File.is_open())
		{
			fileFound = false;
			return;
		}

		fileFound = true;
		string mtl_Line;
	
	
		
		mtl_File.seekg(0, mtl_File.beg);
	
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
		
	
	}

	

	delete[] Farr; // deletar allokerat minne.

}

BufferComponents::BufferComponents() {

	gVertexBuffer = nullptr;	
	gTerrainBuffer = nullptr;
	gConstantBuffer = nullptr;	
	gMTLBuffer = nullptr;
	gVertexConstantBuffer = nullptr;

	depthStencil = nullptr;
	depthState = nullptr;	
	depthView = nullptr;

	gRasteriserState = nullptr;

	gCylinderBuffer = nullptr;
	gCylinderIndexBuffer = nullptr;

	gCubeIndexBuffer = nullptr;

}

BufferComponents::~BufferComponents() {
	

}

void BufferComponents::ReleaseAll() {

	SAFE_RELEASE(gVertexBuffer);
	SAFE_RELEASE(gTerrainBuffer);
	SAFE_RELEASE(gConstantBuffer);
	SAFE_RELEASE(gMTLBuffer);
	SAFE_RELEASE(gVertexConstantBuffer);

	SAFE_RELEASE(depthStencil);
	SAFE_RELEASE(depthState);
	SAFE_RELEASE(depthView);

	SAFE_RELEASE(gRasteriserState);

	SAFE_RELEASE(gCylinderBuffer);
	SAFE_RELEASE(gCylinderIndexBuffer);

	SAFE_RELEASE(gCubeIndexBuffer);

	for (int i = 0; i < CUBECAPACITY; i++) {

		SAFE_RELEASE(cubeObjects[i].gCubeVertexBuffer);
	}

	SAFE_RELEASE(topDownCameraBuffer);
	SAFE_RELEASE(gFrustumBuffer);
	SAFE_RELEASE(gFrustumIndexBuffer);
	SAFE_RELEASE(gArrowBuffer);
}

bool BufferComponents::SetupScene(ID3D11Device* &gDevice, Camera &mCam, FbxImport &fbxImporter) {

	if (!CreateVertexBuffer(gDevice)) {

		return false;
	}

	if(!CreateSkeletalBuffers(gDevice, fbxImporter)){

		return false;
	}

	if(!CreateConstantBuffer(gDevice, mCam)){

		return false;
	}

	if(!CreateTerrainBuffer(gDevice)){

		return false;
	}
	
	if(!CreateOBJBuffer(gDevice)){

		return false;
	
	}

	if(!CreateRasterizerState(gDevice)){
	
		return false;
	
	}
	
	if(!CreateVertexConstantBuffer(gDevice)){
	
		return false;
	
	}

	CreateCylinderBuffers(gDevice);

	if (!CreateCubeVertices(gDevice)) {

		return false;
	}

	if (!CreateCubeIndices(gDevice)) {

		return false;
	}

	if (!CreateTopDownCameraBuffer(gDevice)) {

		return false;
	}

	if (!CreateFrustumBuffer(gDevice)) {

		return false;
	}

	if (!CreateFrustumIndexBuffer(gDevice)) {

		return false;
	}

	return true;
}

bool BufferComponents::CreateTerrainBuffer(ID3D11Device* &gDevice) {

	float maxX = 0;
	float minX = 0;
	float maxY = 0;
	float minY = 0;
	float maxZ = 0;
	float minZ = 0;
	
	importer(ImportStruct,MTLConstantData,0,fileFound,OBJTexturePath);
	int arrayLength = ImportStruct.size();
	for (int i = 0; i < arrayLength; i++)
	{
		//cout << "X: " << ImportStruct[i].Varr.x << " Y: " << ImportStruct[i].Varr.y << " Z: " << ImportStruct[i].Varr.z << endl;

		if (maxX < ImportStruct[i].Varr.x)
		{
			maxX = ImportStruct[i].Varr.x;
		}
		if (minX > ImportStruct[i].Varr.x)
		{
			minX = ImportStruct[i].Varr.x;
		}
		if (maxY < ImportStruct[i].Varr.y)
		{								
			maxY = ImportStruct[i].Varr.y;
		}								
		if (minY > ImportStruct[i].Varr.y)
		{								
			minY = ImportStruct[i].Varr.y;
		}
		if (maxZ < ImportStruct[i].Varr.z)
		{								
			maxZ = ImportStruct[i].Varr.z;
		}								
		if (minZ > ImportStruct[i].Varr.z)
		{							
			minZ = ImportStruct[i].Varr.z;
		}
	}
	int extentX = 0;
	int extentY = 0;
	int extentZ = 0;
	XMFLOAT3 bbCenter;
	float XCenter = (maxX / 2) + (minX / 2);
	float YCenter = (maxY / 2) + (minY / 2);
	float ZCenter = (maxZ / 2) + (minZ / 2);
	bbCenter = { XCenter, YCenter, ZCenter };

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


	//----------------------------------------------------------------------------------------------------------------------------------//
	// PARTICLE POINT CREATION.
	//----------------------------------------------------------------------------------------------------------------------------------//
	HRESULT hr;
	

	
	

	TriangleVertex triangleVertices[1000]; // skapar en array med 1000 platser f�r vertispunkter.
	

	for (int i = 0; i < 1000; i++)
	{
		triangleVertices[i].posX.x = 0; // initsierar positionen
		triangleVertices[i].posX.y = 0;
		triangleVertices[i].posX.z = 0;
		
		XMFLOAT3 random = { 0,0,0 };
		float randomNum = rand() % 200 + (-100); // vi ger x och z v�rdena ett random v�rde mellan -100 och 100
		float randomNum2 = rand() % 100 +5; // Y axeln �r mellan 5 och 100 f�r att inte vara under marken.
		float randomNum3 = rand() % 200 + (-100);
		
		random.x = randomNum;
		random.y = randomNum2;
		random.z = randomNum3;
		
		triangleVertices[i].posX.x = random.x; // vi tildelar v�ra vertispunkter dessa random v�rden s� det blir ett random hav av vertispunkter.
		triangleVertices[i].posX.y = random.y;
		triangleVertices[i].posX.z = random.z;
		
	}
	
	// hoppa vidare till main.

	//----------------------------------------------------------------------------------------------------------------------------------//
	// PARTICLE VERTEX BUFFER
	//----------------------------------------------------------------------------------------------------------------------------------//

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

	fbxImporter.LoadFBX(&fbxVector); //load mesh vertices

	VS_SKINNED_DATA skinData; // constant buffer struct for inverse bindpose matrices.

	for (unsigned int i = 0; i < fbxImporter.meshSkeleton.hierarchy.size(); i++) {

		XMMATRIX inversedBindPose = fbxImporter.Load4X4JointTransformations(fbxImporter.meshSkeleton.hierarchy[i], i); // converts from float4x4 too xmmatrix

		skinData.gBoneTransform[i] = inversedBindPose;
		fbxImporter.invertedBindPose[i] = inversedBindPose; // copy on the cpu

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

	// now go to main and search for fbxImporter.animtimepos
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

	float aspectRatio = float(WIDTH) / float(HEIGHT);		// Using the already defined macros for the width and height of the viewport

	float nearPlane = NEARPLANE;

	float farPlane = FARPLANE;

	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);
	mCam.SetLens(fov, aspectRatio, nearPlane, farPlane);

	//Matrices for the light, worldViewProjection, to use it for shadowmapping

	XMVECTOR lightPos = XMLoadFloat4(&XMFLOAT4(40, 15, 20, 1));
	XMVECTOR lightVec = XMLoadFloat4(&XMFLOAT4(40, 0, 20, 0));
	XMVECTOR upVector = XMLoadFloat4(&XMFLOAT4(1, 0, 0, 0));

	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, lightVec, upVector);

	//Light View matrix
	float lFov = PI * 0.45f;

	float lAspect = float(WIDTH) / float(HEIGHT);

	// The far plane and near plane should be tight together in order to increase precision of the shadow mapping

	float lNearPlane = 0.1f;

	float lFarPlane = 50.f;
	
	XMMATRIX lightProj = XMMatrixPerspectiveFovLH(lFov, lAspect , lNearPlane, lFarPlane);
	//XMMATRIX lightProj = XMMatrixOrthographicLH(WIDTH, HEIGHT, lNearPlane, lFarPlane);
	XMMATRIX lightViewProj = XMMatrixMultiply(lightView, lightProj);

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
	GsConstData.cameraPos = XMFLOAT4(0.0f, 0.0f, 2.0f,1.0f);
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



	//----------------------------------------------------------------------------------------------------------------------------------//
	// LOAD MTL CONSTANT BUFFER.
	//----------------------------------------------------------------------------------------------------------------------------------//

	HRESULT hr;

	
	importer(ImportStruct,MTLConstantData,1,fileFound,OBJTexturePath);


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
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;

	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = false;

	hr = gDevice->CreateRasterizerState(&rasterizerDesc, &gRasteriserState);

	if (FAILED(hr)) {

		return false;
	}

	return true;

}

bool BufferComponents::CreateCylinderBuffers(ID3D11Device* &gDevice) {

	HRESULT hr;

	CylinderMeshData cylinder;
	CreateCylinder(3.0f, 3.0f, 20.0f, 15, 15, cylinder);

	cylinderVertexCount = cylinder.Vertices.size();
	cylinderIndicesCount = cylinder.Indices.size();

	vector<PosNormalTexTan> vertices(cylinderVertexCount);

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i)
	{
		vertices[i].Pos = cylinder.Vertices[i].Position;
		vertices[i].Normal = cylinder.Vertices[i].Normal;
		vertices[i].Tex = cylinder.Vertices[i].TexC;
		vertices[i].TangentU = cylinder.Vertices[i].TangentU;
	}

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(PosNormalTexTan) * cylinderVertexCount;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vertices[0];

	hr = gDevice->CreateBuffer(&bufferDesc, &data, &gCylinderBuffer);

	if (FAILED(hr)) {

		return false;
	}

	vector<UINT> indices;
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());

	// Create the buffer description
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * cylinderIndicesCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Set the subresource data

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &indices[0];

	// Create the buffer

	hr = gDevice->CreateBuffer(&indexBufferDesc, &initData, &gCylinderIndexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;

}

void BufferComponents::CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, CylinderMeshData& meshData)
{
	meshData.Vertices.clear();
	meshData.Indices.clear();

	// Build Stacks.

	float stackHeight = height / stackCount;

	// Amount to increment radius as we move up each stack level from bottom to top.
	float radiusStep = (topRadius - bottomRadius) / stackCount;

	UINT ringCount = stackCount + 1;

	// Compute vertices for each stack ring starting at the bottom and moving up.
	for (UINT i = 0; i < ringCount; ++i)
	{
		float y = -0.5f*height + i*stackHeight;
		float r = bottomRadius + i*radiusStep;

		// vertices of ring
		float dTheta = 2.0f*XM_PI / sliceCount;
		for (UINT j = 0; j <= sliceCount; ++j)
		{
			Vertex_Cylinder vertex;

			float c = cosf(j*dTheta);
			float s = sinf(j*dTheta);

			vertex.Position = XMFLOAT3(r*c, y, r*s);

			vertex.TexC.x = (float)j / sliceCount;
			vertex.TexC.y = 1.0f - (float)i / stackCount;

			// Cylinder can be parameterized as follows, where we introduce v
			// parameter that goes in the same direction as the v tex-coord
			// so that the bitangent goes in the same direction as the v tex-coord.

			// This is unit length.
			vertex.TangentU = XMFLOAT3(-s, 0.0f, c);

			float dr = bottomRadius - topRadius;
			XMFLOAT3 bitangent(dr*c, -height, dr*s);

			XMVECTOR T = XMLoadFloat3(&vertex.TangentU);
			XMVECTOR B = XMLoadFloat3(&bitangent);
			XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
			XMStoreFloat3(&vertex.Normal, N);

			meshData.Vertices.push_back(vertex);
		}
	}

	// Add one because we duplicate the first and last vertex per ring
	// since the texture coordinates are different.
	UINT ringVertexCount = sliceCount + 1;

	// Compute indices for each stack.
	for (UINT i = 0; i < stackCount; ++i)
	{
		for (UINT j = 0; j < sliceCount; ++j)
		{
			meshData.Indices.push_back(i*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)*ringVertexCount + j + 1);

			meshData.Indices.push_back(i*ringVertexCount + j);
			meshData.Indices.push_back((i + 1)*ringVertexCount + j + 1);
			meshData.Indices.push_back(i*ringVertexCount + j + 1);
		}
	}

	BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
}

void BufferComponents::BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, CylinderMeshData& meshData)
{
	// Build top cap.

	UINT baseIndex = (UINT)meshData.Vertices.size();

	float y = 0.5f*height;
	float dTheta = 2.0f*XM_PI / sliceCount;

	// Duplicate cap ring vertices because the texture coordinates and normals differ.
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = topRadius*cosf(i*dTheta);
		float z = topRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(Vertex_Cylinder(x, y, z, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	meshData.Vertices.push_back(Vertex_Cylinder(0.0f, y, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	// Index of center vertex.
	UINT centerIndex = (UINT)meshData.Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.Indices.push_back(centerIndex);
		meshData.Indices.push_back(baseIndex + i + 1);
		meshData.Indices.push_back(baseIndex + i);
	}
}

void BufferComponents::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, CylinderMeshData& meshData)
{
	
	// Build bottom cap.

	UINT baseIndex = (UINT)meshData.Vertices.size();
	float y = -0.5f*height;

	// vertices of ring
	float dTheta = 2.0f*XM_PI / sliceCount;
	for (UINT i = 0; i <= sliceCount; ++i)
	{
		float x = bottomRadius*cosf(i*dTheta);
		float z = bottomRadius*sinf(i*dTheta);

		// Scale down by the height to try and make top cap texture coord area
		// proportional to base.
		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		meshData.Vertices.push_back(Vertex_Cylinder(x, y, z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, u, v));
	}

	// Cap center vertex.
	meshData.Vertices.push_back(Vertex_Cylinder(0.0f, y, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.5f, 0.5f));

	// Cache the index of center vertex.
	UINT centerIndex = (UINT)meshData.Vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; ++i)
	{
		meshData.Indices.push_back(centerIndex);
		meshData.Indices.push_back(baseIndex + i);
		meshData.Indices.push_back(baseIndex + i + 1);
	}
}

bool BufferComponents::CreateVertexConstantBuffer(ID3D11Device* &gDevice)
{
	HRESULT hr;




	D3D11_BUFFER_DESC VtxBufferDesc;
	VtxBufferDesc.ByteWidth = sizeof(VS_CONSTANT_BUFFER);
	VtxBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	VtxBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	VtxBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VtxBufferDesc.MiscFlags = 0;
	VtxBufferDesc.StructureByteStride = 0;

	// We set the the subresource data

	D3D11_SUBRESOURCE_DATA VtxData;
	VtxData.pSysMem = &VtxConstantData;
	VtxData.SysMemPitch = 0;
	VtxData.SysMemSlicePitch = 0;

	// Finally after creating description and subresource data, we create the constant buffer

	hr = gDevice->CreateBuffer(&VtxBufferDesc, &VtxData, &gVertexConstantBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool BufferComponents::CreateCubeVertices(ID3D11Device* &gDevice) {

	HRESULT hr;

	//----------------------------------------------------------------------------------------------------------------------------------//
	// INITIALIZE OFFSET VARIABLES
	//----------------------------------------------------------------------------------------------------------------------------------//

	float xOffsetValue = 0.0f;
	float yOffsetValue = 0.0f;
	float zOffsetValue = 0.0f;
	float spacing = 0.0f;

	//srand(time(NULL));

	for(int i = 0; i < CUBECAPACITY; i++){

		//----------------------------------------------------------------------------------------------------------------------------------//
		// RANDOMIZE NEW OFFSET VALUES FOR EACH CUBE
		//----------------------------------------------------------------------------------------------------------------------------------//

		xOffsetValue = RandomNumber(-15, 15);
		yOffsetValue = RandomNumber(5, 15);
		zOffsetValue = RandomNumber(-15, 15);
		spacing = RandomNumber(-20, 20);

		//----------------------------------------------------------------------------------------------------------------------------------//
		// HARDCODED VERTICES
		//----------------------------------------------------------------------------------------------------------------------------------//

		Vertex_Cube cubeVertices[24] =
		{

			//Front face

			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			1.0, -1.0f, -1.0f, 1.0f, 1.0f,

			// Back face

			1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
			-1.0, -1.0f, 1.0f, 1.0f, 1.0f,

			// Left face

			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,

			// Right face

			1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, -1.0f,  1.0f, 1.0f, 1.0f,

			// Top face

			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, -1.0f, 1.0f, 1.0f,

			// Bottom face

			1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
			1.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 1.0f, 1.0f


		};

		//----------------------------------------------------------------------------------------------------------------------------------//
		// OFFSET VERTICES
		//----------------------------------------------------------------------------------------------------------------------------------//

		for (int j = 0; j < 24; j++) {

			cubeVertices[j].x += xOffsetValue + spacing;
			cubeVertices[j].y += yOffsetValue;
			cubeVertices[j].z += zOffsetValue + spacing;
			
		}

		//----------------------------------------------------------------------------------------------------------------------------------//
		// FILL LIST OF VERTICES FOR BOUNDING BOX CREATION
		//----------------------------------------------------------------------------------------------------------------------------------//

		XMFLOAT3 boundingPoints[24];

		for (int k = 0; k < 24; k++) {

			boundingPoints[k].x = cubeVertices[k].x;
			boundingPoints[k].y = cubeVertices[k].y;
			boundingPoints[k].z = cubeVertices[k].z;
		}

		//----------------------------------------------------------------------------------------------------------------------------------//
		// CREATE VERTEX BUFFER
		//----------------------------------------------------------------------------------------------------------------------------------//

		D3D11_BUFFER_DESC bufferDesc;
		memset(&bufferDesc, 0, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = sizeof(cubeVertices);

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = cubeVertices;
		hr = gDevice->CreateBuffer(&bufferDesc, &data, &cubeObjects[i].gCubeVertexBuffer);

		if (FAILED(hr)) {

			return false;
		}

		//----------------------------------------------------------------------------------------------------------------------------------//
		// TRANSFORM BOUNDING BOX AND INITIALIZE RENDER CHECK BOOLEAN VARIABLE
		//----------------------------------------------------------------------------------------------------------------------------------//

		cubeObjects[i].objectWorldMatrix = XMMatrixIdentity();
		XMMATRIX transform = XMMATRIX(cubeObjects[i].objectWorldMatrix);

		BoundingBox::CreateFromPoints(cubeObjects[i].bbox, 24, boundingPoints, 0);

		cubeObjects[i].bbox.Extents = { 2, 2, 2 };

		cubeObjects[i].bbox.Transform(cubeObjects[i].bbox, transform);

		//----------------------------------------------------------------------------------------------------------------------------------//
		// INITIALIZE RENDER CHECK TO FALSE FOR ALL CUBES
		//----------------------------------------------------------------------------------------------------------------------------------//
		
		cubeObjects[i].renderCheck = false;
		XMFLOAT3 corners[8];
		cubeObjects[i].bbox.GetCorners(corners);

		//cout << "X:  " << corners[i].x << "  Y:  " << corners[i].y << "  Z:  " << corners[i].z << endl;

	}

	//----------------------------------------------------------------------------------------------------------------------------------//
	// RENDER CHECK TEST
	//----------------------------------------------------------------------------------------------------------------------------------//

	/*for (int a = 0; a < 40; a++) {

		cubeObjects[a].renderCheck = false;
	}*/

	return true;
}

bool BufferComponents::CreateCubeIndices(ID3D11Device* &gDevice) {

	HRESULT hr;

	// Create Indices
	unsigned int indices[] = {

		// Front face
		0,1,2,
		2,1,3,

		// Back face

		4,5,6,
		6,5,7,

		// Left face

		8,9,10,
		10,9,11,

		// Right face

		12,13,14,
		14,13,15,

		// Top face

		16,17,18,
		18,17,19,

		// Bottom face

		20,21,22,
		22,21,23 };

	// Create the buffer description
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * 36;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Set the subresource data

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer

	hr = gDevice->CreateBuffer(&bufferDesc, &initData, &gCubeIndexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

float BufferComponents::RandomNumber(float Minimum, float Maximum) {

	return ((float(rand()) / float(RAND_MAX)) * (Maximum - Minimum)) + Minimum;
}

bool BufferComponents::CreateTopDownCameraBuffer(ID3D11Device* &gDevice) {

	HRESULT hr;

	XMVECTOR eyePos = DirectX::XMLoadFloat3(&XMFLOAT3(0, 50, 2));
	XMVECTOR lookAt = DirectX::XMLoadFloat3(&XMFLOAT3(0, 0, 1));
	XMVECTOR up = DirectX::XMLoadFloat3(&XMFLOAT3(0, 1, 0));

	XMMATRIX topDownViewMatrix = XMMatrixLookAtLH(eyePos, lookAt, up);

	topDownCamData.topDownViewTransform = XMMatrixTranspose(topDownViewMatrix);
	topDownCamData.projectionInverse = XMMatrixIdentity();

	D3D11_BUFFER_DESC constBufferDesc;
	constBufferDesc.ByteWidth = sizeof(TOPDOWN_CAMERA);
	constBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constBufferDesc.MiscFlags = 0;
	constBufferDesc.StructureByteStride = 0;

	// We set the the subresource data

	D3D11_SUBRESOURCE_DATA constData;
	constData.pSysMem = &topDownCamData;
	constData.SysMemPitch = 0;
	constData.SysMemSlicePitch = 0;

	hr = gDevice->CreateBuffer(&constBufferDesc, &constData, &topDownCameraBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}

bool BufferComponents::CreateArrowBuffer(ID3D11Device* &gDevice, Camera &mCam) {

	HRESULT hr;

	Vertex_Frustum frustumVertices[2] = {

		0.0f, 0.0f, 0.0f,
		mCam.GetLook().x, mCam.GetLook().y, mCam.GetLook().z
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(frustumVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = frustumVertices;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &gArrowBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;

}

bool BufferComponents::CreateFrustumBuffer(ID3D11Device* &gDevice) {

	HRESULT hr;

	Vertex_Frustum frustumVertices[8] = {

		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		 1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		 1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f
	};

	D3D11_BUFFER_DESC bufferDesc;
	memset(&bufferDesc, 0, sizeof(bufferDesc));
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(frustumVertices);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = frustumVertices;
	hr = gDevice->CreateBuffer(&bufferDesc, &data, &gFrustumBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;

}

bool BufferComponents::CreateFrustumIndexBuffer(ID3D11Device* &gDevice) {

	HRESULT hr;

	// Create Indices
	unsigned int pointIndices[24] = 
	
	{ 0,1, 0,4, 0,2, 1,5, 1,3, 5,4, 2,3, 2,6, 3,7, 6,7, 4,6, 5,7 };

	// Create the buffer description
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(unsigned int) * 24;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	// Set the subresource data

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pointIndices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Create the buffer

	hr = gDevice->CreateBuffer(&bufferDesc, &initData, &gFrustumIndexBuffer);

	if (FAILED(hr)) {

		return false;
	}

	return true;
}