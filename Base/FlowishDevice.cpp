#include "FlowishDevice.h"

#include <set>
#include <stdexcept>
#include <vector>

const std::vector<const char *> deviceRequiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

std::vector<VkExtensionProperties> enumerateDeviceExtensionProperties(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, VK_NULL_HANDLE, &extensionCount, extensions.data());
    return extensions;
}

bool supportedRequiredExtensions(VkPhysicalDevice device) {
    auto available = enumerateDeviceExtensionProperties(device);
    std::set<std::string> required(deviceRequiredExtensions.begin(), deviceRequiredExtensions.end());
    for (const auto &extension : available) {
        required.erase(extension.extensionName);
    }

    return required.empty();
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;
    uint32_t graphicsFamiliesCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &graphicsFamiliesCount, NULL);
    std::vector<VkQueueFamilyProperties> FamilyProperties(graphicsFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &graphicsFamiliesCount, FamilyProperties.data());

    for (uint32_t i = 0; i < graphicsFamiliesCount; i++) {
        if (FamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }
        if (indices.complete()) {
            break;
        }
    }
    return indices;
}

void FlowishDevice::pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);
    if (physicalDeviceCount == 0) {
        throw std::runtime_error("No physical devices found");
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

    for (VkPhysicalDevice physicalDevice : physicalDevices) {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
        if (indices.complete() && supportedRequiredExtensions(physicalDevice)) {
            _physical = physicalDevice;
            _indices = indices;
            return;
        }
    }
    throw std::runtime_error("No suitable GPU found");
}

void FlowishDevice::createLogicalDevice() {
    std::set<uint32_t> uniqueFamilies = {
        _indices.graphicsFamily.value(),
        _indices.presentFamily.value(),
    };
    float priority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    for (uint32_t family : uniqueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.queueFamilyIndex = family;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceRequiredExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceRequiredExtensions.data();

    if (vkCreateDevice(_physical, &createInfo, nullptr, &_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device");
    }

    vkGetDeviceQueue(_device, _indices.graphicsFamily.value(), 0 , &_graphicsQueue);
    vkGetDeviceQueue(_device, _indices.presentFamily.value(), 0 , &_presentQueue);

}



FlowishDevice::FlowishDevice(VkInstance instance, VkSurfaceKHR surface)
    : _surface(surface){
    pickPhysicalDevice(instance, surface);
    createLogicalDevice();
}


FlowishDevice::~FlowishDevice() {
    if (_device != VK_NULL_HANDLE) {
        vkDestroyDevice(_device, nullptr);
    }
}

