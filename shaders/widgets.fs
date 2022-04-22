#version 330 core

out vec4 FragColour;
in vec3 VertexColour;
in vec2 TextureCoord;

uniform sampler2D sprite;

void main() {
    FragColour = texture(sprite, TextureCoord).rgba * vec4(VertexColour, 1.0);
}
