//----------------------------------------------------------------------------------------------------------------------------------//
// Bone Geometry Shader DirectX11
//
// Fredrik Linde TA15
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
	matrix matrixViewInverse;
	float4 cameraPos;
	float4 cameraUp;
	matrix worldInvTranspose;
	float normalMappingFlag;
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

	 // Calculate the normal to determine the direction for the new triangle to be created ( closer to the camera )

	 float4 position = mul(float4(input[0].Pos, 1.0f), worldViewProj);
	 float4 position2 = mul(float4(input[1].Pos, 1.0f), worldViewProj);
	 float4 position3 = mul(float4(input[2].Pos, 1.0f), worldViewProj);

	 float3 triangleSideA = position - position2;
	 float3 triangleSideB = position - position3;

	 normal = normalize(cross(triangleSideA, triangleSideB));

	 // UINT is an unsigned INT. The range is 0 through 4294967295 decimals
	 uint i;

	 for (i = 0; i < 3; i++) {

		 float3 worldPosition = mul(float4(input[i].Pos, 1.0f), matrixWorld).xyz;

		 if (dot(normal, -position) > 0.0f) {

			 // To store and calculate the World position for output to the pixel shader, the input position must be multiplied with the World matrix
			 output.WPos = worldPosition;

			 // To store and calculate the WorldViewProj, the input position must be multiplied with the WorldViewProj matrix

			 output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), worldViewProj);

			 // For the normal to properly work and to later be used correctly when creating the basic diffuse shading, it's required to be computed in world coordinates

			 output.Norm = mul(float4(input[i].Norm, 1.0f), matrixWorld);

			 output.Tex = input[i].Tex;

		 }

		 output.ViewPos = cameraPos - worldPosition;

		 triStream.Append(output);	// The output stream can be seen as list which adds the most recent vertex to the last position in that list
	 }

};