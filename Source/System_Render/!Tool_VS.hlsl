cbuffer perObject
{
	float4x4 gWorldViewProj;
}:

struct VertexIn
{
	float3 PosL    : POSITION;
	float4 Col	   : COLOR;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float4 Col	   : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across geometry.
	vout.Col = vin.Col;

	return vout;
}