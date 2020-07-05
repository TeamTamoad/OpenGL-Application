#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 200.0;

void main()
{
    // Inverse
    //FragColor = vec4(1.0 - texture(screenTexture, TexCoords).rgb, 1.0);

    // Grayscale (Average)
//    FragColor = texture(screenTexture, TexCoords);
//    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
//    FragColor = vec4(average, average, average, 1.0);

    // Grayscale (Weighted)
//    FragColor = texture(screenTexture, TexCoords);
//    float average = 0.216 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
//    FragColor = vec4(average, average, average, 1.0);

    // Kernel 
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9];
    if (TexCoords.x < 0.5 && TexCoords.y < 0.5)
    {
        // sharpen kernel
        kernel = float[](
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
        );
    }
    else if (TexCoords.x > 0.5 && TexCoords.y < 0.5)
    {
         // blur kernel
        kernel = float[](
            1.0 / 16, 2.0 / 16, 1.0 / 16,
            2.0 / 16, 4.0 / 16, 2.0 / 16,
            1.0 / 16, 2.0 / 16, 1.0 / 16  
        );
    }
    else if (TexCoords.x >= 0.5 && TexCoords.y >= 0.5)
    {
         // edge-detection kernel
         kernel = float[](
            1,  1, 1,
            1, -8, 1,
            1,  1, 1
        );
    }
    else
    {
        kernel = float[](
            0, 0, 0,
            0, 1, 0,
            0, 0, 0
        );
    }

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
        sampleTex[i] = texture(screenTexture, TexCoords.st + offsets[i]).rgb;

    vec3 col = vec3(0.0);
    for (int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
} 
