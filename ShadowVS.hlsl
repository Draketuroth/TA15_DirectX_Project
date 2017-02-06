struct VS_IN
{
	float4 Pos : SV_POSITION;
};
cbuffer GS_CONSTANT_BUFFER : register(b0)
{
	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix floorRot;
	matrix matrixViewProj;
	matrix lightViewProj;
};

VS_IN VS_main(VS_IN input) : SV_POSITION
{
	VS_IN output = (VS_IN)0;
	output.Pos = mul(float4(input.Pos.xyz, 1.0f), mul(matrixWorld, lightViewProj));

	return output;
}