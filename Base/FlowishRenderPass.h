#pragma once
#include "FlowishDevice.h"

class FlowishRenderPass {

public:
    FlowishRenderPass(VkDevice device,VkFormat format);
    ~FlowishRenderPass();
    FlowishRenderPass(const FlowishRenderPass&) = delete;
    FlowishRenderPass& operator=(const FlowishRenderPass&) = delete;


    [[nodiscard]] VkRenderPass handle() const { return _renderPass; }

private:
    VkDevice _device;
    VkRenderPass _renderPass = VK_NULL_HANDLE;
};
