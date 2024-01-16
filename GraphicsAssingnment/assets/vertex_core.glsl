#version 330 core

layout(location = 0) in vec3 vertexPosition; // Position
layout(location = 1) in vec2 vertexTexCoords; // Color


out vec2 fragmentTexCoords; // Texture

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    fragmentTexCoords = vec2(vertexTexCoords.x, 1.0 - vertexTexCoords.y);
}