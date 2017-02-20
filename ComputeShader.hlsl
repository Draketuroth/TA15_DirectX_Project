
Texture2D <float4> txFrame : register(t0);
RWStructuredBuffer<float4> rwbuffer : register(u0);

[numthreads(16, 16, 1)]
void CS_main(uint3 i : SV_DispatchThreadID)
{
	//Read color and write to buffer

	uint bufp = i.x + i.y;

	uint3 indx = uint3(uint(bufp % 512), uint(bufp / 512), 0);

	rwbuffer[bufp] = txFrame.Load(indx);

}