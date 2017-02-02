
Texture2D heightMap : register(t0);

SamplerState heightSampler {

	Filter = MIN_MAG_LINEAR_MIP_POINT;

	AddressU = CLAMP;
	AddresV = CLAMP;
};

struct VS_IN {

	float3 Pos	: POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	//float2 BoundsY : TEXCOORD1;
};

struct VS_OUT {

	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm : NORMAL;
	//float2 BoundsY : TEXCOORD1;
};

VS_OUT VS_main(VS_IN input) {

	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos; 

	//output.Pos.y = heightMap.SampleLevel(heightSampler, input.Tex, 0).r;

	output.Tex = input.Tex;
	output.Norm = input.Norm;
	//output.BoundsY = input.BoundsY;

	return output;
}