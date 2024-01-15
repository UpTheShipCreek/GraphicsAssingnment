#version 330 core

out vec4 fragColor; // Output color

in vec3 vertexColor; // Input from the vertex shader
in vec2 TexCoord; // Input from the vertex shader

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    fragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 1);
}