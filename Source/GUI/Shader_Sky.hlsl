cbuffer cb_test
{
	float4x4 worldViewProj;
	float4x4 world;
	float4 g_color;
};

TextureCube cubeMap : register(t0);

SamplerState samTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VertexIn
{
	float3 posL  : POSITION;
};

struct PixelIn
{
	float4 posH  : SV_POSITION;
	float4 color	: COLOR;
	float3 posL : POSITION;
};

PixelIn VS( VertexIn v_in )
{
	PixelIn p_in;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
	p_in.posH = mul(float4(v_in.posL, 1.0f), worldViewProj).xyww;
	p_in.color = g_color;

	// Use local vertex position as cubemap lookup vector.
	p_in.posL = v_in.posL;

	return p_in;
}

float4 PS( PixelIn p_in  ) : SV_TARGET
{
	return cubeMap.Sample(samTriLinearSam, p_in.posL ) * p_in.color;
}