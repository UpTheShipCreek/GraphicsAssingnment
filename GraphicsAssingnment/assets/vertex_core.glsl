#version 330 core

// The locations should match the glVertexAttribPointer calls
layout(location = 0) in vec3 vertexPosition; // Position
layout(location = 1) in vec3 vertexNormal; // Normal
layout(location = 2) in vec2 vertexTexCoords; // Texture

// What we feed to the fragment shader
out vec2 fragmentTexCoords; // Texture
out vec3 Normal; // Normal
out vec2 TexCoord;

// Uniforms, handled by the shader.setMat4 method of the shader class (see shader.h and shader.cpp)
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
    fragmentTexCoords = vec2(vertexTexCoords.x, 1.0 - vertexTexCoords.y);
    TexCoord = vertexTexCoords;
    Normal = mat3(transpose(inverse(model))) * vertexNormal; // Transform normal to world space
}
