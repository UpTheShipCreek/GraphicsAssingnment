#version 330 core

in vec2 fragmentTexCoords;
in vec3 Normal; 
in vec2 TexCoord; // Input from the vertex shader

out vec4 finalColor;


uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    finalColor =  mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5);
}