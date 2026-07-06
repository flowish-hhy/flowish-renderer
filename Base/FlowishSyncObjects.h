#pragma once
#include "FlowishDevice.h"

class FlowishSyncObjects {
public:
    explicit FlowishSyncObjects(VkDevice device);
    ~FlowishSyncObjects();

    [[nodiscard]] VkSemaphore imageAvailable() const { return _imageAvailableSemaphore; }
    [[nodiscard]] VkSemaphore renderFinished() const { return _renderFinishedSemaphore; }
    [[nodiscard]] VkFence getFence() const { return _fence; }

private:
    VkDevice _device;
    VkSemaphore _imageAvailableSemaphore = VK_NULL_HANDLE;
    VkSemaphore _renderFinishedSemaphore = VK_NULL_HANDLE;
    VkFence _fence = VK_NULL_HANDLE;
};
