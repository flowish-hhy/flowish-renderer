
#include "FlowishShaderModule.h"

#include <stdexcept>
#include <vector>

FlowishShaderModule::FlowishShaderModule(VkDevice device, const std::vector<char>& code)
    : _device(device){
    VkShaderModuleCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = code.size();
    info.pCode = reinterpret_cast<const uint32_t*>(code.data());
    if (vkCreateShaderModule(_device, &info, NULL, &_shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
}

FlowishShaderModule::~FlowishShaderModule() {
    if (_shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(_device, _shaderModule, NULL);
    }
}


