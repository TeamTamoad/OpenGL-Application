#version 330 core
in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube cubemapTex;

void main()
{    
    FragColor = texture(cubemapTex, TexCoords);
}