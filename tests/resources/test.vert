#version 450

// vulkan NDC:	x: -1(left), 1(right)
//				y: -1(top), 1(bottom)

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(push_constant) uniform Transformation {
	mat4 transformation;
} transformInstance;

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = transformInstance.transformation * vec4(inPosition, 0.0, 1.0);
	fragColor = inColor;
}
