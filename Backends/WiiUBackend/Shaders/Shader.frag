#version 450

layout(location = 0) in vec3 fragWorldPos;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 fragBaseColorTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1, std140) uniform CameraUBO
{
	mat4 cameraView;
	mat4 cameraProj;
	vec4 cameraPos;
};

struct DirectionalLight
{
	vec4 direction;
	vec4 colorIntensity;
};

struct DirectionalLightData
{
	DirectionalLight lights[8];
	uint count;
};

layout(binding = 4, std140) uniform DirectionalLightUBO
{
	DirectionalLightData directionalLights;
};

struct PointLight
{
	vec4 positionRadius;
	vec4 colorIntensity;
};

struct PointLightData
{
	PointLight lights[64];
	uint count;
};

layout(binding = 5, std140) uniform PointLightUBO
{
	PointLightData pointLights;
};

struct AmbientLightData
{
	vec4 colorIntensity;
};

layout(binding = 6, std140) uniform AmbientLightUBO
{
	AmbientLightData ambientLight;
};

layout(binding = 3) uniform sampler2D baseColorSampler;

void main()
{
	vec4 baseColor = texture(baseColorSampler, fragBaseColorTexCoord);
	vec3 normal = normalize(fragNormal);

	// Ambient light
	vec3 color = ambientLight.colorIntensity.rgb * ambientLight.colorIntensity.a * baseColor.rgb;

	vec3 viewDir = normalize(cameraPos.xyz - fragWorldPos);

	// Directional lights
	for (uint i = 0; i < directionalLights.count || i < 8; ++i)
	{
		DirectionalLight light = directionalLights.lights[i];
		vec3 lightDir = normalize(-light.direction.xyz);
		vec3 lightColor = light.colorIntensity.rgb;
		float intensity = light.colorIntensity.a;

		float diffuse = max(dot(normal, lightDir), 0.0);

		vec3 halfDir = normalize(viewDir + lightDir);
		float specularFactor = max(dot(normal, halfDir), 0.0);
		float shininess = 32.0;
		float specular = pow(specularFactor, shininess);

		color += (diffuse + specular) * lightColor * intensity * baseColor.rgb;
	}
	
	// Point lights
	for (uint i = 0; i < pointLights.count || i < 64; ++i)
	{
		PointLight light = pointLights.lights[i];
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

		vec3 halfDir = normalize(viewDir + lightDir);
		float specularFactor = max(dot(normal, halfDir), 0.0);
		float shininess = 32.0;
		float specular = pow(specularFactor, shininess);

		float attenuation = 1.0 - clamp(distance / radius, 0.0, 1.0);

		color += (diffuse + specular) * lightColor * intensity * attenuation * baseColor.rgb;
	}
	
	outColor = vec4(color, baseColor.a);
}
