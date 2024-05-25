#version 450

// vulkan NDC:	x: -1(left), 1(right)
//				y: -1(top), 1(bottom)

layout(set = 0, binding = 0) uniform UBO {
    mat4 view;
    mat4 projection;
    mat4 viewProjection;
} cameraData;

layout(std140, set = 0, binding = 1) readonly buffer InstanceData {
    mat4 modelMatrix[];
} instanceData;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 vertTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = cameraData.viewProjection * instanceData.modelMatrix[gl_InstanceIndex] * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = vertTexCoord;
}
