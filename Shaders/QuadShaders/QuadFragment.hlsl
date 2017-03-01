//----------------------------------------------------------------------------------------------------------------------------------//
// Quad Fragment Shader DirectX11
//
// Fredrik Linde TA15
//----------------------------------------------------------------------------------------------------------------------------------//

SamplerState texSampler: register(s0);
Texture2D tex0 : register(t0);

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_Target{

	float4 texColor, color;

	texColor = tex0.Sample(texSampler, input.texcoord);

	return texColor;

}