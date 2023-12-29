#version 330 core

in vec3 fragPos; // Input vertex position
out vec4 fragColor; // Output color

void main() {
    if (distance(fragPos, vec3(0.5f, 0.0f, 0.0f)) < 0.5 || distance(fragPos, vec3(0.5f, 0.5f, 0.0f)) < 0.5) {
        fragColor = vec4(1.0, 0.0, 0.0, 1.0); // Red color if close to specified positions
    } 
    else {
        fragColor = vec4(0.0, 0.0, 1.0, 1.0); // Green color otherwise
    }
}
