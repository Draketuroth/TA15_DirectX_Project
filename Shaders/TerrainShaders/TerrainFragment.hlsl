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
	float3 Kd;
	float3 Ka;
	float3 tf;
	float3 Ks;
	float ni;
	float illum;
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
	bool Mcolor = false;

	//normalMap
	//float3 normalMapSampel = gNormalMap.Sampel(samLinear, pin.Tex).rgb; 
	//float3 bumpedNormalW = NormalToWorldSpace(normalMapSample, pin.NormalW, pin.TangentW); 


	input.lPos.xy /= input.lPos.w; //light pos in NDC

	//getting the light pos from [-1, 1] to [0, 1]
	float2 smTexture = float2(0.5f * input.lPos.x + 0.5f, -0.5f * input.lPos.y + 0.5f);

	//pixel depth for shadows
	float depth = input.lPos.z / input.lPos.w;
	
	float shadowCheck = (shadowMap.Sample(texSampler, smTexture).r + 0.0001f < depth) ? 0.25f : 1.0f;

	float nDotL;
	float3 texColor;
	float3 color;

	float shinyPower = 20.0f;

	float3 Ld2 = float3(0.6f, 0.6f, 0.6f);	// Ld represents the light source intensity
	float3 Ka2 = float3(0.2f, 0.2f, 0.2f);		// Ka is the hardcoded ambient light
	float3 Ks2 = float3(0.0f, 0.0f, 0.0f);	// Ks is the hardcoded specular light
	float3 Kd2 = float3(1.0f, 1.0f, 1.0f);	// Kd represents the diffuse reflectivity cofficient
	float3 ads;

	float3 n = normalize(input.Norm);	// The n component is self-explanatory, but represents the normal of the surface
	float3 s = normalize(lightSource - input.WPos);	// The s component represents the direction from the surface to light source in world coordinates
	float3 v = normalize(input.ViewPos).xyz;	// The v component represents the viewer position in world coordinates
	float3 r = reflect(-s.xyz, n);	// The r component represent the reflection of the light direction vector with the the normal n

	if ( Kd.x > 0.0f || Kd.y > 0.0f || Kd.z > 0.0f)
	{
		diffuseLight = Kd * max(dot(s, n), 0.0f);

		specularLight = Ks * pow(max(dot(r, v), 0.0f), shinyPower);

		ads = Ld2 * (Ka + diffuseLight + specularLight);
		color = float3(Kd.x,Kd.y,Kd.z);
		Mcolor = true;

	}
	else
	{
		diffuseLight = Kd2 * max(dot(s, n), 0.0f);

		specularLight = Ks2 * pow(max(dot(r, v), 0.0f), shinyPower);

		ads = Ld2 * (Ka2 + diffuseLight + specularLight);
	}
	

	// Now the Sample state will sample the color output from the texture file so that we can return the correct color
	if (Mcolor == false)
	{
		texColor = tex0.Sample(texSampler, input.Tex).xyz;

		color = float4(texColor, 1.0f);
	}
	

	//return float4(color,1);// *shadowCheck;

	return float4((ads, 1.0f) *color,1) *shadowCheck;
	//return float4(texColor, 1) * shadowCheck;
};

//float3 NormalToWorldSpace(float3 normalMapSample, float3 uintNormalW, float3 tangentW)
//{
//	//Uncompress each component from [0,1] to [-1,1]
//	float3 normalT = 2.0f*normalMapSample - 1.0f; 
//
//	//build orthonomal basis
//	float3 N = uintNormalW; 
//	float3 T = normalize(tangentW - dot(tangentW, N)*N); 
//	float3 B = cross(N, T); 
//
//	float3x3 TBN = float3x3(T, B, N); 
//	
//	//transform from target space to world space
//	float3 bumpedNormalW = mul(normalT, TBN); 
//
//	return bumpedNormalW; 
//}