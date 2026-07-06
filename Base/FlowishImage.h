#pragma once
#include "FlowishDevice.h"

class FlowishImage {
public:
    FlowishImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspect);
    ~FlowishImage();

    [[nodiscard]] VkImage image() const { return _image; }
    [[nodiscard]] VkImageView view() const { return _imageView; }

private:
    VkDevice _device;
    VkImage _image = VK_NULL_HANDLE;
    VkImageView _imageView = VK_NULL_HANDLE;
    VkDeviceMemory _memory = VK_NULL_HANDLE;

};
