#pragma once
#include "vulkan/vulkan.h"

class FlowishBuffer {
public:

    FlowishBuffer(VkPhysicalDevice physicalDevice, VkDevice device,
        VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    ~FlowishBuffer();


    [[nodiscard]] VkBuffer handle() const { return _buffer; };
    [[nodiscard]] VkDeviceMemory memory() const { return _memory; };
    [[nodiscard]] VkDeviceSize size() const {return _size; };

private:
\

    VkBuffer _buffer = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;
    VkDeviceSize _size = 0;

    VkDevice _device = VK_NULL_HANDLE;
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
};
