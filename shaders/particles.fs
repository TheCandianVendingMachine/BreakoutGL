#version 330 core

out vec4 FragColour;
in vec2 TextureCoord;
in float PercentLived;
uniform sampler2D sprite;

void main() {
    FragColour = texture(sprite, TextureCoord) * vec4(1.0, 1.0, 1.0, pow(1.0 - PercentLived, 0.5));
}

