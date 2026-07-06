#pragma once
#include <vector>

#include "FlowishDevice.h"
#include "GLFW/glfw3.h"

class FlowishSwapchain {
public:
    FlowishSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow* window, const QueueFamilyIndices& indices);
    ~FlowishSwapchain();
    FlowishSwapchain(const FlowishSwapchain&) = delete;
    FlowishSwapchain& operator=(const FlowishSwapchain&) = delete;


    [[nodiscard]] VkFormat format() const { return _swapchainImageFormat; }
    [[nodiscard]] VkExtent2D extent() const { return _swapchainExtent; }
    [[nodiscard]] const std::vector<VkImageView>& imageViews() const { return _swapchainImageViews; }
    [[nodiscard]] VkSwapchainKHR handle() const { return _swapchain; }

private:


    VkDevice _device;
    VkSwapchainKHR _swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;
    VkFormat _swapchainImageFormat = VK_FORMAT_UNDEFINED;
    VkExtent2D _swapchainExtent = {};
};

