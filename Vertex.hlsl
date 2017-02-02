//----------------------------------------------------------------------------------------------------------------------------------//
// Vertex Shader DirectX11
//
// Philip Velandria, Jonathan Sundberg, Linnea Vajda, Fredrik Linde
//----------------------------------------------------------------------------------------------------------------------------------//

// The vertex shader is now only responsible for passing data and doesn't manipulate it any further

struct VS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};

struct VS_OUT
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = input.Pos;

	output.Tex = input.Tex;

	return output;
}