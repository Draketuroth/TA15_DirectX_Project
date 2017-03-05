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
Texture2D tex1 : register(t1);

struct PS_IN
{
	centroid float4 Norm: NORMAL;
	float3 Tangent : TANGENT;
	float3 Binormal : BINORMAL;
	float2 Tex:  TEXCOORD;
	float4 Pos : SV_POSITION;
	float4 WPos : WPOSITION;
};



// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{
	float4 lightSource = float4(0.0f, 0.0f, 20.0f, 0.0f);	// Light source in the form of a point light
	float3 lightVector;
	float lightIntensity;
	float3 diffuseLight;
	float3 specularLight;

	float nDotL;
	float3 texColor;
	float4 color;

	float3 bumpMap;
	float3 bumpNormal;

	float shinyPower = 7.0f;

	float3 Ld = float3(0.5f, 0.5f, 0.5f);	// Ld represents the light source intensity
	float3 Ka = float3(0.4f, 0.4f, 0.4f);		// Ka is the hardcoded ambient light
	float3 Ks = float3(0.8f, 0.8f, 0.8f);	// Ks is the hardcoded specular light
	float3 Kd = float3(1.0f, 1.0f, 1.0f);	// Kd represents the diffuse reflectivity cofficient
	float3 ads;

	float3 n = normalize(input.Norm);	// The n component is self-explanatory, but represents the normal of the surface
	float4 s = normalize(lightSource - input.WPos);	// The s component represents the direction from the surface to light source in world coordinates
	float4 v = normalize(input.WPos);	// The v component represents the viewer position in world coordinates
	float3 r = reflect(s.xyz, n);	// The r component represent the reflection of the light direction vector with the the normal n

	diffuseLight = Kd * max(dot(s, float4(n, 1.0f)), 0.0f);

	specularLight = Ks * pow(max(dot(float4(r, 1.0f), v), 0.0f), shinyPower);

	ads = Ld * (Ka + diffuseLight + specularLight);

	// We receive the light vector by subtracting the light source coordinates with the input position of the triangle in world coordinates
	// If we hadn't preserved the world coordinates, we would've subtracted with the screen space coordinates which gives us the wrong diffuse data

	lightVector = lightSource - input.WPos;

	// We don't need to check the angle of cosine between N & L, since we are working with unit vectors so it can't go bigger than 1.0f and backface culling is also active

	nDotL = dot(input.Norm.xyz, normalize(lightVector.xyz));

	// Now the Sample state will sample the color output from the texture file so that we can return the correct color

	texColor = tex0.Sample(texSampler, input.Tex).xyz;

	// Sample the pixel bump map
	bumpMap = tex1.Sample(texSampler, input.Tex).xyz;
	bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map
	bumpNormal = (bumpMap.x * input.Tangent) + (bumpMap.y * input.Binormal) + (bumpMap.z * input.Norm);
	bumpNormal = normalize(bumpNormal);

	// Light intensity
	lightIntensity = saturate(dot(bumpNormal, lightVector));

	color = saturate(float4(texColor, 1.0f) * lightIntensity);
	//color = float4(texColor, 1.0f);

	return color * float4(texColor, 1.0f) * float4(ads, 1.0f);
};

