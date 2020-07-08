#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

layout (std140) uniform FragUniform
{
    vec4 cameraPos;
    bool red;
};

uniform samplerCube cubemapTex;

void main()
{          
     if (red)
    {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); 
    }
    else
    {
        float ratio = 1.00 / 1.52;
        vec3 I = normalize(FragPos - cameraPos.xyz);
        vec3 R = refract(I, normalize(Normal), ratio);
        FragColor = vec4(texture(cubemapTex, R).rgb, 1.0);
    }
}