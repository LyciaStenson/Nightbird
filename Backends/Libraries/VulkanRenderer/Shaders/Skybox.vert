#version 450

layout(set = 0, binding = 0) uniform CameraUBO
{
	mat4 view;
	mat4 projection;
	vec4 position;
} camera;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragDir;

void main()
{
	mat4 viewNoTranslation = mat4(mat3(camera.view));

	vec4 pos = camera.projection * viewNoTranslation * vec4(inPosition, 1.0);

	gl_Position = pos.xyww;

	fragDir = inPosition;
}
