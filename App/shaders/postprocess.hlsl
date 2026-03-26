struct VSOutput
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD;
};

VSOutput VS(uint vertexID : SV_VertexID)
{
	VSOutput output;
	// full screen triangle
	output.UV = float2((vertexID << 1) & 2, vertexID & 2);
	output.Pos = float4(output.UV * float2(2, -2) + float2(-1, 1), 0, 1);
	return output;
}

Texture2D g_InputTexture : register(t0);
SamplerState g_Sampler : register(s0);

float4 PS(VSOutput input) : SV_Target
{
    int3 texCoord = int3(input.Pos.xy, 0);
    return g_InputTexture.Load(texCoord);
    
    float3 color = g_InputTexture.Load(texCoord).rgb;

    // Reinhard Tone Mapping (simple HDR -> SDR konverzió)
    color = color / (color + float3(1.0, 1.0, 1.0));

    // gamma correction
    color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

    // vignette effect
    float2 uv = input.UV;
    uv = uv * 2.0 - 1.0;
    float dist = dot(uv, uv);
    float vignette = smoothstep(2.0, 0.5, dist);
    color *= vignette;

    return float4(color, 1.0);
}