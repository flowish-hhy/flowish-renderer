#pragma once
#include "FlowishDevice.h"

class FlowishDescriptor {
public:
    explicit FlowishDescriptor(VkDevice device);
    ~FlowishDescriptor();

    [[nodiscard]] VkDescriptorSetLayout layout() const {return _descriptorSetLayout;}
    [[nodiscard]] VkDescriptorSet set() const {return _descriptorSet;}
    void writeUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize size);

private:
    VkDevice _device = VK_NULL_HANDLE;
    VkDescriptorSetLayout _descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool _descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet _descriptorSet = VK_NULL_HANDLE;

};
