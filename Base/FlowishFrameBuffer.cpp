#include "FlowishFrameBuffer.h"

#include <stdexcept>

FlowishFramebuffers::FlowishFramebuffers(VkDevice device, VkRenderPass renderPass, const std::vector<VkImageView> &imageViews, VkExtent2D extent)
    : _device(device){
    _framebuffers.resize(imageViews.size());
    for (int i = 0; i < imageViews.size(); i++) {
        VkFramebufferCreateInfo framebufferCreateInfo = {};
        framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferCreateInfo.renderPass = renderPass;
        framebufferCreateInfo.width = extent.width;
        framebufferCreateInfo.height = extent.height;
        framebufferCreateInfo.layers = 1;
        framebufferCreateInfo.attachmentCount = 1;
        framebufferCreateInfo.pAttachments = &imageViews[i];
        if (vkCreateFramebuffer(_device, &framebufferCreateInfo, nullptr, &_framebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer");
        }
    }
}

FlowishFramebuffers::~FlowishFramebuffers() {
    for (int i = 0; i < _framebuffers.size(); i++) {
        if (_framebuffers[i] != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(_device, _framebuffers[i], nullptr);
        }
    }
}



