struct Particle
{
	float3 Position;
	float LifeRemaining;
	float3 Velocity;
	float LifeTime;
	float4 ColorBegin;
	float4 ColorEnd;
	float SizeBegin;
	float SizeEnd;
	float2 Padding;
};

Texture2D<float4> gTextures[] : register(t0, space0);
StructuredBuffer<Particle> gBuffers[] : register(t0, space1);
SamplerState gsamLinearWrap : register(s0);

// TODO: can place DepthMapIndex (0. index) to PassConstants and ParticleBufferIndex to MaterialConstants to prevent usage of another slot
cbuffer RootData : register(b3)
{
	//uint ParticleBufferIndex;
	//uint DepthMapIndex;
	
	//uint DynamicIndices[4];
	// cannot use array, because of packing (array uses 16 byte offset for every element - 4 pieces of 32 bit DWORD)
	uint DynamicIndex0;
	uint DynamicIndex1;
	uint DynamicIndex2;
	uint DynamicIndex3;
};

#include "LightingUtils.hlsl"

cbuffer CameraData : register(b0)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;
	
	Light gLights[MaxLights];
};

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD;
	float ViewZ : TEXCOORD1; // distance from the camera
	//float4 Color : COLOR;
	float LifeRatio : TEXCOORD2;
};

VS_OUTPUT VS(uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
	uint particleBufferIndex = DynamicIndex0;
	
	VS_OUTPUT output;
	
	Particle p = gBuffers[particleBufferIndex][instanceID];
	
	if (p.LifeRemaining <= 0.0f)
	{
		output.Pos = float4(0, 0, 0, 0);
		//output.Pos = float4((0.0f / 0.0f), 0, 0, 0); // NaN, throws instantly before the rasterization
		output.UV = float2(0, 0);
		//output.Color = float4(0, 0, 0, 0);
		output.ViewZ = 0;
		output.LifeRatio = -1;
		return output;
	}

	// interpolation
	float lifeRatio = 1.0f - (p.LifeRemaining / p.LifeTime);
	output.LifeRatio = lifeRatio;
	
	float currentSize = lerp(p.SizeBegin, p.SizeEnd, lifeRatio);
	float4 currentColor = lerp(p.ColorBegin, p.ColorEnd, lifeRatio);

	float2 quadPos = float2((vertexID & 1) ? 0.5f : -0.5f, (vertexID & 2) ? -0.5f : 0.5f);
	output.UV = quadPos + 0.5f;

	// billboarding
	float3 cameraRight = float3(gView[0][0], gView[1][0], gView[2][0]);
	float3 cameraUp = float3(gView[0][1], gView[1][1], gView[2][1]);
	
	float3 worldPos = p.Position 
					+ (cameraRight * quadPos.x * currentSize)
					+ (cameraUp * quadPos.y * currentSize);

	output.Pos = mul(float4(worldPos, 1.0f), gViewProj);
	output.ViewZ = mul(float4(worldPos, 1.0f), gView).z;
	//output.Color = currentColor;
	
	return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	uint depthIndex = DynamicIndex1;
	uint gradientTexIndex = DynamicIndex2;
	uint textureIndex = DynamicIndex3;
	
	int xCoord = clamp(int(input.LifeRatio * 255.0f), 0, 255);
	
	//float4 particleColor = gTextures[gradientTexIndex].SampleLevel(gsamLinearWrap, float2(input.LifeRatio, 0.5f), 0);
	//float4 particleColor = gTextures[gradientTexIndex].Load(int3(xCoord, 0, 0));
	float4 particleColor = gTextures[gradientTexIndex].Load(int3(xCoord, 0, 0)) * gTextures[textureIndex].Sample(gsamLinearWrap, input.UV);
	
	float dist = length(input.UV - 0.5f);
	float circleAlpha = smoothstep(0.5f, 0.4f, dist);
	
	float4 finalColor = particleColor;
	//float4 finalColor = input.Color;
	finalColor.a *= circleAlpha;
	
	int3 pixelCoord = int3(input.Pos.xy, 0);
	
	float rawDepth = gTextures[depthIndex].Load(pixelCoord).r;
	
	// linearization for z distance
	float bgViewZ = gProj[3][2] / (rawDepth - gProj[2][2]);
	
	// distance between depth and the particle
	float depthDiff = bgViewZ - input.ViewZ;
	//return float4(rawDepth, rawDepth, rawDepth, rawDepth);
	
	// fading particle
	float fadeDistance = 0.5f;
	float softAlpha = saturate(depthDiff / fadeDistance);
	
	float nearFade = saturate((input.ViewZ - 0.01f) / 1.0f);
	finalColor.a *= nearFade;
	
	// do not disappear if the background is the sky (depth is 0.0 with reversed z)
	if (rawDepth == 0.0f)
		softAlpha = 1.0f;
	
	finalColor.a *= softAlpha;
	
	if (finalColor.a < 0.01f)
		discard;

	return finalColor;
}