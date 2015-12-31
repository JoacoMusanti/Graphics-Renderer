cbuffer cbPerFrame : register ( b0 )
{
	matrix View			: packoffset ( c0 );
	matrix Projection	: packoffset ( c4 );
}

cbuffer cbPerObj : register ( b1 )
{
	matrix World		: packoffset ( c0 );
}

struct PointLight
{
	float4 Color;
	float3 Position;
};

static const uint MaxLights = 128;

cbuffer cbPointLight : register ( b2 )
{
	PointLight Lights[MaxLights]	: packoffset ( c0 );
	uint NumLights					: packoffset ( c256 );
}


cbuffer cbMaterial : register ( b3 )
{
	float4 MatKd		: packoffset ( c0 );
	float4 MatKa		: packoffset ( c1 );
	float4 MatKs		: packoffset ( c2 );
}

cbuffer cbCamera : register ( b4 )
{
	float4 CamPosition	: packoffset ( c0 );
}

SamplerState gLinearSampler : register ( s0 );
Texture2D	 gTextDiffuse	: register ( t0 );

//--------------------------------------------------------------------------------------

struct VS_INPUT
{
    float4 Pos		: POSITION;
	float3 Normal	: NORMAL;
	float2 TexCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos		: SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal	: NORMAL;
	float3 LightPos : TEXCOORD1;
};

// Lighting function
// -----------------------------------------------------
float4 CalcDirLighting( float3 Normal, float3 Pos )
{
	float intensity = saturate( dot( Normal, Pos ) );
	
	return saturate( Lights[0].Color * intensity );
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	input.Pos.w = 1.0f;

    PS_INPUT output = (PS_INPUT)0;

    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

	output.TexCoord = input.TexCoord;

	output.Normal	= normalize( mul( input.Normal, (float3x3) World ) );

	float4 WorldPosition = mul( input.Pos, World );

	output.LightPos = normalize( Lights[0].Position.xyz - WorldPosition.xyz );

    return output;
}

float4 PS( PS_INPUT input ) : SV_TARGET
{
	float4 Color = CalcDirLighting( input.Normal, input.LightPos );

	return gTextDiffuse.Sample( gLinearSampler, input.TexCoord ) * Color;
}
