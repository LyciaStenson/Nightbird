#version 450

layout(set = 0, binding = 0) uniform CameraUBO
{
	mat4 view;
	mat4 projection;
	vec4 position;
} cameraUBO;

struct DirectionalLight
{
	vec4 direction;
	vec4 colorIntensity;
};

layout (std430, set = 0, binding = 1) readonly buffer DirectionalLights
{
	DirectionalLight directionalLights[];
};

layout (set = 0, binding = 2) uniform DirectionalLightMeta
{
	uint count;
} directionalLightMeta;

struct PointLight
{
	vec4 positionRadius;
	vec4 colorIntensity;
};

layout (std430, set = 0, binding = 3) readonly buffer PointLights
{
	PointLight pointLights[];
};

layout (set = 0, binding = 4) uniform PointLightMeta
{
	uint count;
} pointLightMeta;

layout(set = 2, binding = 0) uniform MaterialFactorsUBO
{
	vec4 baseColor;
	vec3 metallicRoughness;
} factorsUBO;

layout(set = 2, binding = 1) uniform sampler2D baseColorSampler;
layout(set = 2, binding = 2) uniform sampler2D metallicRoughnessSampler;
layout(set = 2, binding = 3) uniform sampler2D normalSampler;

layout(location = 0) in vec3 fragWorldPos;
layout(location = 1) in vec3 fragNormal;

layout(location = 2) in vec2 fragBaseColorTexCoord;
layout(location = 3) in vec2 fragMetallicRoughnessTexCoord;
layout(location = 4) in vec2 fragNormalTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 baseColor = texture(baseColorSampler, fragBaseColorTexCoord) * factorsUBO.baseColor;

	vec4 metallicRoughness = texture(metallicRoughnessSampler, fragMetallicRoughnessTexCoord);
	float metallic = metallicRoughness.b * factorsUBO.metallicRoughness.b;
	float roughness = metallicRoughness.g * factorsUBO.metallicRoughness.g;

	//vec3 normal = texture(normalSampler, fragNormalTexCoord).rgb;
	vec3 normal = normalize(fragNormal);
	
	//outColor = vec4(vec3(metallic), 1.0);
	//outColor = vec4(vec3(roughness), 1.0);
	//outColor = vec4(normal, 1.0);

	vec3 color = vec3(0.0);

	for (uint i = 0; i < directionalLightMeta.count; ++i)
	{
		DirectionalLight light = directionalLights[i];
		vec3 lightDir = normalize(-light.direction.xyz);
		vec3 lightColor = light.colorIntensity.rgb;
		float intensity = light.colorIntensity.a;

		float diffuse = max(dot(normal, lightDir), 0.0);

		vec3 viewDir = normalize(cameraUBO.position.xyz - fragWorldPos);
		vec3 halfDir = normalize(viewDir + lightDir);
		float specularFactor = max(dot(normal, halfDir), 0.0);

		float shininess = mix(4.0, 128.0, 1.0 - roughness);
		float specular = pow(specularFactor, shininess);

		color += (diffuse + specular) * lightColor * intensity;
	}

	for (uint i = 0; i < pointLightMeta.count; ++i)
	{
		PointLight light = pointLights[i];
		vec3 lightPos = light.positionRadius.xyz;
		float radius = light.positionRadius.w;
		vec3 lightColor = light.colorIntensity.rgb;
		float intensity = light.colorIntensity.a;

		vec3 lightDir = lightPos - fragWorldPos;
		float distance = length(lightDir);

		if (distance > radius)
			continue;

		lightDir = normalize(lightDir);

		float diffuse = max(dot(normal, lightDir), 0.0);
		
		vec3 viewDir = normalize(cameraUBO.position.xyz - fragWorldPos);
		vec3 halfDir = normalize(viewDir + lightDir);
		float specularFactor = max(dot(normal, halfDir), 0.0);

		float shininess = mix(4.0, 128.0, 1.0 - roughness);
		
		float specular = pow(specularFactor, shininess);
		
		float attenuation = 1.0 - clamp(distance / radius, 0.0, 1.0);
		
		color += (diffuse + specular) * lightColor * intensity * attenuation;
	}

	//color *= baseColor.rgb;

	color = baseColor.rgb;

	vec3 gammaCorrected = pow(color, vec3(1.0 / 2.2));
	outColor = vec4(gammaCorrected, baseColor.a);
}
