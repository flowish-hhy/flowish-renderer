#pragma once
#include <array>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {
        .binding = 0,
        .stride = static_cast<uint32_t>(sizeof(Vertex)),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };
    return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
    return {{
        {.location = 0, .binding = 0,
            .format = VK_FORMAT_R32G32_SFLOAT,
            .offset = static_cast<uint32_t>(offsetof(Vertex, pos))},
        {.location = 1, .binding = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .offset = static_cast<uint32_t>(offsetof(Vertex, color))},
    }};
}


struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};
