//----------------------------------------------------------------------------------------------------------------------------------//
// Compute Shader Horizontal Blur (FIRST PASS) DirectX11
//
// Fredrik Linde TA15
//----------------------------------------------------------------------------------------------------------------------------------//

cbuffer cbGaussian
{
	static const float gWeights[11] =
	{
		0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
	};
};

cbuffer cbImmutable
{
	static const int gBlurRadius = 5;

};

Texture2D InputTex : register (t0);
RWTexture2D<float4> OutputTex : register (u0);

#define NUMBER_OF_THREADS_X 256
#define CACHE_SIZE_X (NUMBER_OF_THREADS_X + 2 * gBlurRadius)
groupshared float4 gCacheX[CACHE_SIZE_X];

[numthreads(NUMBER_OF_THREADS_X, 1, 1)]
void CS_main(int3 groupThreadID : SV_GroupThreadID, int3 dispatchThreadID : SV_DispatchThreadID)
{

	// This thread group runs NUMBER_OF_THREADS threads. To get the extra 2 * BlurRadius pixels, 
	// have 2 * BlurRadius threads sample an extra pixel.
	if (groupThreadID.x < gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int x = max(dispatchThreadID.x - gBlurRadius, 0);
		gCacheX[groupThreadID.x] = InputTex[int2(x, dispatchThreadID.y)];
	}

	if (groupThreadID.x >= NUMBER_OF_THREADS_X - gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int x = min(dispatchThreadID.x + gBlurRadius, InputTex.Length.x - 1);
		gCacheX[groupThreadID.x + 2 * gBlurRadius] = InputTex[int2(x, dispatchThreadID.y)];
	}

	// Clamp out of bound samples that occur at image borders.
	gCacheX[groupThreadID.x + gBlurRadius] = InputTex[min(dispatchThreadID.xy, InputTex.Length.xy - 1)];

	// Wait for all threads to finish.
	GroupMemoryBarrierWithGroupSync();

	// Each pixel can now be blurred

	float4 blurColor = float4(0, 0, 0, 0);

	[unroll]
	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.x + gBlurRadius + i;

		blurColor += gWeights[i + gBlurRadius] * gCacheX[k];
	}

	OutputTex[dispatchThreadID.xy] = blurColor;


}
