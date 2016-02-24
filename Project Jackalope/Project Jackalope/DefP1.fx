cbuffer Constants
{
	matrix WorldViewProjMatrix;
	matrix WorldMatrix;
	float3 SpecularAlbedo;
	float SpecularPower;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
SamplerState AnisoSampler;

struct VSInput
{
	float4 Position : POSITION;
	float2 TexCoord : TEXCOORDS0;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
};

struct VSOutput
{
	float4 PositionCS : SV_Position;
	float2 TexCoord : TEXCOORD;
	float3 NormalWS : NORMALWS;
	float3 PositionWS : POSITIONWS;
	float3 TangentWS : TANGENTWS;
	float3 BitangentWS : BITANGENTWS;
};

struct PSInput
{
	float4 PositonSS : SV_Position;
	float2 TexCoord : TEXCOORD;
	float3 NormalWS : NORMALWS;
	float3 PositionWS : POSITIONWS;
	float3 TangentWS : TANGENTWS;
	float3 BitangentWS : BITANGENTWS;
};

struct PSOutput
{
	float4 Normal : SV_Target0;
	float4 DiffuseAlbedo : SV_Target1;
	float4 SpecularAlbedo : SV_Target2;
	float4 Position : SV_Target3;
};

VSOutput VSMain( in VSInput input )
{
	VSOutput output;

	output.PositionWS = mul(input.Position, WorldMatrix).xyz;
	float3 normalWS = normalize(mul(input.Normal, (float3x3)WorldMatrix));
	output.NormalWS = normalWS;

	float3 tangentWS = normalize(mul(input.Tangent.xyz, (float3x3)WorldMatrix));
	float3 bitangentWS = normalize(cross(normalWS, tangentWS)) * input.Tangent.w;

	output.TangentWS = tangentWS;
	output.BitangentWS = bitangentWS;

	output.PositionCS = mul(input.Position, WorldViewProjMatrix);

	output.TexCoord = input.TexCoord;

	return output;
}

PSOutput PSMain( in PSInput input )
{
	PSOutput output;

	float3 diffuseAlbedo = DiffuseMap.Sample(AnisoSampler, input.TexCoord).rgb;
	
	float3x3 tangentFrameWS = float3x3(normalize(input.TangentWS), normalize(input.BitangentWS), normalize(input.NormalWS));

	float3 normalTS = NormalMap.Sample(AnisoSampler, input.TexCoord).rgb;
	normalTS = normalize(normalTS * 2.0f - 1.0f);

	float3 normalWS = mul(normalTS, tangentFrameWS);

	output.Normal = float4(normalWS, 1.0f);
	output.DiffuseAlbedo = float4(diffuseAlbedo, 1.0f);
	output.SpecularAlbedo = float4(SpecularAlbedo, 1.0f);
	output.Position = float4(input.PositionWS, 1.0f);

	return output;
}