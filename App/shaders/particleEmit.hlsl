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

uint pcg_hash(inout uint state)
{
    state = state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	return (word >> 22u) ^ word;
}

float rand(inout uint state)
{
    return float(pcg_hash(state)) * (1.0 / 4294967296.0);
}

[numthreads(64, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	if (DTid.x >= EmitCount)
		return;
	
	uint originalCount;
	InterlockedAdd(gUintBuffers[CounterBufferIdx][0], -1, originalCount);
	
	// if the counter was 0, then no more space
	if (originalCount == 0 || originalCount > MaxParticles)
	{
		// give back and restore from underflow
		InterlockedAdd(gUintBuffers[CounterBufferIdx][0], 1, originalCount);
		return;
	}
	
	uint particleIndex = gUintBuffers[DeadListBufferIdx][originalCount - 1];
	
	Particle p;
	uint rngState = RandomSeed ^ particleIndex;
	
	float u = rand(rngState);
	float v = rand(rngState);
	float w = rand(rngState);
	float theta = u * 2.0f * 3.14159265f;
	float phi = acos(2.0f * v - 1.0f);
	
	float sinPhi = sin(phi);
	float3 dir = float3(sinPhi * cos(theta), cos(phi), sinPhi * sin(theta));
	float r = pow(w, 0.3333333f);
	
	float3 spawnOffset = dir * r * SpawnExtents;
	float offsetLen = length(spawnOffset);
	float3 radialDir = offsetLen > 0.0001f ? (spawnOffset / offsetLen) : float3(0.0f, 1.0f, 0.0f);
	
	float3 randomVec = float3(rand(rngState) - 0.5, rand(rngState) - 0.5, rand(rngState) - 0.5);
	float3 randomDir = length(randomVec) > 0.0001f ? normalize(randomVec) : float3(0.0f, 1.0f, 0.0f);
	float randomMagnitude = rand(rngState) * VelocityVariation;
	
	p.Position = EmitterPosition + spawnOffset;
	p.Velocity = EmitterVelocity + (radialDir * VelocityRadial) + (randomDir * randomMagnitude);
	p.LifeTime = LifeTimeParams + (rand(rngState) * 0.5f);
	p.LifeRemaining = p.LifeTime;
	p.SizeBegin = SizeBeginParams;
	p.SizeEnd = SizeEndParams;
	p.ColorBegin = ColorBeginParams;
	p.ColorEnd = ColorEndParams;
	p.Padding = float2(0.0f, 0.0f);
	
	gParticleBuffers[ParticleBufferIdx][particleIndex] = p;
}