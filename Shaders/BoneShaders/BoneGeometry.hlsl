//----------------------------------------------------------------------------------------------------------------------------------//
// Bone Geometry Shader DirectX11
//
// BTH - Fredrik Linde TA15 2016
//----------------------------------------------------------------------------------------------------------------------------------//

// The registers are underlying hardware registers on the GPU where all data is stored during execution of the shaders
// There are different types of register for different types of data
// - Buffers uses "b"

cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix lightViewProj;
	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix floorRot;
	float3 cameraPos;
	matrix lightViewProj;

};

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD;
	float3 Norm : NORMAL;

};

struct GS_OUT
{
	float4 Norm: NORMAL;
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION;
	float3 ViewPos : POSITION1;
	
};

// The geometry shader takes an entire primitive as an array as its input, but it also require an upper limit for the amount of 
// vertices to output. Therefore the program runs for every primitive. Because we are going to output two triangles, the total 
// vertex count must be a total of 6

[maxvertexcount(3)]
 void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> triStream)
{	 
	 GS_OUT output;

	 float3 normal, viewVector;

	 normal = (input[0].Norm);
	 normal = normalize(mul(float4(normal, 1.0f), matrixView).xyz);

	 viewVector = normalize(mul(float4(input[0].Pos, 1.0f), matrixView).xyz);

	 // UINT is an unsigned INT. The range is 0 through 4294967295 decimals
	 uint i;
	 
	 for (i = 0; i < 3; i++) {

		if(dot(normal, viewVector) < 1.0f){

			 // To store and calculate the World position for output to the pixel shader, the input position must be multiplied with the World matrix
			 float3 worldPosition = mul(float4(input[i].Pos, 1.0f), matrixWorld).xyz;
			 output.WPos = worldPosition;

			 // To store and calculate the WorldViewProj, the input position must be multiplied with the WorldViewProj matrix

			 output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), worldViewProj);

			 // For the normal to properly work and to later be used correctly when creating the basic diffuse shading, it's required to be computed in world coordinates

			 output.Norm = float4(mul(input[i].Norm, (float3x3)matrixWorld), 1.0f);

			 output.Tex = input[i].Tex;

			 output.ViewPos = cameraPos - worldPosition;

		}

		 triStream.Append(output);	// The output stream can be seen as list which adds the most recent vertex to the last position in that list

	 }

};