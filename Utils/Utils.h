#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "vulkan/vulkan.h"

inline std::vector<char> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file " + path);
    }
    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void copyBuffer(VkDevice device, VkCommandPool pool, VkQueue queue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
uint32_t findMemoryType(VkPhysicalDevice _physicalDevice, uint32_t typeBits, VkMemoryPropertyFlags properties);

