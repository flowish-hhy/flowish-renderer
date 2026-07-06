

#include "FlowishDescriptor.h"

#include <stdexcept>

FlowishDescriptor::FlowishDescriptor(VkDevice device)
    : _device(device){
    VkDescriptorSetLayoutBinding b {};
    b.binding = 0;
    b.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    b.descriptorCount = 1;
    b.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = &b
    };

    if (vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    VkDescriptorPoolSize descriptorPoolSize = {
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1
    };
    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &descriptorPoolSize,
    };
    if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = _descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &_descriptorSetLayout
    };
    if (vkAllocateDescriptorSets(_device, &descriptorSetAllocateInfo, &_descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

void FlowishDescriptor::writeUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize size) {
    VkDescriptorBufferInfo descriptorBufferInfo = {
        .buffer = buffer,
        .offset = 0,
        .range = size,
    };
    VkWriteDescriptorSet descriptorWrite = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = _descriptorSet,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .pBufferInfo = &descriptorBufferInfo,
    };
    vkUpdateDescriptorSets(_device, 1, &descriptorWrite, 0, nullptr);


}

FlowishDescriptor::~FlowishDescriptor() {
    if (_descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(_device, _descriptorSetLayout, nullptr);
    }
    if (_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
    }
}

