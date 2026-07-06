#pragma once
#include <vulkan/vulkan_core.h>

#include "FlowishDevice.h"

class FlowishCommandPool {
public:
    FlowishCommandPool(VkDevice device, QueueFamilyIndices indices);
    ~FlowishCommandPool();

    [[nodiscard]] VkCommandPool handle() const { return _cmdPool; }
    [[nodiscard]] VkCommandBuffer commandBuffer() const { return _cmdBuffer; }

private:
    VkCommandPool _cmdPool = VK_NULL_HANDLE;
    VkDevice _device;
    VkCommandBuffer _cmdBuffer = VK_NULL_HANDLE;
};
