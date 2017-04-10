//----------------------------------------------------------------------------------------------------------------------------------//
// Vertex Shader DirectX11
//
// Jonathan Sundberg TA15
//----------------------------------------------------------------------------------------------------------------------------------//




// constant buffern för particle rörelser
cbuffer VS_CONSTANT_BUFFER : register(b0) {

	float4 particleMovement[1000];

	
};

struct VS_IN
{
	float3 Pos : POSITION;
	
};

struct VS_OUT
{
	float3 Pos : POSITION;
	
};
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input, uint id : SV_VertexID)
{
	VS_OUT output = (VS_OUT)0;

	
	input.Pos += particleMovement[id].xyz; // vi tilldelar våra partikelpunkter våran partikelrörelse
	output.Pos = input.Pos;

	

	return output;
	// fortsätt till geo shader.
}