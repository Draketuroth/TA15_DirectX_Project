//----------------------------------------------------------------------------------------------------------------------------------//
// Terrain Fragment Shader DirectX11
//
// Linnea Vajda TA15
//----------------------------------------------------------------------------------------------------------------------------------//

SamplerState texSampler: register(s0);
SamplerState shadowSampler : register(s1);
Texture2D tex0 : register(t0);
Texture2D shadowMap : register(t1);

cbuffer MTL_STRUCT : register (b0)
{
	float4 Kd; // Diffuse color
	float4 Ka; // ambient color
	float4 tf;
	float4 Ks; // specular color
	float ni;
	float illum;
	float2 padding;
};



struct PS_IN
{
	float3 Norm: NORMAL;
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION;
	float3 ViewPos : POSITION1;
	float4 lPos : TEXCOORD1;
};


//float3 NormalToWorldSpace(float3 normalMapSample, float3 uintNormalW, float3 tangentW); 

// The transformed geometry from the geometry shader is now mapped onto the active Render Target, which will be our back buffer
float4 PS_main(PS_IN input) : SV_Target
{

	float3 lightSource = float3(0.0f, 20.0f, 20.0f);	// Light source in the form of a point light
	float3 lightVector;
	float lightIntensity;
	float3 diffuseLight;
	float3 specularLight;
	
	//Manually have to divide all the elements of lPos by W since it is not passed as a SV_POSITION
	input.lPos.xy /= input.lPos.w; //light pos in NDC

	//getting the light pos from [-1, 1] to [0, 1]
	float2 smTexture = float2(0.5f * input.lPos.x + 0.5f, -0.5f * input.lPos.y + 0.5f);

	//pixel depth for shadows
	float depth = input.lPos.z / input.lPos.w;//The depth from the point in the light coordinate system(not the point sampled from the shadow map)
	
	float shadowCheck = (shadowMap.Sample(shadowSampler, smTexture).r + 0.0001f < depth) ? 0.25f : 1.0f;//this returns either 0.25 or 1 depending on if the distance from the sampled point(shadow map)
	//is less than the distance from the vertex point of the geometry (that is being handled in the current pixel) to the light position, if that is true, it means that it is in shadow

	float nDotL;
	float3 texColor;
	float3 color;

	float shinyPower = 20.0f;

	float3 Ld2 = float3(1.0f, 1.0f, 1.0f);	// Ld represents the light source intensity
	float3 Ka2 = float3(0.8f, 0.8f, 0.8f);		// Ka is the hardcoded ambient light
	float3 Ks2 = float3(1.0f, 1.0f, 1.0f);	// Ks is the color of the hardcoded specular light
	float3 Kd2 = float3(0.0f, 1.0f, 1.0f);	// Kd represents the diffuse reflectivity cofficient
	float3 ads;

	float3 n = normalize(input.Norm);	// The n component is self-explanatory, but represents the normal of the surface
	float3 s = normalize(lightSource - input.WPos);	// The s component represents the direction from the surface to light source in world coordinates
	float3 v = normalize(input.ViewPos).xyz;	// The v component represents the viewer position in world coordinates
	float3 r = reflect(-s.xyz, n);	// The r component represent the reflection of the light direction vector with the the normal n

	


	// We check if we have information in the .MTL file, if we dont we use hardcoded values.	
	if (Ka.x > 0.0f || Ka.y > 0.0f || Ka.z > 0.0f)
	{

		specularLight = Ks.xyz * pow(max(dot(r, v), 0.0f), shinyPower);

		ads = Ld2 * (Ka.xyz + specularLight);


		texColor = tex0.Sample(texSampler, input.Tex).xyz;

		color = texColor;

		return float4(ads, 1.0f) * float4(color, 1.0f) * shadowCheck;

	}
	else
	{
	
		diffuseLight = Kd2 * max(dot(s, n), 0.0f);

		specularLight = Ks2.xyz * pow(max(dot(r, v), 0.0f), shinyPower);

		ads = Ld2 * (Ka2 + diffuseLight + specularLight);

		return float4(ads, 1.0f) * shadowCheck;
	}


};