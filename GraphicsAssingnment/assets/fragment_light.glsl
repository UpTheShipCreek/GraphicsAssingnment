#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    vec3 ambient = 0.8 * vec3(1.0, 1.0, 1.0);
    vec3 objectColor = texture(texture_diffuse1,  TexCoords).rgb;
    vec3 result = objectColor * ambient;

    FragColor = vec4(result, 1.0);
}