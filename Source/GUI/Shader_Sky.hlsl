cbuffer cb_test
{
	float4x4 world;
};

struct VertexIn
{
	float3 position : POSITION;
};

struct PixelIn
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
};

PixelIn VS( VertexIn vIn )
{
	PixelIn pIn;

	float4x4 m = float4x4(	1,0,0,0,
							0,1,0,0,
							0,0,1,0,
							0,0,0,1);

	pIn.position	= mul(float4(vIn.position, 1), world);
	pIn.color		= float4(1,0,0,1);

	return pIn;
}

float4 PS( PixelIn pIn ) : SV_TARGET
{
	return pIn.color;
}