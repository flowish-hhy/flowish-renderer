#pragma once
#include <vulkan/vulkan_core.h>

#include "GLFW/glfw3.h"

class FlowishSurface {
public:
    FlowishSurface(VkInstance instance, GLFWwindow* window);
    ~FlowishSurface();

    FlowishSurface(const FlowishSurface&) = delete;
    FlowishSurface& operator=(const FlowishSurface&) = delete;

    [[nodiscard]] VkSurfaceKHR handle() const { return _surface; }
private:
    VkInstance _instance;
    VkSurfaceKHR _surface = VK_NULL_HANDLE;
};


