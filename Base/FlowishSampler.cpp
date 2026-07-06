#include "FlowishSampler.h"

#include <stdexcept>


FlowishSampler::FlowishSampler(VkDevice device)
    : _device(device){

    VkSamplerCreateInfo info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .anisotropyEnable = VK_FALSE,
    };

    if (vkCreateSampler(_device, &info, nullptr, &_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create sampler!");
    }
}

FlowishSampler::~FlowishSampler() {
    if (_sampler != VK_NULL_HANDLE) {
        vkDestroySampler(_device, _sampler, nullptr);
    }
}
