RWStructuredBuffer<uint> gUintBuffers[] : register(u0, space1);

cbuffer InitParams : register(b0)
{
	// we don't need these for the init phase
	float3 Padding1;
	float Padding2;
	float3 Padding3;
	float Padding4;
	float4 Padding5;
	float4 Padding6;
	float Padding7;
	float Padding8;
	float Padding9;
	uint Padding10;
	float3 Padding11;
	float Padding12;
	
	uint MaxParticles;
	uint ParticleBufferIdx;
	uint DeadListBufferIdx;
	uint CounterBufferIdx;
	
	uint EmitCount;
	float3 Padding;
};

[numthreads(256, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
	uint index = DTid.x;
	
	if (index >= MaxParticles)
		return;
	
	if (index == 0)
	{
		gUintBuffers[CounterBufferIdx][0] = MaxParticles;
	}
	
	gUintBuffers[DeadListBufferIdx][index] = index;
}