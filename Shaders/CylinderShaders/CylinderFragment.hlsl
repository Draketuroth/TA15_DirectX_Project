//----------------------------------------------------------------------------------------------------------------------------------//
// Cylinder Fragment Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

SamplerState texSampler: register(s1);
Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);

float3 NormalToWorldSpace(float3 normalMapSample, float3 uintNormalW, float3 tangentW)
{
	//Uncompress each component from [0,1] to [-1,1]
	float3 normalT = 2.0f*normalMapSample - 1.0f; 

	//build orthonomal basis
	float3 N = uintNormalW; 
	float3 T = normalize(tangentW - dot(tangentW, N)*N); 
	float3 B = cross(N, T); 

	float3x3 TBN = float3x3(T, B, N); 
	
	//transform from target space to world space
	float3 bumpedNormalW = mul(normalT, TBN); 

	return bumpedNormalW; 
}

void ComputeDirectionalLight(float3 normal, float3 toEye,
	out float4 ambient,
	out float4 diffuse,
	out float4 spec)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Set material attributes

	float4 matAmbient = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 matDiffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 matSpecular = float4(0.8f, 0.8f, 0.8f, 16.0f);
	float4 matReflect = float4(0.0f, 0.0f, 0.0f, 1.0f);

	// Set directional light attributes

	float4 LAmbient = float4(0.2f, 0.2f, 0.2f, 1.0f);
	float4 LDiffuse = float4(0.7f, 0.7f, 0.6f, 1.0f);
	float4 LSpecular = float4(0.8f, 0.8f, 0.7f, 1.0f);
	float3 LDirection = float3(0.707f, 0.0f, 0.707f);

	// The light vector aims opposite the direction the light rays travel.
	float3 lightVec = -LDirection;

	// Add ambient term.
	ambient = matAmbient * LAmbient;

	// Add diffuse and specular term, provided the surface is in 
	// the line of site of the light.

	float diffuseFactor = dot(lightVec, normal);

	// Flatten to avoid dynamic branching.
	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), matSpecular.w);

		diffuse = diffuseFactor * matDiffuse * LDiffuse;
		spec = specFactor * matSpecular * LSpecular;
	}
}


SamplerState samLinear : register (s0)
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct PS_IN
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex : TEXCOORD;
	float3 EyePosW : POSITION1;
	float normalFlag : POSITION2;
};


float4 PS_main(PS_IN input) : SV_Target
{

	input.NormalW = normalize(input.NormalW);

	float3 toEye = input.EyePosW - input.PosW;

	float3 distToEye = length(toEye);

	toEye /= distToEye;

	float4 texColor = float4(1, 1, 1, 1);

	texColor = gDiffuseMap.Sample(samLinear, input.Tex);

	float3 normalMapSample = gNormalMap.Sample(samLinear, input.Tex).rgb;
	float3 bumpedNormalW = NormalToWorldSpace(normalMapSample, input.NormalW, input.TangentW);

	float4 litColor = texColor;

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;
	ComputeDirectionalLight(bumpedNormalW, toEye, A, D, S);

	ambient += A;
	diffuse += D;
	spec += S;

	if(input.normalFlag == 1){

	return texColor*(ambient + diffuse) + spec;

	}

	else{

	return litColor;

	}

};

