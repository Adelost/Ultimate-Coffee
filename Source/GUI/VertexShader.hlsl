// Remember to change padding if changed
#define NUMBER_OF_POINTLIGHTS 4

cbuffer cbPerObject
{
	float4x4 worldViewProj;
	float4x4 world;
};

// Set directional light each frame since the user might want to change direction or color

cbuffer cbPerFrame 
{
	// Directional light
	float4 dlDirectionAndAmbient; // xyz = Direction, w = Ambient 
	float4 dlColor;

	// Point light
	float4 plPosition[NUMBER_OF_POINTLIGHTS];
	float4 plColor[NUMBER_OF_POINTLIGHTS];
	float plRange[NUMBER_OF_POINTLIGHTS];
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
	float4 worldPos : POSITION;
	float4 color	: COLOR;
	float3 normal	: NORMAL;
};

PixelIn vertexMain( VertexIn vIn )
{
	PixelIn pIn;

	pIn.position	= mul(float4(vIn.position, 1), worldViewProj);
	pIn.color		= vIn.color;
	pIn.worldPos	= mul(float4(vIn.position, 1), world);
	pIn.normal		= mul(vIn.normal, (float3x3)world);

	return pIn;
}

float4 pixelMain( PixelIn pIn ) : SV_TARGET
{
	float3 lightDir = normalize(dlDirectionAndAmbient.xyz);
	float ambient = dlDirectionAndAmbient.w;
	float3 normal = normalize(pIn.normal);

	// Calculate directional light
	float lightValue = max(dot(lightDir, normal), 0);
	float3 light = dlColor.xyz*lightValue;

	float3 ambientLight = float3(ambient, ambient, ambient);

	// Calculate point lights
	for(unsigned int i = 0; i < NUMBER_OF_POINTLIGHTS; i++)
	{
		// Calculate light direction
		lightDir = plPosition[i].xyz - pIn.worldPos.xyz;

		// Calculate lighting power
		float lightDistance = length(lightDir);
		float fallOff = min(plRange[i]/(lightDistance), 1);
		
		// Calculate lighting color
		lightValue = max(dot(normalize(lightDir), normal), 0);

		// Add to final lighting
		light += plColor[i].xyz*lightValue*fallOff;
	}

	return pIn.color*float4(light + ambientLight, 1);
}