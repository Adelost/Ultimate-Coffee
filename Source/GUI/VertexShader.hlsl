// Remember to change padding if changed
#define NUMBER_OF_POINTLIGHTS 4

cbuffer cbPerObject : register(b0)
{
	float4x4 worldViewProj;
	float4x4 world;
	float4 g_color;
};

Texture2D tex_test : register(t0);

SamplerState samTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


cbuffer cbPerFrame : register(b1)
{
	// Directional light
	float4 dlDirectionAndAmbient; // xyz = Direction, w = Ambient 
	float4 dlColor;

	// Point light
	float4 plPosition[NUMBER_OF_POINTLIGHTS];
	float4 plColorAndRange[NUMBER_OF_POINTLIGHTS]; // xyz = Color, w = Range

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
	float4 worldPos : POSITION;
	float4 color	: COLOR;
	float3 normal	: NORMAL;
};

PixelIn vertexMain( VertexIn vIn )
{
	PixelIn o;

	o.position	= mul(float4(vIn.position, 1), worldViewProj);
	o.color		= vIn.color * g_color;
	o.worldPos	= mul(float4(vIn.position, 1), world);
	o.normal	= mul(vIn.normal, (float3x3)world);

	return o;
}

float4 pixelMain( PixelIn pIn ) : SV_TARGET
{
	if(drawDebug)
	{
		return float4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	float3 lightDir = normalize(dlDirectionAndAmbient.xyz);
	float ambient = dlDirectionAndAmbient.w;	float3 normal = normalize(pIn.normal);

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
		float range = plColorAndRange[i].w;
		float lightDistance = length(lightDir);
		float fallOff = min(range/(lightDistance), 1);
		
		// Calculate lighting color
		lightValue = max(dot(normalize(lightDir), normal), 0);

		// Add to final lighting
		light += plColorAndRange[i].xyz*lightValue*fallOff;
	}


	


	return pIn.color*float4(light + ambientLight, 1);

	//return tex_test.Sample(samTriLinearSam, float2(pIn.worldPos.x/100, pIn.worldPos.y/100));;
}






SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};


cbuffer cbOculus : register(b2)
{
	float4x4 g_world;
	float2 LensCenter;
	float2 ScreenCenter;
	float2 Scale;
	float2 ScaleIn;
	float4 HmdWarpParam;
	bool bStereo;
};

struct QVertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
};

struct QVertexOut
{
	float4 PosH : SV_POSITION;
	float2 Tex  : TEXCOORD;
};
 
QVertexOut VS(QVertexIn vin)
{
	QVertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), g_world);
	vout.Tex  = vin.Tex;
	
	return vout;
}


float2 HmdWarp(float2 in01)
{
	float2 theta = (in01 - LensCenter) * ScaleIn; // Scales to [-1, 1]
	float rSq = theta.x * theta.x + theta.y * theta.y;
	float2 rvector= theta * (HmdWarpParam.x + HmdWarpParam.y * rSq +
	HmdWarpParam.z * rSq * rSq +
	HmdWarpParam.w * rSq * rSq * rSq);

	return LensCenter + Scale * rvector;
}

float4 PS(QVertexOut pin) : SV_Target
{
	if(bStereo)
	{
		float2 oTexCoord = pin.Tex;
		
		// HACK: Right viewport is rendered first
		// then Left is rendered at half x-scale to 
		// prevent deept buffer overwrite. A very 
		// ugly solution but to tired to fix.
		if(ScreenCenter.x < 0.5)
		{
			oTexCoord.x /= 2;
		}


		float2 tc = HmdWarp(oTexCoord);
		if(any(clamp(tc, ScreenCenter-float2(0.25, 0.5), ScreenCenter+float2(0.25, 0.5)) - tc))
			return 0;

		return tex_test.Sample(samLinear, tc);
	}

	return tex_test.Sample(samLinear, pin.Tex);
}