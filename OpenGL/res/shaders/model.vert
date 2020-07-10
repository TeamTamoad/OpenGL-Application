#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

void main()
{  
    // Caculate light in world space
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = mat3(transpose(inverse(model))) * aNormal;
	TexCoords = aTexCoords;

	// Explode
	float magnitude = 1.5;
	vec3 direction = Normal * ((sin(time * 0.5) + 1.0) / 2.0) * magnitude;
	vec4 position = vec4(aPos, 1.0) + vec4(direction, 0.0);
    gl_Position = projection * view * model * position;
}