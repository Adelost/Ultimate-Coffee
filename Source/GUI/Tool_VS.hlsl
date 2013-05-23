cbuffer perObject : register(b0)
{
	float4x4 gWorldViewProj;
};

cbuffer perFrame
{
	uint gColorSchemeId;
	uint pad1, pad2, pad3;
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
	
	if(gColorSchemeId == 0) // XYZ = RGB.
	{
		if(vIn.Col.r == 1.0f && vIn.Col.g == 0.0f && vIn.Col.b == 1.0f)
		{
			vOut.Col = float4(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else if(vIn.Col.r == 1.0f && vIn.Col.g == 1.0f && vIn.Col.b == 0.0f)
		{
			vOut.Col = float4(0.0f, 1.0f, 0.0f, 1.0f);
		}
		else if(vIn.Col.r == 0.0f && vIn.Col.g == 1.0f && vIn.Col.b == 1.0f)
		{
			vOut.Col = float4(0.0f, 0.0f, 1.0f, 1.0f);
		}
		else if(vIn.Col.r == 1.0f && vIn.Col.g == 0.5f && vIn.Col.b == 0.5f)
		{
			vOut.Col = float4(0.5f, 0.5f, 0.0f, 1.0f);
		}
		else if(vIn.Col.r == 0.5f && vIn.Col.g == 0.5f && vIn.Col.b == 1.0f)
		{
			vOut.Col = float4(0.0f, 0.5f, 0.5f, 1.0f);
		}
		else if(vIn.Col.r == 0.5f && vIn.Col.g == 1.0f && vIn.Col.b == 0.5f)
		{
			vOut.Col = float4(0.5f, 0.9f, 0.5f, 1.0f);
		}
		else
			vOut.Col = vIn.Col;
	}
	else if(gColorSchemeId == 1) // XYZ = MYC.
	{
		vOut.Col = vIn.Col;
	}

	return vOut;
}