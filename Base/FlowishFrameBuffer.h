#pragma once
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include "FlowishDevice.h"

class FlowishFramebuffers {
public:
    FlowishFramebuffers(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView>& imageView, VkExtent2D extent, VkImageView depthImage);
    ~FlowishFramebuffers();
    VkFramebuffer get(uint32_t index) { return _framebuffers[index]; };

private:
    std::vector<VkFramebuffer> _framebuffers;
    VkDevice _device;

};
