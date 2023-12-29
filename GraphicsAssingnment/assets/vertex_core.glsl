#version 330 core

layout(location = 0) in vec3 aPos; // Assuming aPos is an attribute
out vec3 fragPos; // Pass the position to the fragment shader

void main() {
    gl_Position = vec4(aPos, 1.0);
    fragPos = aPos;
}