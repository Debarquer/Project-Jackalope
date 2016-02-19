struct VOut
{
    float4 position : SV_POSITION;
	float4 wposition : WPOSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	float3 tangent : TANGENT;
};

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    matrix worldViewProj;
	matrix world;
	float3 lightColor;
	float3 lightPosition;
};

Texture2D tex : register(t0);
Texture2D normalMap : register(t1);
SamplerState sampAni;

// *** Vertex shader ***
VOut VShader(float4 position : POSITION, float4 color : COLOR, float3 normal : NORMAL, float2 uv : TEXCOORD, float3 tangent : TANGENT)
{
    VOut output;

	//float4 testPos = mul(world, position);

	output.color = float4(1.0, 1.0, 1.0, 1.0);
	output.wposition = mul(world, position);
    output.position = mul(worldViewProj, position);
	output.normal = mul(world, normal);
	output.tangent = mul(world, tangent);
	output.uv = uv;

    return output;
}

// *** Pixel shader ***
float4 PShader(float4 position : SV_POSITION, float4 wposition : WPOSITION, float4 color : COLOR, float3 normal : NORMAL, float2 uv : TEXCOORD) : SV_TARGET
{
	float4 s = tex.Sample(sampAni, uv);
	float4 nCol = normalMap.Sample(sampAni, uv);

	float3 norNormal = normalize(normal);
	float3 norLightPos = normalize(lightPosition - wposition);
	float angle = max(dot(norLightPos, norNormal), 0.0);

	return s*angle + s*0.1;//color*angle + color*0.01;
}