//----------------------------------------------------------------------------------------------------------------------------------//
// Geometry Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
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
	matrix viewProj;
	matrix floorRot;
	matrix matrixViewInverse;
	float4 cameraPos;
	float4 cameraUp;
	matrix worldInvTranspose;
	float normalMappingFlag;

};

cbuffer CUBE_CONSTANT_BUFFER : register(b1) {

	matrix cubeTransforms;
};

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex: TEXCOORD;

};

struct GS_OUT
{
	float4 Norm: NORMAL;
	float2 Tex: TEXCOORD;
	float4 Pos : SV_POSITION;
	float4 WPos : WPOSITION;
	
};

[maxvertexcount(3)]
 void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> triStream)
{	 
	 GS_OUT output;

	 uint i;

	float3 sideA = input[1].Pos.xyz - input[0].Pos.xyz;
	float3 sideB = input[2].Pos.xyz - input[0].Pos.xyz;

	float3 normalAB = normalize(cross(sideA, sideB));

	for (i = 0; i < 3; i++)
	{

		output.WPos = mul(float4(input[i].Pos.xyz, 1.0f), cubeTransforms);

		matrix cubeWVP = mul(cubeTransforms,viewProj);
		output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), cubeWVP);

		output.Norm = mul(float4(normalAB, 1.0f), cubeTransforms);
		output.Tex = input[i].Tex;

		triStream.Append(output);
	}

};