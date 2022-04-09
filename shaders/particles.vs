#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 3) in vec2 aTextureCoord;

uniform mat4 view;
uniform mat4 projection;

struct ParticleData {
    vec2 spawn;
    vec2 initialVelocity;
    uint runtime; // time this particle has been alive in microseconds
    int accelerationType;
    float size;
    float rotation;
};

layout(std430) readonly buffer ParticleBuffer {
    ParticleData particleData[];
};

out vec2 TextureCoord;

vec2 accelTypeNone(ParticleData particle)
{
    return particle.spawn + particle.initialVelocity * float(particle.runtime) / 1000000.0;
}

vec2 accelTypeGravity(ParticleData particle)
{
    float runtime = float(particle.runtime) / 1000000.0;
    return particle.spawn + particle.initialVelocity * runtime + 0.5 * vec2(0, 50) * runtime * runtime;
}

void main() {
    ParticleData workingParticle = particleData[gl_InstanceID];

    vec2 currentOffset = vec2(0);
    switch (workingParticle.accelerationType) {
        case 0:
            currentOffset = accelTypeNone(workingParticle);
            break;
        case 1:
            currentOffset = accelTypeGravity(workingParticle);
            break;
    }

    vec2 adjustedPosition = workingParticle.size * aPosition.xy;

    vec2 finalPosition = vec2(
        adjustedPosition.x * cos(workingParticle.rotation) - adjustedPosition.y * sin(workingParticle.rotation),
        adjustedPosition.x * sin(workingParticle.rotation) + adjustedPosition.y * cos(workingParticle.rotation)
    );

    gl_Position = projection * view * vec4(finalPosition + currentOffset, 0.0, 1.0);

    TextureCoord = vec2(aTextureCoord.x, 1.0 - aTextureCoord.y);
}