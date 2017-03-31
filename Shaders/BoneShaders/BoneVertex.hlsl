//----------------------------------------------------------------------------------------------------------------------------------//
// Bone Vertex Shader DirectX11
//
// Fredrik Linde TA15
//----------------------------------------------------------------------------------------------------------------------------------//

// The vertex shader is now only responsible for passing data and doesn't manipulate it any further

cbuffer VS_SKINNED_DATA: register (b0) {

	float4x4 gBoneTransforms[16];
};

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;
	float4 Weight : BLENDWEIGHT;
	uint4 BoneIndices : BLENDINDICES;
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

	for (int i = 0; i < 4 ; ++i) // loop trough the 4 weights
	{

		position += input.Weight[i] * mul(float4(input.Pos, 1.0f), gBoneTransforms[input.BoneIndices[i]]).xyz; // the vertex position is affected by the joint movement and influenced by the corresponding weight

		normal += input.Weight[i] * mul(input.Norm, (float3x3)gBoneTransforms[input.BoneIndices[i]]); // we make it 3x3 matrix to skip homogenus and take away translation.
	}
	
	output.Pos = position;		// Switch to Input position to temporarily disable animation update

	output.Tex = input.Tex;

	output.Norm = normal;

	return output;
}


