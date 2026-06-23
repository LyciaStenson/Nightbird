#version 450

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aBaseColorTexCoord;

layout(binding = 1, std140) uniform CameraUBO
{
	mat4 view;
	mat4 projection;
	vec4 position;
};

layout(binding = 2, std140) uniform ModelUBO
{
	mat4 model;
	mat4 normalMatrix;
};

layout(location = 0) out vec3 fragWorldPos;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 fragBaseColorTexCoord;

void main()
{
	vec4 worldPosition = model * vec4(aPosition.xyz, 1.0);
	fragWorldPos = worldPosition.xyz;
	fragNormal = normalize(mat3(normalMatrix) * aNormal);
	gl_Position = projection * view * worldPosition;
	fragBaseColorTexCoord = aBaseColorTexCoord;
}
