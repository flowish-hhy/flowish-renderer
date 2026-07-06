#include "FlowishSwapchain.h"

#include <algorithm>
#include <stdexcept>

VkSurfaceFormatKHR findSuitableSurface(std::vector<VkSurfaceFormatKHR>& surfaceFormats) {
    for (VkSurfaceFormatKHR surfaceFormat : surfaceFormats) {
        if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB
            && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return surfaceFormat;
        }
    }
    return surfaceFormats[0];
}

VkPresentModeKHR findPresentMode(std::vector<VkPresentModeKHR> presentModes) {
    for (VkPresentModeKHR presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}



FlowishSwapchain::FlowishSwapchain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow *window, const QueueFamilyIndices &indices)
    :_device(logicalDevice){

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice,surface,&surfaceCapabilities);

    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,surface,&count,nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,surface,&count,formats.data());

    count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,surface,&count,nullptr);
    std::vector<VkPresentModeKHR> presentModes(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,surface,&count,presentModes.data());

    VkSurfaceFormatKHR surfaceFormat = findSuitableSurface(formats);
    VkPresentModeKHR presentMode = findPresentMode(presentModes);

    VkExtent2D swapchainExtent;
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
        swapchainExtent = surfaceCapabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        width = std::clamp(width, (int) surfaceCapabilities.minImageExtent.width, (int) surfaceCapabilities.maxImageExtent.width);
        height = std::clamp(height, (int)surfaceCapabilities.minImageExtent.height, (int) surfaceCapabilities.maxImageExtent.height);
        swapchainExtent = VkExtent2D(width, height);
    }

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = surfaceCapabilities.minImageCount;
    swapchainInfo.imageFormat = surfaceFormat.format;
    swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainInfo.imageExtent = swapchainExtent;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (indices.graphicsFamily != indices.presentFamily) {
        uint32_t queueFamilyIndices[] = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value(),
        };
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.queueFamilyIndexCount = 0;
        swapchainInfo.pQueueFamilyIndices = nullptr;
    }

    swapchainInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.presentMode = presentMode;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

    _swapchainImageFormat = surfaceFormat.format;
    _swapchainExtent = swapchainExtent;

    if (vkCreateSwapchainKHR(_device,&swapchainInfo,nullptr, &_swapchain)!=VK_SUCCESS) {
        throw std::runtime_error("failed to create swapchain");
    }

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(_device,_swapchain,&imageCount,nullptr);
    _swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(_device,_swapchain,&imageCount,_swapchainImages.data());

    for (auto & image : _swapchainImages) {
        VkImageViewCreateInfo imageViewInfo = {};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.image = image;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = surfaceFormat.format;
        imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        VkImageView imageView = VK_NULL_HANDLE;
        if (vkCreateImageView(_device,&imageViewInfo,nullptr,&imageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image view");
        }
        _swapchainImageViews.push_back(imageView);
    }

}

FlowishSwapchain::~FlowishSwapchain() {

    for (auto &imageView : _swapchainImageViews) {
        vkDestroyImageView(_device,imageView,nullptr);
    }

    if (_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(_device, _swapchain, nullptr);
    }
}
