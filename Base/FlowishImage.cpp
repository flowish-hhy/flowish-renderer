
#include "FlowishImage.h"

#include <stdexcept>

#include "../Utils/Utils.h"

FlowishImage::FlowishImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspect)
    :_device(device){

    VkImageCreateInfo imageInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {width,height,1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    if (vkCreateImage(device, &imageInfo, nullptr, &_image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements imageMemoryRequirementsInfo = {};
    vkGetImageMemoryRequirements(device,_image,&imageMemoryRequirementsInfo);

    uint32_t index = findMemoryType(physicalDevice,imageMemoryRequirementsInfo.memoryTypeBits,properties);
    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = imageMemoryRequirementsInfo.size;
    allocInfo.memoryTypeIndex = index;
    if (vkAllocateMemory(device,&allocInfo,nullptr,&_memory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    vkBindImageMemory(device,_image,_memory,0);

    VkImageViewCreateInfo imageViewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = _image,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .subresourceRange = {
            .aspectMask = aspect,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        }
    };

    if (vkCreateImageView(device,&imageViewInfo,nullptr,&_imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

}


FlowishImage::~FlowishImage() {
    if (_imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(_device,_imageView,nullptr);
    }
    if (_image != VK_NULL_HANDLE) {
        vkDestroyImage(_device,_image,nullptr);
    }
    if (_memory != VK_NULL_HANDLE) {
        vkFreeMemory(_device,_memory,nullptr);
    }
}
