#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec3 aColour;
layout (location = 3) in vec2 aTextureCoord;

uniform mat4 model;
uniform mat4 projection;

out vec3 VertexColour;
out vec2 TextureCoord;

void main() {
    // weird mat4 = view matrix for standard ortho projection
    gl_Position = projection * mat4(-1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 1) * model * vec4(aPosition.xy, 0.0, 1.0);

    VertexColour = aColour;
    TextureCoord = vec2(aTextureCoord.x, aTextureCoord.y);
}