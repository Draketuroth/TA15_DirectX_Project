//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - SamplerState uses "s"
// - Texture uses "t"

SamplerState texSampler: register(s1);
Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);

float3 NormalToWorldSpace(float3 normalMapSample, float3 uintNormalW, float3 tangentW)
{
	//Uncompress each component from [0,1] to [-1,1]
	float3 normalT = 2.0f*normalMapSample - 1.0f; 

	//build orthonomal basis
	float3 N = uintNormalW; 
	float3 T = normalize(tangentW - dot(tangentW, N)*N); 
	float3 B = cross(N, T); 

	float3x3 TBN = float3x3(T, B, N); 
	
	//transform from target space to world space
	float3 bumpedNormalW = mul(normalT, TBN); 

	return bumpedNormalW; 
}

SamplerState samLinear : register (s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct PS_IN
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex : TEXCOORD;
	float3 EyePosW : POSITION1;
};


// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{

	input.NormalW = normalize(input.NormalW);

	float3 toEye = input.EyePosW - input.PosW;

	float3 distToEye = length(toEye);

	toEye /= distToEye;

	float4 texColor = float4(1, 1, 1, 1);

	texColor = gDiffuseMap.Sample(samLinear, input.Tex);

	float3 normalMapSample = gNormalMap.Sample(samLinear, input.Tex).rgb;
	float3 bumpedNormalW = NormalToWorldSpace(normalMapSample, input.NormalW, input.TangentW);

	return texColor;

};

