#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

layout (binding = 1) uniform DynamicUniformBufferObject {
	mat4 model;
} dynamicUbo;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inTexCoord;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec3 outFragPos;
layout (location = 2) out vec3 outNormal;

void main() {
	const vec4 worldPos = dynamicUbo.model * vec4(inPosition, 1.0);
	gl_Position = ubo.proj * ubo.view * worldPos;

	outColor = inColor;
	outFragPos = worldPos.xyz;
	outNormal = inNormal;
}