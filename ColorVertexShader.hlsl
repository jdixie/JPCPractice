cbuffer PerApplication : register(b0)
{
	matrix projectionMatrix;
}

cbuffer PerFrame : register(b1)
{
	matrix viewMatrix;
}

cbuffer PerObject : register(b2)
{
	matrix worldMatrix;
}

struct VertexData
{
	float3 position : POSITION;
	float3 color: COLOR;
};

struct VShaderOutput
{
	float4 color : COLOR;
	float4 position : SV_POSITION;
};

VShaderOutput VShader(VertexData IN)
{
	VShaderOutput OUT;

	matrix mvp = mul(projectionMatrix, mul(viewMatrix, worldMatrix));
	OUT.position = mul(mvp, float4(IN.position, 1.0f));
	OUT.color = float4(IN.color, 1.0f);

	return OUT;
}