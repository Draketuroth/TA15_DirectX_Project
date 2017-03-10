//----------------------------------------------------------------------------------------------------------------------------------//
// Shadow Map Vertex Shader DirectX11
//
// Philip Velandria TA15
//----------------------------------------------------------------------------------------------------------------------------------//

struct VS_IN{
	float3 Pos : POSITION;
};
struct VS_OUT{
	float4 Pos : SV_POSITION;
};
cbuffer GS_CONSTANT_BUFFER : register(b0){
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


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), mul(matrixWorld, lightViewProj));

	return output;
}