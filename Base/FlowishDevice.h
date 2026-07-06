#pragma once
#include <cstdint>
#include <optional>
#include <vector>

#include "FlowishInstance.h"


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    [[nodiscard]] bool complete() const {return graphicsFamily.has_value() && presentFamily.has_value();}
};


class FlowishDevice {
public:
    FlowishDevice(VkInstance instance, VkSurfaceKHR surface);
    ~FlowishDevice();

    FlowishDevice(const FlowishDevice&) = delete;
    FlowishDevice& operator=(const FlowishDevice&) = delete;

    [[nodiscard]] const QueueFamilyIndices& queueFamilyIndices() const {return _indices;}

    [[nodiscard]] VkQueue graphicsQueue() const {return _graphicsQueue;}
    [[nodiscard]] VkQueue presentQueue() const {return _presentQueue;}
    [[nodiscard]] VkDevice device() const {return _device;}
    [[nodiscard]] VkPhysicalDevice physicalDevice() const {return _physical;}


private:
    void pickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
    void createLogicalDevice();
    VkSurfaceKHR _surface;
    VkPhysicalDevice _physical = VK_NULL_HANDLE;
    VkDevice _device = VK_NULL_HANDLE;
    QueueFamilyIndices _indices;
    VkQueue _graphicsQueue = VK_NULL_HANDLE;
    VkQueue _presentQueue = VK_NULL_HANDLE;

};