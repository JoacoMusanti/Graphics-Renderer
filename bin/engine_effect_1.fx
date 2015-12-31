Texture2D tex2D;
SamplerState linearSampler
{
	Filter   = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

// --------------------------------------------------------------
// STRUCTS, BITCHES
// --------------------------------------------------------------
struct Material
{
	float3 ambient;
	float3 diffuse;
	float3 specular;
	float3 illum;
};

struct Light
{
	float3 position;
	float3 direction;		// Target lights
	float3 ambient;
	float3 diffuse;
	float3 specular;
};

// --------------------------------------------------------------
// GLOBALS, BITCHES
// --------------------------------------------------------------

matrix World;
matrix View;
matrix Projection;
matrix WorldInverseTranspose;

float4 DColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
float3 LPos = float3(0, 0, 0);

Material mat;
Light light;

//--------------------------------------------------------------------------------------

struct VS_INPUT
{
    float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD0;
	float3 Normal : NORMAL;
	float3 Light : TEXCOORD1;
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
	float4 WorldPos = 0.0f;

    PS_INPUT output = (PS_INPUT)0;

    output.Pos = mul( input.Pos, World );
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );

	output.TexCoord = input.TexCoord;

	output.Normal = normalize( mul( input.Normal, (float3x3)World ) );

	WorldPos = mul( input.Pos, World );

	output.Light = normalize( (LPos.xyz - WorldPos.xyz) );

    return output;
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT input ) : SV_TARGET
{
	float4 textureColor;
	float4 LColor = (1.0f, 1.0f, 1.0f, 1.0f);
	float LIntensity;

	LIntensity = saturate( dot( input.Normal, input.Light ) );

	LColor = DColor * LIntensity;

	textureColor = tex2D.Sample( linearSampler, input.TexCoord );

	return saturate(LColor) * textureColor;
}

//--------------------------------------------------------------------------------------
technique10 Render
{
   pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

