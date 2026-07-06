#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>



class FlowishShaderModule {
public:
    FlowishShaderModule(VkDevice device, const std::vector<char>& code);
    ~FlowishShaderModule();
    FlowishShaderModule(const FlowishShaderModule&) = delete;
    FlowishShaderModule& operator=(const FlowishShaderModule&) = delete;


    [[nodiscard]] VkShaderModule handle() const { return _shaderModule; }

private:
    VkShaderModule _shaderModule = VK_NULL_HANDLE;
    VkDevice _device;
};
