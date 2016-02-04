struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    matrix worldViewProj;
	matrix world;
	float3 lightColor;
	float3 lightPosition;
};

// *** Vertex shader ***
VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

	float4 testPos = mul(world, position);

	if(testPos.y < 2)
		output.color = float4(0.0, 0.0, 1.0, 1.0);
	else
		output.color = float4(testPos.y/26, testPos.y/26, testPos.y/26, 1.0);

    output.position = mul(worldViewProj, position);

    return output;
}

// *** Pixel shader ***
float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;		
}