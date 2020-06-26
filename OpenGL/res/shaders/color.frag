#version 330 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float innerCutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

vec3 CalcDirLight(DirLight light, const vec3 normal, const vec3 viewDir, const vec3 diffMap, const vec3 specMap);
vec3 CalcPointLight(PointLight light, const vec3 normal, const vec3 viewDir, const vec3 diffeMap, const vec3 specMap);
vec3 CalcSpotLight(SpotLight light, const vec3 normal, const vec3 viewDir, const vec3 diffMap, const vec3 specMap);

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform Material material;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform SpotLight spotLight;
#define NR_POINT_LIGHT 4
uniform PointLight pointLights[NR_POINT_LIGHT];
//uniform float time;

void main()
{
	// properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 diffMap = texture(material.diffuse, TexCoords).rgb;
	vec3 specMap = texture(material.specular, TexCoords).rgb;
	vec3 result = vec3(0.0);

	// Directional lighting
	result += CalcDirLight(dirLight, norm, viewDir, diffMap, specMap);
	// Point lights
	for (int i=0; i < NR_POINT_LIGHT; i++)
		result += CalcPointLight(pointLights[i], norm, viewDir, diffMap, specMap);
	// Spot light
	result += CalcSpotLight(spotLight, norm, viewDir, diffMap, specMap);

	FragColor = vec4(result, 1.0f);
	/*vec3 emission = vec3(0);
	if (texture(material.specular, TexCoords).r == 0 && diff == 0)
	{
		emission = texture(material.emission, TexCoords).rgb;
		emission = texture(material.emission, TexCoords + vec2(0.0, time/3)).rgb;
	}*/	
}

vec3 CalcDirLight(DirLight light, const vec3 normal, const vec3 viewDir, const vec3 diffMap, const vec3 specMap)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	// combine results
	vec3 ambient = light.ambient * diffMap;
	vec3 diffuse = light.diffuse * diff * diffMap;
	vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

	return (ambient + diffuse + specular);
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
	vec3 ambient  = light.ambient  * diffMap;
    vec3 diffuse  = light.diffuse  * diff * diffMap;
    vec3 specular = light.specular * spec * specMap;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, const vec3 normal, const vec3 viewDir, const vec3 diffMap, const vec3 specMap)
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

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.innerCutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	// combine results
	vec3 ambient  = light.ambient  * diffMap;
    vec3 diffuse  = light.diffuse  * diff * diffMap;
    vec3 specular = light.specular * spec * specMap;
    ambient  *= attenuation;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}