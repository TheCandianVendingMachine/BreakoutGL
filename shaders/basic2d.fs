#version 330 core

uniform vec3 DefinedColour;

out vec4 FragColour;
in vec3 VertexColour;
in vec2 TextureCoord;

uniform sampler2D sprite;

void main() {
    FragColour = texture(sprite, TextureCoord).rgba * vec4(VertexColour, 1.0) * vec4(DefinedColour, 1.0);
}

