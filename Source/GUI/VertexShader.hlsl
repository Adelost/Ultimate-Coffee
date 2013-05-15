cbuffer cbPerObject
{
	float4x4 worldViewProj;
	float4x4 world;
};

// Set directional light each frame since the user might want to change direction or color

cbuffer cbPerFrame 
{
	float ambient;

	// Directional light
	float3 dlDirection;
	float3 dlColor;

	int drawDebug;
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
	float3 normal	: NORMAL;
};

PixelIn vertexMain( VertexIn vIn )
{
	PixelIn pIn;

	pIn.position	= mul(float4(vIn.position, 1), worldViewProj);
	pIn.color		= vIn.color;
	pIn.normal		= mul(vIn.normal, (float3x3)world);

	return pIn;
}

float4 pixelMain( PixelIn pIn ) : SV_TARGET
{
	if(drawDebug)
	{
		return float4(0.0, 1.0, 0.0, 1.0);
		//return float4(0.4, 0.6, 0.9, 1.0);
	}

	float3 lightDir = normalize(dlDirection);
	float3 normal = normalize(pIn.normal);

	float lightValue = max(dot(lightDir, normal), 0);
	float3 light = dlColor*lightValue;
	float3 ambientLight = float3(ambient, ambient, ambient);

	return saturate(pIn.color*float4(light + ambientLight, 1));
}