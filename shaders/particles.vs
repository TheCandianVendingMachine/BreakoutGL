#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec3 aColour;
layout (location = 3) in vec2 aTextureCoord;

uniform mat4 view;
uniform mat4 projection;

struct ParticleData {
    vec2 spawn;
    vec2 initialVelocity;
    float runtime;
    int accelerationType;
};

layout(std430) readonly buffer ParticleBuffer {
    ParticleData particleData[];
};

out vec3 VertexColour;
out vec2 TextureCoord;

void main() {
    vec2 currentOffset = particleData[gl_InstanceID].spawn;
    currentOffset += particleData[gl_InstanceID].initialVelocity * particleData[gl_InstanceID].runtime;

    gl_Position = projection * view * vec4(aPosition.xy + currentOffset, 0.0, 1.0);

    VertexColour = aColour;
    TextureCoord = vec2(aTextureCoord.x, 1.0 - aTextureCoord.y);
}