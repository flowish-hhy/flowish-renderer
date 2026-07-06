

#include "FlowishDescriptor.h"

#include <stdexcept>

void DescriptorLayoutBuilder::clear() {
    _bindings.clear();
}

DescriptorLayoutBuilder& DescriptorLayoutBuilder::addBinding(uint32_t binding, VkDescriptorType descriptorType) {
    VkDescriptorSetLayoutBinding layoutBinding = {
        .binding = binding,
        .descriptorType = descriptorType,
        .descriptorCount = 1,
    };
    _bindings.push_back(layoutBinding);
    return *this;
}

VkDescriptorSetLayout DescriptorLayoutBuilder::build(VkDevice device,
    VkShaderStageFlags stageFlags,
    void * pNext,
    VkDescriptorSetLayoutCreateFlags flags) {

    for (auto& binding : _bindings) {
        binding.stageFlags |= stageFlags;
    }

    VkDescriptorSetLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = pNext,
        .flags = flags,
        .bindingCount = static_cast<uint32_t>(_bindings.size()),
        .pBindings = _bindings.data(),
    };
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    if (vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    return layout;
}

DescriptorAllocator::DescriptorAllocator(VkDevice device, uint32_t initialSets,
                                         std::span<PoolSizeRatio> ratios)
    : _device(device) {
    _ratios.clear();
    for (auto r : ratios) _ratios.push_back(r);

    VkDescriptorPool firstPool = createPool(initialSets, ratios);

    _setsPerPool = static_cast<uint32_t>(initialSets * 1.5f);

    _readyPools.push_back(firstPool);
}

DescriptorAllocator::~DescriptorAllocator() {
    destroyPools();
}

VkDescriptorSet DescriptorAllocator::allocate(VkDescriptorSetLayout layout, void* pNext) {
    VkDescriptorPool pool = getPool();

    VkDescriptorSetAllocateInfo ai{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = pNext,
        .descriptorPool = pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &layout,
    };
    VkDescriptorSet set;
    VkResult r = vkAllocateDescriptorSets(_device, &ai, &set);

    if (r == VK_ERROR_OUT_OF_POOL_MEMORY || r == VK_ERROR_FRAGMENTED_POOL) {
        _fullPools.push_back(pool);
        pool = getPool();
        ai.descriptorPool = pool;
        if (vkAllocateDescriptorSets(_device, &ai, &set) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor set!");
        }
    } else if (r != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor set!");
    }

    _readyPools.push_back(pool);
    return set;
}

VkDescriptorPool DescriptorAllocator::getPool() {
    if (!_readyPools.empty()) {
        VkDescriptorPool p = _readyPools.back();
        _readyPools.pop_back();
        return p;
    }
    VkDescriptorPool p = createPool(_setsPerPool, _ratios);
    _setsPerPool = std::min<uint32_t>(_setsPerPool * 1.5f, 4092);
    return p;
}

VkDescriptorPool DescriptorAllocator::createPool(uint32_t setCount,
                                                std::span<PoolSizeRatio> ratios) {
    std::vector<VkDescriptorPoolSize> poolSizes;
    poolSizes.reserve(ratios.size());
    for (auto r : ratios) {
        poolSizes.push_back({
            .type = r.descriptorType,
            .descriptorCount = static_cast<uint32_t>(r.ratio * setCount),
        });
    }

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .flags         = 0,
        .maxSets       = setCount,
        .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
        .pPoolSizes    = poolSizes.data(),
    };

    VkDescriptorPool pool;
    if (vkCreateDescriptorPool(_device, &poolInfo, nullptr, &pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
    return pool;
}

void DescriptorAllocator::clearPools() {
    for (auto p : _readyPools) {
        vkResetDescriptorPool(_device, p, 0);
    }
    for (auto p : _fullPools) {
        vkResetDescriptorPool(_device, p, 0);
        _readyPools.push_back(p);
    }
    _fullPools.clear();
}

void DescriptorAllocator::destroyPools() {
    for (auto p : _readyPools) vkDestroyDescriptorPool(_device, p, nullptr);
    _readyPools.clear();
    for (auto p : _fullPools)  vkDestroyDescriptorPool(_device, p, nullptr);
    _fullPools.clear();
}

void DescriptorWriter::writeBuffer(uint32_t binding, VkBuffer buffer,
                                   size_t size, size_t offset, VkDescriptorType type) {
    VkDescriptorBufferInfo& info = _bufferInfos.emplace_back(
        VkDescriptorBufferInfo{ .buffer = buffer, .offset = offset, .range = size });

    VkWriteDescriptorSet w{
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = type,
        .pBufferInfo = &info,
    };
    _writes.push_back(w);
}

void DescriptorWriter::writeImage(uint32_t binding, VkImageView imageView, VkSampler sampler, VkImageLayout layout, VkDescriptorType type) {
    VkDescriptorImageInfo& descriptorImageInfo = _imageInfos.emplace_back( VkDescriptorImageInfo{
        .sampler = sampler,
        .imageView = imageView,
        .imageLayout = layout,
    });

    VkWriteDescriptorSet descriptorWrite = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstBinding = binding,
        .descriptorCount = 1,
        .descriptorType = type,
        .pImageInfo = &descriptorImageInfo,
    };
    _writes.push_back(descriptorWrite);
}

void DescriptorWriter::updateSet(VkDevice device, VkDescriptorSet set) {
    for (auto& w : _writes) w.dstSet = set;
    vkUpdateDescriptorSets(device, (uint32_t)_writes.size(),
                           _writes.data(), 0, nullptr);
}

