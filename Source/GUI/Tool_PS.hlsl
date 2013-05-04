cbuffer perObject
{
	bool isSelected;
}

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float4 Col	   : COLOR;
};

float4 PS(VertexOut pin) : SV_Target
{
	float4 litColor = pin.Col;

	if(isSelected)
		litColor = float4(0.0f, 1.0f, 0.0f, 1.0f);

    return litColor;
}