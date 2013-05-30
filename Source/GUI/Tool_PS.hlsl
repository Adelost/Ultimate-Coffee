cbuffer perFrame
{
	float3 g_camViewVectorW;
	float3 g_pivotPoint;
	uint pad1, pad2, pad3;
}

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float4 Col	   : COLOR;
};

float4 PS(VertexOut pin) : SV_Target
{
	float3 norm = normalize(g_camViewVectorW);
	float3 invCamLookAt = -norm;
	float3 lightFixturePos = g_pivotPoint + invCamLookAt;
	float dist = length(lightFixturePos - pin.PosH);

	float4 litColor = pin.Col;
	//if(dist > 0.00005f)
	// litColor = float4(0.0f, 1.0f, 0.0f, 0.5f); //pin.Col * (dist * 0.005f); //float4(g_camViewVectorW, 1.0f); // (litColor.xyz, 0.25f);
	//else
	//	litColor = float4(1.0f, 0.0f, 0.0f, 0.5f);


	//	// Calculate light direction
	//	//lightDir = plPosition[i].xyz - pIn.worldPos.xyz;

	//	// Calculate lighting power
	//	//float range = plColorAndRange[i].w;
	//	//float lightDistance = length(lightDir);
	//	float fallOff = min(1.0f/(dist), 1);
	//	
	//	// Calculate lighting color
	//	lightValue = max(dot(normalize(norm), float3(1.0f, 0.0f, 0.0f)), 0);


	//return pin.color*float4(litColor*fallOff + ambientLight, 1);

   return litColor;
}