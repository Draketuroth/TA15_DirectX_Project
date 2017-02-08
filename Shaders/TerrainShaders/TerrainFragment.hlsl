
SamplerState texSampler: register(s0);
Texture2D tex0 : register(t0);

cbuffer MTL_STRUCT : register (b0)
{
	float3 Kd;
	float3 Ka;
	float3 tf;
	float ni;
	float illum;
};



struct PS_IN
{
	float4 Pos : SV_POSITION;
	float4 WPos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm: NORMAL;
	
	
	
};

// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{
	float4 lightSource = float4(0.0f, 5000.0f, 500.0f, 0.0f);	// Light source in the form of a point light
	float3 lightVector;
	float lightIntensity;
	float3 diffuseLight;
	float3 specularLight;

	

	
	float nDotL;
	float3 texColor;
	float4 color;
	
	float shinyPower = 8.0f;

	float3 Ld = float3(0.4f, 0.4f, 0.4f);	// Ld represents the light source intensity


	float3 Ka = float3(0.6f, 0.6f, 0.6f);		// Ka is the hardcoded ambient light
	float3 Ks = float3(0.7f, 0.7f, 0.7f);	// Ks is the hardcoded specular light
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

	lightVector = lightSource.xyz - input.WPos.xyz;

	// We don't need to check the angle of cosine between N & L, since we are working with unit vectors so it can't go bigger than 1.0f and backface culling is also active

	nDotL = dot(input.Norm.xyz, normalize(lightVector.xyz));

	// Now the Sample state will sample the color output from the texture file so that we can return the correct color

	texColor = tex0.Sample(texSampler, input.Tex).xyz;

	color = float4(texColor, 1.0f);

	return float4(ads, 1.0f) * color;
};