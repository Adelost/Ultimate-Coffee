cbuffer cbPerObject
{
	float4x4 worldViewProj;
};

struct VertexIn
{
	float3 position : POSITION;
	float4 color	: COLOR;
	float3 normal	: NORMAL;
};

struct PixelIn
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float4 normal	: NORMAL;
};

PixelIn vertexMain( VertexIn vIn )
{
	PixelIn pIn;

	pIn.position	= mul(float4(vIn.position, 1), worldViewProj);
	pIn.color		= vIn.color;
	pIn.normal		= mul(float4(vIn.normal, 0), worldViewProj);

	return pIn;
}

float4 pixelMain( PixelIn pIn ) : SV_TARGET
{
	float4 lightDir = float4( 1.0, 1.0, 1.0, 0.0);
	lightDir = normalize(lightDir);
	float4 normal = normalize(pIn.normal);

	float lightValue = max(dot(lightDir, normal), 0);

	return pIn.color*lightValue + pIn.color*0.1;
}