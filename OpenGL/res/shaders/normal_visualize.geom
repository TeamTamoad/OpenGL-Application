#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VERT_OUT {
	vec3 Normal;
} geom_in[];

uniform float magnitude = 0.15;

uniform mat4 projection;

void GenerateLine(int index)
{
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = projection * (gl_in[index].gl_Position + vec4(geom_in[index].Normal, 0.0) * magnitude);
	EmitVertex();
	EndPrimitive();
}

void main()
{
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}