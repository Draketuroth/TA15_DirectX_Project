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
	matrix floorRot;
	float3 cameraPos;

};


VS_OUT VS_main(VS_IN input) : SV_POSITION
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), mul(matrixWorld, lightViewProj));

	return output;
}