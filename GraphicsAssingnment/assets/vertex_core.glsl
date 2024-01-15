#version 330 core

layout(location = 0) in vec3 aPos; // Position
layout(location = 1) in vec3 aColor; // Color
layout(location = 2) in vec2 aTexCoord; // Texture

out vec3 vertexColor; // Output a color to the fragment shader
out vec2 TexCoord; // Output a texture coordinate to the fragment shader

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * transform * vec4(aPos, 1.0f);
    vertexColor = aColor;
	TexCoord = aTexCoord;
}