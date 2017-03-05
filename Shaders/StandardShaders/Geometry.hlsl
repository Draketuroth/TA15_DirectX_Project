//----------------------------------------------------------------------------------------------------------------------------------//
// Geometry Shader DirectX11
//
// Jonathan Sundberg TA15
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - Buffers uses "b"

cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix lightViewProj;
	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix floorRot;
	matrix matrixViewInverse;
	float4 cameraPos;
	float4 cameraUp;
	matrix worldInvTranspose;
	float normalMappingFlag;

	
};

struct GS_IN
{
	float3 Pos : POSITION;
	
	

};

struct GS_OUT
{
	//float4 Norm: NORMAL;
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION;
	float3 ViewPos : POSITION1;
	
};

// The geometry shader takes an entire primitive as an array as its input, but it also require an upper limit for the amount of 
// vertices to output. Therefore the program runs for every primitive. Because we are going to output two triangles, the total 
// vertex count must be a total of 6

[maxvertexcount(4)]
 void GS_main(point GS_IN input[1], inout TriangleStream<GS_OUT> PStream)
{	 
	 GS_OUT output;

	
	 //Normalen för quaden.
	 float3 normal = cameraPos.xyz-input[0].Pos ;
	 normal = normalize(normal);

	 float3 cameraSpace = { 0, 1, 0 };
	  

	 
	
	 

	 
	 float3 normalizedCamera = normalize(cameraUp.xyz);


	 float3 rightVec = cross(cameraSpace,normal);
	 rightVec = normalize(rightVec);

	 float3 upVec = cross(normal, rightVec);
	 upVec = normalize(upVec);

	 rightVec = cross(upVec,normal);
	 rightVec = normalize(rightVec);
	 





	

	
	 
	float4 v = float4((input[0].Pos + (1 * rightVec) - (1 * upVec)),1.0f); // top left
	float4 v2 = float4((input[0].Pos + (1 * rightVec) + (1 * upVec)),1.0f); // bottom left
	float4 v3 = float4((input[0].Pos - (1 * rightVec) - (1 * upVec)),1.0f); // top right
	float4 v4 = float4((input[0].Pos - (1 * rightVec) + (1 * upVec)),1.0f); // bottom right

	float2 uvTL = {0.0f,0.0f};
	float2 uvBL = {1.0f,0.0f};
	float2 uvTR = {0.0f,1.0f};
	float2 uvBR = {1.0f,1.0f};

	// up from view matrix;


	float3 worldPosition = mul(float4(input[0].Pos, 1.0f), matrixWorld).xyz;

	 

	 output.ViewPos = cameraPos - worldPosition;
	 //output.Norm = normal;

	 output.WPos = worldPosition;

	// float4 pos1 = { input[0].Pos.x-1,input[0].Pos.y+1,input[0].Pos.z,1};
	// //float4 pos1 = { 0,1,0,1 };
	// float4 pos2 = { input[0].Pos.x-1,input[0].Pos.y - 1,input[0].Pos.z,1 };
	//// float4 pos2 = { 0,0,0,1};
	// float4 pos3 = { input[0].Pos.x+1,input[0].Pos.y + 1,input[0].Pos.z,1 };
	//// float4 pos3 = { 1,1,0,1};
	// float4 pos4 = { input[0].Pos.x+1,input[0].Pos.y - 1,input[0].Pos.z,1 };
	//// float4 pos4 = { 1,0,0,1};

	 float4 position = mul(v, worldViewProj);
	 float4 position2 = mul(v2, worldViewProj);
	 float4 position3 = mul(v3, worldViewProj);
	 float4 position4 = mul(v4, worldViewProj);



	 // top left
	 output.Pos = position;
	 output.Tex = uvTL;
	 PStream.Append(output);

	 // bottom left
	 output.Pos = position2;
	 output.Tex = uvBL;
	 PStream.Append(output);

	 // top right
	 output.Pos = position3;
	 output.Tex = uvTR;
	 PStream.Append(output);

	 //bottom left
	 output.Pos = position4;
	 output.Tex = uvBR;
	 PStream.Append(output);

	
	

	 //PStream.Append(output);	// The output stream can be seen as list which adds the most recent vertex to the last position in that list
	 

};