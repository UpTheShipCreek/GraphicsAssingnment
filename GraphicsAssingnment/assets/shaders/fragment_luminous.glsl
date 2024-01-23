#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
};

uniform sampler2D texture_diffuse1;
uniform vec3 viewPos;
uniform Light light;

void main()
{    
    vec3 ambient = 1 * vec3(1.0, 1.0, 1.0);
    vec3 objectColor = texture(texture_diffuse1,  TexCoords).rgb;
    vec3 result = objectColor * ambient;

    FragColor = vec4(result, 1.0);
}