//----------------------------------------------------------------------------------------------------------------------------------//
// Fragment Shader DirectX11
//
// Philip Velandria, Jonathan Sundberg, Linnea Vajda, Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - SamplerState uses "s"
// - Texture uses "t"

SamplerState texSampler: register(s0);
Texture2D tex0 : register(t0);

struct PS_IN
{
	//float4 Norm: NORMAL;
	//float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION;
	float3 ViewPos : POSITION1;
};

// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{

	return float4(1.0f,1.0f,1.0f,1.0f);

	//float3 lightSource = float3(0.0f, 0.0f, 0.0f);	// Light source in the form of a point light
	//float3 lightVector;
	//float lightIntensity;
	//float3 diffuseLight;
	//float3 specularLight;

	//float nDotL;
	//float3 texColor;
	//float4 color;

	//float shinyPower = 20.0f;

	//float3 Ld = float3(0.6f, 0.6f, 0.6f);	// Ld represents the light source intensity
	//float3 Ka = float3(0.2f, 0.2f, 0.2f);		// Ka is the hardcoded ambient light
	//float3 Ks = float3(1.0f, 1.0f, 1.0f);	// Ks is the hardcoded specular light
	//float3 Kd = float3(1.0f, 1.0f, 1.0f);	// Kd represents the diffuse reflectivity cofficient
	//float3 ads;

	//float3 n = normalize(input.Norm).xyz;	// The n component is self-explanatory, but represents the normal of the surface
	//float3 s = normalize(lightSource - input.WPos);	// The s component represents the direction from the surface to light source in world coordinates
	//float3 v = normalize(input.ViewPos).xyz;	// The v component represents the viewer position in world coordinates
	//float3 r = reflect(-s.xyz, n);	// The r component represent the reflection of the light direction vector with the the normal n

	//diffuseLight = Kd * max(dot(s, n), 0.0f);

	//specularLight = Ks * pow(max(dot(r, v), 0.0f), shinyPower);

	//ads = Ld * (Ka + diffuseLight + specularLight);

	//// Now the Sample state will sample the color output from the texture file so that we can return the correct color

	////texColor = tex0.Sample(texSampler, input.Tex).xyz;

	//color = (200, 200, 200);		//float4(texColor, 1.0f);

	/*return float4(ads, 1.0f) * color;*/
};