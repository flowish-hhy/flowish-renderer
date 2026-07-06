#pragma once
#include <span>
#include <deque>
#include <vector>
#include "FlowishDevice.h"

class DescriptorLayoutBuilder {
public:

    VkDescriptorSetLayout build(VkDevice device,
        VkShaderStageFlags stageFlags,
        void * pNext = nullptr,
        VkDescriptorSetLayoutCreateFlags flags = 0);
    void clear();
    DescriptorLayoutBuilder& addBinding(uint32_t binding, VkDescriptorType descriptorType);
private:
    std::vector<VkDescriptorSetLayoutBinding> _bindings;
};

class DescriptorAllocator {
public:
    struct PoolSizeRatio {
        VkDescriptorType descriptorType;
        float ratio;
    };

    DescriptorAllocator(VkDevice device, uint32_t initialSets,
                        std::span<PoolSizeRatio> poolSizeRatios);
    ~DescriptorAllocator();

    DescriptorAllocator(const DescriptorAllocator&) = delete;
    DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;

    void clearPools();
    VkDescriptorSet allocate(VkDescriptorSetLayout layout, void *pNext = nullptr);

private:
    VkDescriptorPool getPool();
    VkDescriptorPool createPool(uint32_t setCount, std::span<PoolSizeRatio> poolSizeRatios);
    void destroyPools();

    VkDevice _device = VK_NULL_HANDLE;
    std::vector<PoolSizeRatio> _ratios;
    std::vector<VkDescriptorPool> _fullPools;
    std::vector<VkDescriptorPool> _readyPools;
    uint32_t _setsPerPool = 0;
};


class DescriptorWriter {
public:
    void writeBuffer(uint32_t binding, VkBuffer buffer,
                     size_t size, size_t offset, VkDescriptorType type);
    void writeImage(uint32_t binding, VkImageView view, VkSampler sampler,
                    VkImageLayout layout, VkDescriptorType type);
    void clear();
    void updateSet(VkDevice device, VkDescriptorSet set);
private:
    std::deque<VkDescriptorBufferInfo> _bufferInfos;
    std::deque<VkDescriptorImageInfo>  _imageInfos;
    std::vector<VkWriteDescriptorSet>  _writes;
};

