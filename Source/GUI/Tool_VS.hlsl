cbuffer perObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float4 Col	: COLOR;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 Col	: COLOR;
};

VertexOut VS( VertexIn vIn )
{
	VertexOut vOut;

	vOut.PosH	= mul(float4(vIn.PosL, 1), gWorldViewProj);
	vOut.Col	= vIn.Col;

	return vOut;
}