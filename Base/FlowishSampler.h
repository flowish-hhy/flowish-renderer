#pragma once
#include <vulkan/vulkan_core.h>


class FlowishSampler {
public:
    explicit FlowishSampler(VkDevice device);
    ~FlowishSampler();

    [[nodiscard]] VkSampler handle() const { return _sampler; }

private:
    VkDevice _device;
    VkSampler _sampler = VK_NULL_HANDLE;

};
