#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrix;

out VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vert_out;

uniform mat4 view;
uniform mat4 projection;

void main()
{  
    // Caculate light in world space
	vert_out.FragPos = vec3(instanceMatrix * vec4(aPos, 1.0));
	vert_out.Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
	vert_out.TexCoords = aTexCoords;

    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
}