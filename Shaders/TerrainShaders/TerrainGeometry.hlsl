//----------------------------------------------------------------------------------------------------------------------------------//
// Terrain Geometry Shader DirectX11
//
// Linnea Vajda TA15
//----------------------------------------------------------------------------------------------------------------------------------//

cbuffer GS_CONSTANT_BUFFER : register(b0) {

	matrix lightViewProj;
	matrix worldViewProj;
	matrix matrixWorld;
	matrix matrixView;
	matrix matrixProjection;
	matrix viewProj;
	matrix floorRot;
	matrix matrixViewInverse;
	float4 cameraPos;
	float4 cameraUp;
	matrix worldInvTranspose;
};

cbuffer TOPDOWN_CAMERA : register(b1) {

	matrix topDownViewTransform;
	matrix projectionInverse;
};

struct GS_IN
{
	float3 Pos : POSITION;
	float2 Tex : TEXCOORD0;
	float3 Norm : NORMAL;

};

struct GS_OUT
{
	float3 Norm: NORMAL;
	float2 Tex : TEXCOORD;
	float4 Pos : SV_POSITION;
	float3 WPos : POSITION;
	float3 ViewPos : POSITION1;

	float4 lPos : TEXCOORD1;

};

// The geometry shader takes an entire primitive as an array as its input, but it also require an upper limit for the amount of 
// vertices to output. Therefore the program runs for every primitive. Because we are going to output two triangles, the total 
// vertex count must be a total of 6

[maxvertexcount(3)]
void GS_main(triangle GS_IN input[3], inout TriangleStream<GS_OUT> triStream){
	
	GS_OUT output;

	float3 normal, viewVector, Wnormal;

	// Calculate the normal to determine the direction for the new triangle to be created ( closer to the camera )
	// THIS SECTION IS FOR BACKFACE CULLING
	float3 Wposition = mul(float4(input[0].Pos, 1.0f), worldViewProj).xyz;
	float3 Wposition2 = mul(float4(input[1].Pos, 1.0f), worldViewProj).xyz;
	float3 Wposition3 = mul(float4(input[2].Pos, 1.0f), worldViewProj).xyz;

	float3 WtriangleSideA = (Wposition - Wposition2).xyz;
	float3 WtriangleSideB = (Wposition - Wposition3).xyz;

	Wnormal = normalize(cross(WtriangleSideA, WtriangleSideB));

	//THIS SECTION IS FOR LIGHTNING NORMALS
	float3 position = input[0].Pos;
	float3 position2 = input[1].Pos;
	float3 position3 = input[2].Pos;

	float3 triangleSideA = (position - position2).xyz;
	float3 triangleSideB = (position - position3).xyz;

	normal = normalize(cross(triangleSideA, triangleSideB));
	// UINT is an unsigned INT. The range is 0 through 4294967295 decimals
	uint i;

	if (dot(Wnormal.xyz, -Wposition.xyz) > 0.0f) {

		for (i = 0; i < 3; i++) {

			float3 worldPosition = mul(float4(input[i].Pos, 1.0f), matrixWorld).xyz;

			// To store and calculate the World position for output to the pixel shader, the input position must be multiplied with the World matrix
			output.WPos = worldPosition;

			// To store and calculate the WorldViewProj, the input position must be multiplied with the WorldViewProj matrix
			matrix WVP = mul(topDownViewTransform, matrixProjection);
			WVP = mul(WVP, matrixWorld);
			output.Pos = mul(float4(input[i].Pos.xyz, 1.0f), WVP);

			output.lPos = mul(float4(input[i].Pos.xyz, 1.0f), mul(matrixWorld, lightViewProj));

			// For the normal to properly work and to later be used correctly when creating the basic diffuse shading, it's required to be computed in world coordinates

			

			

			output.Norm = mul(float4(normal, 1.0f), (float3x3)matrixWorld);

			output.Tex = input[i].Tex;

			output.ViewPos = cameraPos.xyz - worldPosition.xyz;

			triStream.Append(output);	// The output stream can be seen as list which adds the most recent vertex to the last position in that list

		}

	}

};