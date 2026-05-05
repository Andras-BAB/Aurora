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

RWStructuredBuffer<Particle> gParticleBuffers[] : register(u0, space0);
RWStructuredBuffer<uint> gUintBuffers[] : register(u0, space1);

cbuffer SystemParams : register(b0)
{
	float3 EmitterPosition;
	float DeltaTime;
	float3 EmitterVelocity;
	float LifeTimeParams;
	float4 ColorBeginParams;
	float4 ColorEndParams;
	float SizeBeginParams;
	float SizeEndParams;
	float VelocityVariation;
	uint RandomSeed;
	float3 SpawnExtents;
	float VelocityRadial;
	
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
	
	Particle p = gParticleBuffers[ParticleBufferIdx][index];
	
	if (p.LifeRemaining <= 0.0f)
		return;
	
	p.LifeRemaining -= DeltaTime;
	p.Position += p.Velocity * DeltaTime;
	
	// if died in this frame
	if (p.LifeRemaining <= 0.0f)
	{
		p.SizeBegin = 0.0f;
		p.SizeEnd = 0.0f;
		
		// place back to the dead list
		uint insertIndex;
		InterlockedAdd(gUintBuffers[CounterBufferIdx][0], 1, insertIndex);
		
		gUintBuffers[DeadListBufferIdx][insertIndex] = index;
	}
	
	gParticleBuffers[ParticleBufferIdx][index] = p;
}