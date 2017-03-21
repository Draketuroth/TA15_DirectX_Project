struct VS_IN
{
	float3 Pos : POSITION;
};
struct VS_OUT
{
	float4 Pos : SV_POSITION;

};

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
};

cbuffer TOPDOWN_CAMERA : register(b1) {

	matrix topDownViewTransform;
	matrix projectionInverse;
};

VS_OUT VS_main( VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos.xyz, 1.0f), projectionInverse);


	return output;
}