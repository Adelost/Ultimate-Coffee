cbuffer cbPerObject
{
	float4x4 worldViewProj;
};

struct VertexIn
{
	float3 position : POSITION;
	float4 color	: COLOR;
};

struct PixelIn
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
};

PixelIn vertexMain( VertexIn vIn )
{
	PixelIn pIn;

	pIn.position	= /*mul(*/float4(vIn.position, 1)/*, worldViewProj)*/;
	pIn.color		= vIn.color;

	return pIn;
}

float4 pixelMain( PixelIn pIn ) : SV_TARGET
{
	return pIn.color;
}