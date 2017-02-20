
Texture2D<float> InputTex : register(t0);
RWTexture2D<float> OutputTex : register(u0);

[numthreads(16, 16, 1)]
void CS_main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
	int3 textureLocation = int3(0, 0, 0);
	textureLocation.x = DispatchThreadID.x / (512.0f - 1.0f);
	textureLocation.y = DispatchThreadID.y / (512.0f - 1.0f);

	float value = InputTex.Load(textureLocation);

	OutputTex[DispatchThreadID.xy] = 7.0f * value;
}