#version 330 core

in vec2 fragmentTexCoords;

out vec4 finalColor;

uniform sampler2D basicTexture;

void main() {
    finalColor =  texture(basicTexture, fragmentTexCoords);
}