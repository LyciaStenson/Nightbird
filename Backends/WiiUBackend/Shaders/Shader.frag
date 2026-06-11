#version 450

layout(location = 0) in vec2 fragBaseColorTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 3) uniform sampler2D baseColorSampler;

void main()
{
	outColor = texture(baseColorSampler, fragBaseColorTexCoord);
}
