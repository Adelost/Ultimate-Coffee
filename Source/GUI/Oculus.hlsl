// StdVertexShaderSrc
float4x4 Proj;
float4x4 View;
struct Varyings
{
	float4 Position : SV_Position;
	float4 Color    : COLOR0;
	float2 TexCoord : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
	float3 Normal   : NORMAL;
	float3 VPos     : TEXCOORD4;
};
void main(in float4 Position : POSITION, in float4 Color : COLOR0, in float2 TexCoord : TEXCOORD0, in float2 TexCoord1 : TEXCOORD1, in float3 Normal : NORMAL,
		  out Varyings ov)
{
	ov.Position = mul(Proj, mul(View, Position));
	ov.Normal = mul(View, Normal);
	ov.VPos = mul(View, Position);
	ov.TexCoord = TexCoord;
	ov.TexCoord1 = TexCoord1;
	ov.Color = Color;
};




// DirectVertexShaderSrc
float4x4 View : register(c4);
void main(in float4 Position : POSITION, in float4 Color : COLOR0, in float2 TexCoord : TEXCOORD0, in float2 TexCoord1 : TEXCOORD1, in float3 Normal : NORMAL,
		  out float4 oPosition : SV_Position, out float4 oColor : COLOR, out float2 oTexCoord : TEXCOORD0, out float2 oTexCoord1 : TEXCOORD1, out float3 oNormal : NORMAL)
{
	oPosition = mul(View, Position);
	oTexCoord = TexCoord;
	oTexCoord1 = TexCoord1;
	oColor = Color;
	oNormal = mul(View, Normal);
};




// SolidPixelShaderSrc
float4 Color;
struct Varyings
{
	float4 Position : SV_Position;
	float4 Color    : COLOR0;
	float2 TexCoord : TEXCOORD0;
};
float4 main(in Varyings ov) : SV_Target
{
	return Color;
};

// GouraudPixelShaderSrc
struct Varyings
{
	float4 Position : SV_Position;
	float4 Color    : COLOR0;
	float2 TexCoord : TEXCOORD0;
};
float4 main(in Varyings ov) : SV_Target
{
	return ov.Color;
};

// TexturePixelShaderSrc
Texture2D Texture : register(t0);
SamplerState Linear : register(s0);
struct Varyings
{
	float4 Position : SV_Position;
	float4 Color    : COLOR0;
	float2 TexCoord : TEXCOORD0;
};
float4 main(in Varyings ov) : SV_Target
{
	float4 color2 = ov.Color * Texture.Sample(Linear, ov.TexCoord);
	if (color2.a <= 0.4)
		discard;
	return color2;
};

// MultiTexturePixelShaderSrc
Texture2D Texture[2] : register(t0);
SamplerState Linear : register(s0);
struct Varyings
{
	float4 Position : SV_Position;
	float4 Color    : COLOR0;
	float2 TexCoord : TEXCOORD0;
	float2 TexCoord1 : TEXCOORD1;
};
float4 main(in Varyings ov) : SV_Target
{
	float4 color1;
	float4 color2;
	color1 = Texture[0].Sample(Linear, ov.TexCoord);
	color2 = Texture[1].Sample(Linear, ov.TexCoord1);
	color2.rgb = color2.rgb * lerp(1.2, 1.9, saturate(length(color2.rgb)));
	color2 = color1 * color2;
	if (color2.a <= 0.4)
		discard;
	return color2;
};

// LIGHTING_COMMON
cbuffer Lighting : register(b1) 
{                               
	float3 Ambient;             
	float3 LightPos[8];         
	float4 LightColor[8];       
	float  LightCount;          
};                          
struct Varyings                 
{                                       
	float4 Position : SV_Position;       
	float4 Color    : COLOR0;            
	float2 TexCoord : TEXCOORD0;         
	float3 Normal   : NORMAL;            
	float3 VPos     : TEXCOORD4;         
};

// LIGHTING_COMMON
float4 DoLight(Varyings v)              
{                                       
	float3 norm = normalize(v.Normal);   
	float3 light = Ambient;              
	for (uint i = 0; i < LightCount; i++)
	{                                        
		float3 ltp = (LightPos[i] - v.VPos); 
		float  ldist = dot(ltp,ltp);         
		ltp = normalize(ltp);                
		light += saturate(LightColor[i] * v.Color.rgb * dot(norm, ltp) / sqrt(ldist));
	}                                        
	return float4(light, v.Color.a);         
}

// LitSolidPixelShaderSrc, LIGHTING_COMMON
float4 main(in Varyings ov) : SV_Target
{
	return DoLight(ov) * ov.Color;
};

