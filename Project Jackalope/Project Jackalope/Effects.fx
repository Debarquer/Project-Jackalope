struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
	float3 normal : NORMAL;
};

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    matrix worldViewProj;
	matrix world;
	float3 lightColor;
	float3 lightPosition;
};

// *** Vertex shader ***
VOut VShader(float4 position : POSITION, float4 color : COLOR, float3 normal : NORMAL)
{
    VOut output;

	float4 testPos = mul(world, position);

	output.color = float4(testPos.y/26, testPos.y/26, testPos.y/26, 1.0);
    output.position = mul(worldViewProj, position);

    return output;
}

// *** Pixel shader ***
float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float3 normal : NORMAL) : SV_TARGET
{
	return color;		
}