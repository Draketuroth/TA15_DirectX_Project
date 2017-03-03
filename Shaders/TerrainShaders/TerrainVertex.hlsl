//----------------------------------------------------------------------------------------------------------------------------------//
// Terrain Vertex Shader DirectX11
//
// Linnea Vajda TA15
//----------------------------------------------------------------------------------------------------------------------------------//

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
	float3x3 Tangent : TANGENT;
	//float2 BoundsY : TEXCOORD1;
};


VS_OUT VS_main(VS_IN input) {

	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos; 

	//output.Pos.y = heightMap.SampleLevel(heightSampler, input.Tex, 0).r;

	output.Tex = input.Tex;
	output.Norm = input.Norm;
	//output.BoundsY = input.BoundsY;

	//normalmap
	float3 tangentW = float3(0.0f, 0.0f, 0.0f); 

	//build orthonomal basis
	float3 N = input.Norm;
	float3 T = normalize(tangentW - dot(tangentW, N)*N);
	float3 B = cross(N, T);

	output.Tangent = float3x3(T, B, N);

	return output;
}
