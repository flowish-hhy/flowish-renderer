
#include "FlowishBuffer.h"

#include <stdexcept>

#include "../Utils/Utils.h"

FlowishBuffer::FlowishBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
    : _device(device), _physicalDevice(physicalDevice), _size(size){

    VkBufferCreateInfo bufferInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    if (vkCreateBuffer(_device, &bufferInfo, nullptr, &_buffer)) {
        throw std::runtime_error("failed to create vertex buffer");
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device, _buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(_physicalDevice,memRequirements.memoryTypeBits, properties);
    if (vkAllocateMemory(_device, &allocInfo, nullptr, &_memory)) {
        throw std::runtime_error("failed to allocate vertex buffer memory");
    }

    vkBindBufferMemory(_device, _buffer, _memory, 0);

}


FlowishBuffer::~FlowishBuffer() {
    if (_buffer) { vkDestroyBuffer(_device, _buffer, nullptr); }
    if (_memory) { vkFreeMemory(_device, _memory, nullptr); }
}

