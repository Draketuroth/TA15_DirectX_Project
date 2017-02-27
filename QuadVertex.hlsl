struct VS_INPUT
{
	float3 Pos : POSITION;

};
struct VS_OUT
{
	float4 Pos : SV_POSITION;
};

VS_OUT VS_main(VS_INPUT input) : SV_POSITION
{
	VS_OUT output = (VS_OUT)0;

return output;
}