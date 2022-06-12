#version 330 core

uniform vec3 DefinedColour;

out vec4 FragColour;
in vec4 VertexColour;
in vec2 TextureCoord;

uniform sampler2D sprite;

void main() {
    FragColour = texture(sprite, TextureCoord).rgba * VertexColour * vec4(DefinedColour, 1.0);
}

