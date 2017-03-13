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

};
struct VS_INPUT
{
	float3 Pos : POSITION;

};
struct VS_OUT
{
	float4 Pos : SV_POSITION;
};

VS_OUT VS_main(VS_INPUT input) : SV_POSITION
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = mul(float4(input.Pos, 1.0f), matrixWorld);
	return output;
}