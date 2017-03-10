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

	/*if (input.position.x > 300 && input.position.x < 700) {

		return float4(1.0f, 0.0f, 0.0f, 1.0f);
	}

	if (input.position.x > 700 && input.position.x < 1024) {

	return float4(0.0f, 0.0f, 1.0f, 1.0f);
	}

	else {

		return float4(0.0f, 1.0f, 0.0f, 1.0f);
	}*/

}