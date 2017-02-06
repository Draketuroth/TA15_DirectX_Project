//----------------------------------------------------------------------------------------------------------------------------------//
// Bone Vertex Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

// The vertex shader is now only responsible for passing data and doesn't manipulate it any further

cbuffer VS_SKINNED_DATA: register (b0) {

	float4x4 gBoneTransforms[4];
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float4 Weight : WEIGHT;
	uint4 BoneIndices : BONEINDICES;
};

struct VS_OUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	// Perform vertex blending

	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 normal = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4 ; ++i) {

		position += input.Weight[i] * mul(float4(input.Pos, 1.0f), gBoneTransforms[input.BoneIndices[i]]).xyz;

		normal += input.Weight[i] * mul(input.Norm, (float3x3)gBoneTransforms[input.BoneIndices[i]]);
	}
	
	output.Pos = position;		// Switch to Input position to temporarily disable animation update

	output.Tex = input.Tex;

	output.Norm = normal;

	return output;
}


