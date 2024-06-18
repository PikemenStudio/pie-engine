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

struct LightSource {
    vec4 position;
    vec4 color;
    float intensity;
};

layout(set = 0, binding = 2) uniform LightSourceUBO {
    LightSource lights[2];
} light;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 vertTexCoord;
layout(location = 3) in vec3 normal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 lightColor;

void calculate_position(vec4 model) {
    gl_Position = cameraData.viewProjection * model;
}

vec3 calculate_point_light(vec4 model) {
    vec3 result_light_color = vec3(0.0);

    for (int i = 0; i < 2; i++) {
        vec3 direction_to_light = light.lights[0].position.xyz - model.xyz;

        vec3 light_color = light.lights[i].color.xyz * (1.0 / dot(direction_to_light, direction_to_light));
        vec3 ambient = vec3(1.0, 1.0, 1.0) * 0.1;
        vec3 diffuse = light_color * max(dot(normalize(normal), normalize(direction_to_light)), 0.0) * float(light.lights[i].intensity);

        result_light_color += ambient + diffuse;
    }
    return result_light_color;
}

void main() {
    vec4 model = instanceData.modelMatrix[gl_InstanceIndex] * vec4(inPosition, 1.0);
    calculate_position(model);

    lightColor = calculate_point_light(model);

    fragColor = inColor;
    fragTexCoord = vertTexCoord;
    fragNormal = normal;
}