// LitTexturePixelShaderSrc
Texture2D Texture : register(t0);
SamplerState Linear : register(s0);
LIGHTING_COMMON
	float4 main(in Varyings ov) : SV_Target
{
	return DoLight(ov) * Texture.Sample(Linear, ov.TexCoord);
};

// AlphaTexturePixelShaderSrc
Texture2D Texture : register(t0);
SamplerState Linear : register(s0);
struct Varyings
{
	float4 Position : SV_Position;
	float4 Color    : COLOR0;
	float2 TexCoord : TEXCOORD0;
};
float4 main(in Varyings ov) : SV_Target
{
	return ov.Color * float4(1,1,1,Texture.Sample(Linear, ov.TexCoord).r);
};

// PostProcessVertexShaderSrc
float4x4 View : register(c4);
float4x4 Texm : register(c8);
void main(in float4 Position : POSITION, in float4 Color : COLOR0, in float2 TexCoord : TEXCOORD0, in float2 TexCoord1 : TEXCOORD1,
		  out float4 oPosition : SV_Position, out float4 oColor : COLOR, out float2 oTexCoord : TEXCOORD0)
{
	oPosition = mul(View, Position);
	oTexCoord = mul(Texm, float4(TexCoord,0,1));
	oColor = Color;
};






// Shader with just lens distortion correction.
Texture2D Texture : register(t0);
SamplerState Linear : register(s0);
float2 LensCenter;
float2 ScreenCenter;
float2 Scale;
float2 ScaleIn;
float4 HmdWarpParam;


// Scales input texture coordinates for distortion.
// ScaleIn maps texture coordinates to Scales to ([-1, 1]), although top/bottom will be
// larger due to aspect ratio.
float2 HmdWarp(float2 in01)
{
	float2 theta = (in01 - LensCenter) * ScaleIn; // Scales to [-1, 1]
	float  rSq = theta.x * theta.x + theta.y * theta.y;
	float2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + 
		HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);
	return LensCenter + Scale * theta1;
}

float4 main(in float4 oPosition : SV_Position, in float4 oColor : COLOR,
			in float2 oTexCoord : TEXCOORD0) : SV_Target
{
	float2 tc = HmdWarp(oTexCoord);
	if (any(clamp(tc, ScreenCenter-float2(0.25,0.5), ScreenCenter+float2(0.25, 0.5)) - tc))
		return 0;
	return Texture.Sample(Linear, tc);
};





// Shader with lens distortion and chromatic aberration correction.
Texture2D Texture : register(t0);
SamplerState Linear : register(s0);
float2 LensCenter;
float2 ScreenCenter;
float2 Scale;
float2 ScaleIn;
float4 HmdWarpParam;
float4 ChromAbParam;


// Scales input texture coordinates for distortion.
// ScaleIn maps texture coordinates to Scales to ([-1, 1]), although top/bottom will be
// larger due to aspect ratio.
float4 main(in float4 oPosition : SV_Position, in float4 oColor : COLOR,
			in float2 oTexCoord : TEXCOORD0) : SV_Target
{
	float2 theta = (oTexCoord - LensCenter) * ScaleIn; // Scales to [-1, 1]
	float  rSq = theta.x * theta.x + theta.y * theta.y;
	float2 theta1 = theta * (HmdWarpParam.x + HmdWarpParam.y * rSq + 
		HmdWarpParam.z * rSq * rSq + HmdWarpParam.w * rSq * rSq * rSq);

	// Detect whether blue texture coordinates are out of range since these will scaled out the furthest.
	float2 thetaBlue = theta1 * (ChromAbParam.z + ChromAbParam.w * rSq);
	float2 tcBlue = LensCenter + Scale * thetaBlue;
	if (any(clamp(tcBlue, ScreenCenter-float2(0.25,0.5), ScreenCenter+float2(0.25, 0.5)) - tcBlue))
		return 0;

	// Now do blue texture lookup.
	float  blue = Texture.Sample(Linear, tcBlue).b;

	// Do green lookup (no scaling).
	float2 tcGreen = LensCenter + Scale * theta1;
	float4 greenColor = Texture.Sample(Linear, tcGreen);
	float  green = greenColor.g;
	float  alpha = greenColor.a;

	// Do red scale and lookup.
	float2 thetaRed = theta1 * (ChromAbParam.x + ChromAbParam.y * rSq);
	float2 tcRed = LensCenter + Scale * thetaRed;
	float  red = Texture.Sample(Linear, tcRed).r;

	return float4(red, green, blue, alpha);
};