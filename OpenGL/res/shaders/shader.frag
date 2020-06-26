#version 330 core

in vec3 Color;
in vec2 Texcoord;

out vec4 outColor;
        
uniform sampler2D texKitten;
uniform sampler2D texPuppy;
uniform float time;

void main()
{
    vec4 colKitten = texture(texKitten, Texcoord);
    vec4 colPuppy = texture(texPuppy, Texcoord);

    if (Texcoord.y < 0.5)
        outColor = mix(colKitten, colPuppy, 0.5);
    else
        outColor = texture(texKitten, vec2(Texcoord.x + sin(Texcoord.y * 60.0 + time * 2.0) / 30.0, 1.0 - Texcoord.y));
           
    outColor = outColor  * vec4(Color, 1.0);
}  