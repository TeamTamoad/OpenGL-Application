#version 330 core
struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct PointLight
{
	vec3 position;
	vec3 diffuse;

	float constant;
	float linear;
	float quadratic;	
};

vec3 CalcPointLight(PointLight light, const vec3 normal, const vec3 viewDir, const vec3 diffeMap, const vec3 specMap);

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform Material material;
uniform PointLight pointLight;
uniform vec3 ambient;
uniform vec3 viewPos;

void main()
{    
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 diffMap = texture(material.texture_diffuse1, TexCoords).rgb;
	vec3 specMap = texture(material.texture_specular1, TexCoords).rgb;
	vec3 result = vec3(0.0);

	result += CalcPointLight(pointLight, norm, viewDir, diffMap, specMap);
	result += ambient * diffMap;

	FragColor = vec4(result, 1.0);
}

vec3 CalcPointLight(PointLight light, const vec3 normal, const vec3 viewDir, const vec3 diffMap, const vec3 specMap)
{
	vec3 lightDir = normalize(light.position - FragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// attenuation
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	
	// combine results
    vec3 diffuse  = light.diffuse  * diff * diffMap;
    vec3 specular = light.diffuse * spec * specMap;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (diffuse + specular);
}