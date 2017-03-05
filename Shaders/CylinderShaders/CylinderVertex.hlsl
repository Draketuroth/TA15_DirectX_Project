//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

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

struct VS_IN
{
	float3 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
	float3 Tangent: TANGENT;
};

struct VS_OUT
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex : TEXCOORD;
	float4 EyePosW : POSITION1;
	float normalFlag : POSITION2;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.PosW = mul(float4(input.Pos + float3(50.0f, 10.0f, 0.0f), 1.0f), matrixWorld).xyz;
	output.NormalW = mul(input.Normal, (float3x3)worldInvTranspose);
	output.TangentW = mul(input.Tangent, (float3x3)matrixWorld);

	output.PosH = mul(float4(input.Pos + float3(50.0f, 10.0f, 0.0f), 1.0f), worldViewProj);

	output.Tex = input.Tex;
	output.EyePosW = cameraPos;
	output.normalFlag = normalMappingFlag;

	return output;
}


