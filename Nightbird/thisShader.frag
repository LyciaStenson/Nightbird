#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

//in vec2 texCoord;

//uniform sampler2D ourTexture;

uniform vec3 lightPos;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main()
{
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = diff * lightColor;

	vec3 result = (ambient + diffuse) * objectColor;

	FragColor = vec4(result, 1.0f);

	//FragColor = texture(ourTexture, texCoord);
}