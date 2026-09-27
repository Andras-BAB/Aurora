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

float3 ACESFilm(float3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return saturate((x * (a * x + b)) / (x * (c * x + d) + e));
}

float4 PS(VSOutput input) : SV_Target
{
	int3 texCoord = int3(input.Pos.xy, 0);
	//return g_InputTexture.Load(texCoord);
	
	float3 color = g_InputTexture.Load(texCoord).rgb;
	
	// luminance in linear space
	float lum = dot(color, float3(0.2126, 0.7152, 0.0722));

	// desaturation for bright lights to become white (from 2.0 with 0.2 power)
	float desatFactor = saturate((lum - 2.0) * 0.2);
	
	// mix the original color with a bright color
	color = lerp(color, float3(lum, lum, lum), desatFactor);
	
	// Reinhard Tone Mapping (simple HDR -> SDR conversion)
	//color = color / (color + float3(1.0, 1.0, 1.0));
	color = ACESFilm(color);
	
	// gamma correction
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

	// vignette effect
	//float2 uv = input.UV;
	//uv = uv * 2.0 - 1.0;
	//float dist = dot(uv, uv);
	//float vignette = smoothstep(2.0, 0.5, dist);
	//color *= vignette;

	return float4(color, 1.0);
}