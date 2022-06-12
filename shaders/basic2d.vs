#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec4 aColour;
layout (location = 3) in vec2 aTextureCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 VertexColour;
out vec2 TextureCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPosition.xy, 0.0, 1.0);

    VertexColour = aColour;
    TextureCoord = vec2(aTextureCoord.x, 1.0 - aTextureCoord.y);
}