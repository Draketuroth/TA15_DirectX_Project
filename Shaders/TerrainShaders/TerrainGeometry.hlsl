
cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix floorRot;
	float3 cameraPos;

};

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;

};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float4 WPos : WPOSITION;
	float2 Tex : TEXCOORD0;
	float4 Norm: NORMAL;

};

// The geometry shader takes an entire primitive as an array as its input, but it also require an upper limit for the amount of 
// vertices to output. Therefore the program runs for every primitive. Because we are going to output two triangles, the total 
// vertex count must be a total of 6

[maxvertexcount(3)]
void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> triStream){
	
	GS_OUT output;

	float3 sideA = input[1].Pos.xyz - input[0].Pos.xyz;
	float3 sideB = input[2].Pos.xyz - input[0].Pos.xyz;

	// Calculate the normal to determine the direction for the new triangle to be created ( closer to the camera )

	float3 normalAB = normalize(cross(sideA, sideB));

	// UINT is an unsigned INT. The range is 0 through 4294967295 decimals
	uint i;

	for (i = 0; i < 3; i++) {

		// To store and calculate the World position for output to the pixel shader, the input position must be multiplied with the World matrix

		output.WPos = mul(float4(input[i].Pos.xyz, 1.0f), matrixWorld);

		// To store and calculate the WorldViewProj, the input position must be multiplied with the WorldViewProj matrix

		output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), worldViewProj);

		// For the normal to properly work and to later be used correctly when creating the basic diffuse shading, it's required to be computed in world coordinates

		output.Norm = mul(float4(normalAB, 1.0f), matrixWorld);

		output.Tex = input[i].Tex;

	}

		triStream.Append(output);	// The output stream can be seen as list which adds the most recent vertex to the last position in that list

};