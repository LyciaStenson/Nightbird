#version 450

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec2 aBaseColorTexCoord;

layout(binding = 1, std140) uniform CameraUBO
{
	mat4 view;
	mat4 projection;
	vec4 position;
};

layout(binding = 2, std140) uniform ModelUBO
{
	mat4 model;
};

layout(location = 0) out vec2 fragBaseColorTexCoord;

void main()
{
	gl_Position = projection * view * model * vec4(aPosition.xyz, 1.0);
	fragBaseColorTexCoord = aBaseColorTexCoord;
}
