#pragma once
#include "FlowishDevice.h"

class FlowishPipeline {
public:
    FlowishPipeline(VkDevice _device, VkRenderPass _renderPass, VkShaderModule vert, VkShaderModule frag, VkDescriptorSetLayout layout);
    ~FlowishPipeline();
    FlowishPipeline(const FlowishPipeline&) = delete;
    FlowishPipeline& operator=(const FlowishPipeline&) = delete;




    [[nodiscard]] VkPipeline handle() const { return _pipeline; }
    [[nodiscard]] VkPipelineLayout layout() const { return _pipelineLayout; }
private:
    VkDevice _device;
    VkPipeline _pipeline = VK_NULL_HANDLE;
    VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;
};
