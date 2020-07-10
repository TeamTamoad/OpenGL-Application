#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} geom_in[];

out VERT_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} geom_out;

uniform float time;

vec3 getNormal()
{
	vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
	return normalize(cross(a,b));
}

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.5;
	vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
	return position + vec4(direction, 0.0);
}

void main()
{
	vec3 normal = getNormal();

	gl_Position = explode(gl_in[0].gl_Position, normal);
	geom_out.FragPos = explode(vec4(geom_in[0].FragPos, 1.0), normal).xyz;
	geom_out.Normal = geom_in[0].Normal;
	geom_out.TexCoords = geom_in[0].TexCoords;
	EmitVertex();

	gl_Position = explode(gl_in[1].gl_Position, normal);
	geom_out.FragPos = explode(vec4(geom_in[1].FragPos, 1.0), normal).xyz;
	geom_out.Normal = geom_in[1].Normal;
	geom_out.TexCoords = geom_in[1].TexCoords;
	EmitVertex();

	gl_Position = explode(gl_in[2].gl_Position, normal);
	geom_out.FragPos = explode(vec4(geom_in[2].FragPos, 1.0), normal).xyz;
	geom_out.Normal = geom_in[2].Normal;
	geom_out.TexCoords = geom_in[2].TexCoords;
	EmitVertex();

	EndPrimitive();
}