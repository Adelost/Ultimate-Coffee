cbuffer cb_test
{
	float4x4  gWorldViewProj;
};

Texture2D gTexture : register(t0);

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex  : TEXCOORD;
};
 
VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.Tex  = vin.Tex;
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return gTexture.Sample(samLinear, pin.Tex);
}

float4 PS(VertexOut pin, uniform int index) : SV_Target
{
	float4 c = gTexture.Sample(samLinear, pin.Tex).r;
	
	// draw as grayscale
	return float4(c.rrr, 1);
}