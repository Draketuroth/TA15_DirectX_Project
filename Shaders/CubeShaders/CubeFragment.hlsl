//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - SamplerState uses "s"
// - Texture uses "t"

SamplerState texSampler: register(s0);
Texture2D tex0 : register(t0);

struct PS_IN
{
	float3 Norm: NORMAL;
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float4 WPos : WPOSITION;
};


float4 PS_main(PS_IN input) : SV_Target
{

	float4 lightSource = float4(0.0f, 0.0f, -3.0f, 1.0f);

	float4 lightVector = lightSource - input.WPos;

	float nDotL = dot(input.Norm.xyz, normalize(lightVector.xyz));

	float3 texSample = tex0.Sample(texSampler, input.Tex).xyz;

	float4 color = float4(texSample, 1.0f);

	return color;
};