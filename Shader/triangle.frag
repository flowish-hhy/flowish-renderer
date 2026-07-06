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

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragUV;
layout(location = 1) in vec3 fragWorldPos;
layout(location = 2) in vec3 fragWorldNormal;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 N = normalize(fragWorldNormal);
    vec3 L = normalize(ubo.lightPos.xyz - fragWorldPos);
    vec3 V = normalize(ubo.viewPos.xyz  - fragWorldPos);
    vec3 H = normalize(L + V);

    vec3 baseColor = texture(texSampler, fragUV).rgb;

    float ambient  = 0.1;
    float diffuse  = max(dot(N, L), 0.0);
    float specular = pow(max(dot(N, H), 0.0), 32.0);

    vec3 color = (ambient + diffuse) * baseColor * ubo.lightColor.rgb
               + specular * ubo.lightColor.rgb;

    outColor = vec4(color, 1.0);
}
