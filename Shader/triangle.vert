#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 normalMatrix;
    vec4 lightPos;
    vec4 viewPos;
    vec4 lightColor;
} ubo;

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec2 fragUV;
layout(location = 1) out vec3 fragWorldPos;
layout(location = 2) out vec3 fragWorldNormal;

void main() {
    vec4 worldPos = ubo.model * vec4(inPos, 1.0);
    gl_Position     = ubo.proj * ubo.view * worldPos;
    fragUV          = inUV;
    fragWorldPos    = worldPos.xyz;
    fragWorldNormal = mat3(ubo.normalMatrix) * inNormal;
}
