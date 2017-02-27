//----------------------------------------------------------------------------------------------------------------------------------//
// Quad Fragment Shader DirectX11
//
// Fredrik Linde TA15
//----------------------------------------------------------------------------------------------------------------------------------//

SamplerState texSampler: register(s0);
Texture2D tex0 : register(t0);
Texture2D tex1 : register(t1);

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_Target{

	float4 texColor, blurColor, color;

	texColor = tex0.Sample(texSampler, input.texcoord);

	blurColor = tex0.Sample(texSampler, input.texcoord);
	texColor = tex1.Sample(texSampler, input.texcoord);

	// Use these to turn off/on the gaussian blur in the scene

	//return blurColor;
	return texColor;

}