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

// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{
	// The point light should be placed just one unit behind the camera position on the z-axis

	float4 lightSource = float4(0.0f, 0.0f, -3.0f, 0.0f);

	// We receive the light vector by subtracting the light source coordinates with the input position of the triangle in world coordinates
	// If we hadn't preserved the world coordinates, we would've subtracted with the screen space coordinates which gives us the wrong diffuse data

	float4 lightVector = lightSource - input.WPos;

	// We don't need to check the angle of cosine between N & L, since we are working with unit vectors so it can't go bigger than 1.0f and backface culling is also active

	float nDotL = dot(input.Norm.xyz, normalize(lightVector.xyz));

	// Now the Sample state will sample the color output from the texture file so that we can return the correct color

	float3 texSample = tex0.Sample(texSampler, input.Tex).xyz;

	float4 color = float4(texSample, 1.0f);

	return color * color;
};