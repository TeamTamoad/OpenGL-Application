#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VERT_OUT {
	vec3 Normal;
} vert_out;

uniform mat4 model;
uniform mat4 view;

void main()
{  
	// Calculate in view space
	vert_out.Normal = mat3(transpose(inverse(view * model))) * aNormal;
    gl_Position = view * model * vec4(aPos, 1.0);
}