#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 fragTexCoord;
layout (location = 2) in vec3 fragNormal;
layout (location = 3) in vec3 lightColor;

layout (location = 0) out vec4 outColor;

layout (set = 1, binding = 0) uniform sampler2D material;
layout (set = 1, binding = 1) uniform sampler2D material1;

const vec4 sunColor = vec4(1.0, 1.0, 1.0, 1.0);
const vec3 sunDirection = normalize(vec3(0.0, 0.0, -1.0));

void main() {
    //vec4 light_color = vec4(lightColor.xyz, 1.0);
    vec4 light_color = vec4(lightColor.xyz, 1.0) + sunColor * 0.5 * max(0.0, dot(fragNormal, -sunDirection));
    outColor = light_color * vec4(fragColor, 1.0) * texture(material, fragTexCoord);
}
