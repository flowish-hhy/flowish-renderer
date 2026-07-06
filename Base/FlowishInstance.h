#pragma once
#include <vulkan/vulkan.h>


class FlowishInstance {
public:
    FlowishInstance();
    ~FlowishInstance();

    FlowishInstance(const FlowishInstance&) = delete;
    FlowishInstance& operator=(const FlowishInstance&) = delete;

    [[nodiscard]] VkInstance handle() const { return _instance; }

private:
    VkInstance _instance = VK_NULL_HANDLE;
};