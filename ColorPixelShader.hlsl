struct PShaderInput
{
	float4 color : COLOR;
};

float4 PShader(PShaderInput IN) : SV_TARGET
{
	return IN.color;
}