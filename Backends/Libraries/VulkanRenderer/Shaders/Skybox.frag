#version 450

layout(set = 0, binding = 6) uniform samplerCube skyboxSampler;

layout(location = 0) in vec3 fragDir;

layout(location = 0) out vec4 outColor;

void main()
{
	outColor = texture(skyboxSampler, fragDir);
}
