//----------------------------------------------------------------------------------------------------------------------------------//
// Compute Shader Horizontal + Vertical Blur (SECOND PASS) DirectX11
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

#define NUMBER_OF_THREADS_Y 24
#define CACHE_SIZE_Y (NUMBER_OF_THREADS_Y + 2 * gBlurRadius)
groupshared float4 gCacheY[CACHE_SIZE_Y];

[numthreads(1, NUMBER_OF_THREADS_Y, 1)]
void CS_main(int3 groupThreadID : SV_GroupThreadID, int3 dispatchThreadID : SV_DispatchThreadID)
{
	
	// This thread group runs NUMBER_OF_THREADS threads.  To get the extra 2 * BlurRadius pixels, 
	// have 2 * BlurRadius threads sample an extra pixel.
	if(groupThreadID.y < gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = max(dispatchThreadID.y - gBlurRadius, 0);
		gCacheY[groupThreadID.y] = InputTex[int2(dispatchThreadID.x, y)];
	}

	if(groupThreadID.y >= NUMBER_OF_THREADS_Y - gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = min(dispatchThreadID.y + gBlurRadius, InputTex.Length.y - 1);
		gCacheY[groupThreadID.y + 2 * gBlurRadius] = InputTex[int2(dispatchThreadID.x, y)];
	}
	
	// Clamp out of bound samples that occur at image borders.
	gCacheY[groupThreadID.y + gBlurRadius] = InputTex[min(dispatchThreadID.xy, InputTex.Length.xy - 1)];


	// Wait for all threads to finish.
	GroupMemoryBarrierWithGroupSync();
	
	// Now each pixel can be blurred

	float4 blurColor = float4(0, 0, 0, 0);
	
	[unroll]
	for(int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.y + gBlurRadius + i;
		
		blurColor += gWeights[i + gBlurRadius] * gCacheY[k];
	}
	
	OutputTex[dispatchThreadID.xy] = blurColor;
	
	}