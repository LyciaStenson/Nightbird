#version 450

layout(set = 0, binding = 0) uniform CameraUBO
{
	mat4 view;
	mat4 projection;
	vec4 position;
} cameraUBO;

layout(set = 1, binding = 0) uniform MeshUBO
{
	mat4 model;
} meshUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inBaseColorTexCoord;
layout(location = 3) in vec2 inMetallicRoughnessTexCoord;
layout(location = 4) in vec2 inNormalTexCoord;

layout(location = 0) out vec3 fragWorldPos;
layout(location = 1) out vec3 fragNormal;

layout(location = 2) out vec2 fragBaseColorTexCoord;
layout(location = 3) out vec2 fragMetallicRoughnessTexCoord;
layout(location = 4) out vec2 fragNormalTexCoord;

void main()
{
	vec4 worldPosition = meshUBO.model * vec4(inPosition, 1.0);
	fragWorldPos = worldPosition.xyz;
	
	mat3 normalMat = transpose(inverse(mat3(meshUBO.model)));
	fragNormal = normalize(normalMat * inNormal);
	
	gl_Position = cameraUBO.projection * cameraUBO.view * worldPosition;
	fragBaseColorTexCoord = inBaseColorTexCoord;
	fragMetallicRoughnessTexCoord = inMetallicRoughnessTexCoord;
	fragNormalTexCoord = inNormalTexCoord;
}
