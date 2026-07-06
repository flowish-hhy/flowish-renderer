
#include "FlowishSyncObjects.h"

#include <stdexcept>

FlowishSyncObjects::FlowishSyncObjects(VkDevice device)
    :_device(device){
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr , &_renderFinishedSemaphore) != VK_SUCCESS
        || vkCreateSemaphore(_device, &semaphoreInfo, nullptr , &_imageAvailableSemaphore) != VK_SUCCESS
        || vkCreateFence(_device, &fenceInfo, nullptr , &_fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence");
    }
}

FlowishSyncObjects::~FlowishSyncObjects() {
    if (_renderFinishedSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(_device, _renderFinishedSemaphore, nullptr);
    }
    if (_imageAvailableSemaphore != VK_NULL_HANDLE) {
        vkDestroySemaphore(_device, _imageAvailableSemaphore, nullptr);
    }
    if (_fence != VK_NULL_HANDLE) {
        vkDestroyFence(_device, _fence, nullptr);
    }
}

