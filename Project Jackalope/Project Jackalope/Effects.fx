struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    matrix worldViewProj;
	matrix world;
};

// *** Vertex shader ***
VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = mul(worldViewProj, position);
    output.color = float4(1, 0.4784313725490196, 0.3411764705882353, 1.0);

    return output;
}

// *** Pixel shader ***
float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}